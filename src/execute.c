#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <ntstatus.h>
#include <bcrypt.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#include "exec_stack.h"
#include "bytecode.h"

#include "jv_alloc.h"
#include "jq_parser.h"
#include "locfile.h"
#include "jv.h"
#include "jq.h"
#include "jq_plugin.h"
#include "parser.h"
#include "builtin.h"
#include "util.h"
#include "linker.h"

struct jq_plugin_vtable vtable;

struct raw_handle {
  struct jq_io_table *vt;
  void *handle;
  int refcnt;
};

struct jq_handles {
  jv jhandles;
  struct raw_handle **handles;
  size_t nhandles;
};

enum bt_priority {
  BT_PRIO_NONE = 0,

  // This is a signal matched by a tag
  // the tag may encode an instruction
  // or a pc offset
  // special case of tag 0 is reserved
  // for a simple data backtracking.
  BT_PRIO_SIGNAL,

  // unwind protect handlers
  BT_PRIO_PROTECT,

  // ===============
  // this should be the max priority
  BT_PRIO_ALWAYS,
  // ===============

  // terminate is a kind of mode 
  // which is set when unwinding before termination
  BT_PRIO_TERMINATE = BT_PRIO_ALWAYS,
};

#define BT_PRIO_BITS 2

#if (BT_PRIO_ALWAYS >= (1<<BT_PRIO_BITS)) 
  #error "bt_priority should fit in " #BT_PRIO_BITS " bits"
#endif

#define BT_TAG_BITS (32 - BT_PRIO_BITS)
#define BT_MAX_TAG ((1 << BT_TAG_BITS) - 1)

// Backtracking tag space is broken into the following 16 bit regions:
// 0x00000 - 0x0ffff  -  opcode space + hard tags
// 0x10000 - 0x1ffff  -  soft tags
// 0x20000 - ....   pc tag, where the lower 16 bits are the offset and the higher bits is level + 2

#define BT_TAG_HARD_START NUM_OPCODES
#define BT_TAG_SOFT_START 0x10000
#define BT_TAG_PC_START   0x20000

#define BT_TAG_IS_OPCODE(tag)     ((tag)<NUM_OPCODES)
#define BT_TAG_IS_HARD(tag)       ((tag)>=BT_TAG_HARD_START && (tag)<BT_TAG_SOFT_START)
#define BT_TAG_IS_SOFT(tag)       ((tag)>=BT_TAG_SOFT_START && (tag)<BT_TAG_PC_START)
#define BT_TAG_IS_FRAME_PC(tag)   ((tag)>=BT_TAG_PC_START)

enum bt_hard_tag {
  // should be the first member here
  __hard_tag_start = (BT_TAG_HARD_START-1),

  // should be the last member here
  __hard_tag_end,
};

#if ( __hard_tag_end >= BT_TAG_SOFT_START)
#   error "Too many hard tags"
#endif

#define BT_TAG_SOFT(tag)                (BT_TAG_SOFT_START + (tag))
#define BT_TAG_FRAME_PC(level,offset)   (BT_TAG_PC_START + ((level)<<16) + (offset))

// returns -1 if the tag is not soft, of the soft tag itself.
// NB: 0 is a valid soft tag
#define BT_TAG_GET_SOFT(tag)        (MAX(-1, (int)((tag) - BT_TAG_SOFT_START)))
#define BT_TAG_GET_PC_FRAME(tag)    (MAX(-1, (int)((tag) - BT_TAG_PC_START) >> 16))
#define BT_TAG_GET_PC_OFFSET(tag)   ((tag) & 0xFFFF)

union bt_descriptor {
  uint32_t value;
  struct {
    uint32_t tag: BT_TAG_BITS;
    enum bt_priority prio: BT_PRIO_BITS;
  } fields;
};

static inline enum bt_priority bt_desc_prio(uint32_t desc_val) {
  union bt_descriptor desc = {.value=desc_val};
  return desc.fields.prio;
}

static inline uint32_t bt_desc_tag(uint32_t desc_val) {
  union bt_descriptor desc = {.value=desc_val};
  return desc.fields.tag;
}

static inline uint32_t bt_desc_make(enum bt_priority prio, uint32_t tag) {
  union bt_descriptor desc = {.fields={tag, prio}};
  return desc.value;
}

#define BT_DESC(tag)                bt_desc_make(BT_PRIO_SIGNAL, tag)
#define BT_DESC_SOFT(soft_tag)      bt_desc_make(BT_PRIO_SIGNAL, BT_SOFT_TAG(soft_tag))

#define BT_DESC_NEXT_VALUE          BT_DESC(BACKTRACK_0)
#define BT_DESC_ERROR               BT_DESC(RAISE)

#define BT_DESC_FRAME_PC(level, offset)   bt_desc_make(BT_PRIO_SIGNAL, BT_TAG_FRAME_PC(level, offset))
#define BT_DESC_PROTECT                   bt_desc_make(BT_PRIO_PROTECT, 0)
#define BT_DESC_TERMINATE                 bt_desc_make(BT_PRIO_TERMINATE, 0)
#define BT_DESC_ALWAYS                    bt_desc_make(BT_PRIO_ALWAYS, 0)

struct bt_state {
  uint32_t desc;
  uint32_t ttl;
  jv payload;
};

struct jq_state {
  struct jq_plugin_vtable *vtable; /* Must be first! */
  void (*nomem_handler)(void *);
  void *nomem_handler_data;
  struct bytecode* bc;
  struct lib_loading_state *libs;

  jq_msg_cb err_cb;
  void *err_cb_data;
  jv error;
  
  struct bt_state bt;

  struct stack stk;
  stack_ptr curr_frame;
  stack_ptr stk_top;
  stack_ptr fork_top;

  jv path;
  jv value_at_path;
  int subexp_nest;
  int debug_trace_enabled;
  int initial_execution;
  unsigned next_label;

  unsigned int halted;
  unsigned int finished;
  jv exit_code;
  jv error_message;

  jv vmid;
  jv rnd;
  jv attrs;
  jq_input_cb input_cb;
  void *input_cb_data;
  jq_msg_cb debug_cb;
  void *debug_cb_data;

  /*
   * backward compatible start input
   * instead of storing it on the stack
   */
  jv start_input;
  jq_input_mode input_mode;

  /*
   * I/O and co-routine handles (futures).
   *
   * Once we have co-routines we'll have a single handle namespace
   * for all co-routines in a family.  To avoid an extra allocation
   * in jq_init(), we'll have a struct jq_handles and a pointer to it
   * or an ancestor co-routine's jq_state's handles.
   */
  struct jq_handles handles_s;
  struct jq_handles *handles;

  /*
   * Co-routines
   *
   *  - parent points to the parent jq_state
   *  - restore_limit is the point past which the stack is shared with
   *    the parent, thus the child is not to unwind further than this
   *    point and the parent must teardown the child before unwinding
   *    past point
   *  - start_limit is the point to unwind to in order to reset a child
   *    co-routine so it can be restarted
   */
  struct jq_state* parent;
  stack_ptr restore_limit;
  uint16_t* start_pc;

  /*
   * I/O policy
   */
  struct jq_state* io_policy;
  jv io_policy_data;
};

struct closure {
  struct bytecode* bc;  // jq bytecode
  stack_ptr env;        // jq stack address of closed frame
};

// locals for any function called: either a closure or a local variable
union frame_entry {
  struct closure closure;
  jv localvar;
};

// jq function call frame
struct frame {
  struct bytecode* bc;      // jq bytecode for callee
  stack_ptr env;            // jq stack address of frame to return to
  stack_ptr retdata;        // jq stack address to unwind to on RET_JQ
  uint16_t* retaddr;        // jq bytecode return address
  union frame_entry entries[]; // nclosures + nlocals
};

static int frame_size(struct bytecode* bc) {
  return sizeof(struct frame) + sizeof(union frame_entry) * (bc->nclosures + bc->nlocals);
}

static struct frame* frame_current(struct jq_state* jq) {
  struct frame* fp = stack_block(&jq->stk, jq->curr_frame);

  stack_ptr next = *stack_block_next(&jq->stk, jq->curr_frame);
  if (next) {
    struct frame* fpnext = stack_block(&jq->stk, next);
    struct bytecode* bc = fpnext->bc;
    assert(fp->retaddr >= bc->code && fp->retaddr < bc->code + bc->codelen);
  } else {
    assert(fp->retaddr == 0);
  }
  return fp;
}

static stack_ptr frame_get_level(struct jq_state* jq, int level) {
  stack_ptr fr = jq->curr_frame;
  for (int i=0; i<level; i++) {
    struct frame* fp = stack_block(&jq->stk, fr);
    fr = fp->env;
  }
  return fr;
}

static jv* frame_local_var(struct jq_state* jq, int var, int level) {
  struct frame* fr = stack_block(&jq->stk, frame_get_level(jq, level));
  assert(var >= 0);
  assert(var < fr->bc->nlocals);
  return &fr->entries[fr->bc->nclosures + var].localvar;
}

static struct closure make_closure(struct jq_state* jq, uint16_t* pc) {
  uint16_t level = *pc++;
  uint16_t idx = *pc++;
  stack_ptr fridx = frame_get_level(jq, level);
  struct frame* fr = stack_block(&jq->stk, fridx);
  if (idx & ARG_NEWCLOSURE) {
    // A new closure closing the frame identified by level, and with
    // the bytecode body of the idx'th subfunction of that frame
    int subfn_idx = idx & ~ARG_NEWCLOSURE;
    assert(subfn_idx < fr->bc->nsubfunctions);
    struct closure cl = {fr->bc->subfunctions[subfn_idx],
                         fridx};
    return cl;
  } else {
    // A reference to a closure from the frame identified by level; copy
    // it as-is
    int closure = idx;
    assert(closure >= 0);
    assert(closure < fr->bc->nclosures);
    return fr->entries[closure].closure;
  }
}

static struct frame* frame_push(struct jq_state* jq, struct closure callee,
                                uint16_t* argdef, int nargs) {
  stack_ptr new_frame_idx = stack_push_block(&jq->stk, jq->curr_frame, frame_size(callee.bc));
  struct frame* new_frame = stack_block(&jq->stk, new_frame_idx);
  new_frame->bc = callee.bc;
  new_frame->env = callee.env;
  assert(nargs == new_frame->bc->nclosures);
  union frame_entry* entries = new_frame->entries;
  for (int i=0; i<nargs; i++) {
    entries->closure = make_closure(jq, argdef + i * 2);
    entries++;
  }
  for (int i=0; i<callee.bc->nlocals; i++) {
    entries->localvar = jv_invalid();
    entries++;
  }
  jq->curr_frame = new_frame_idx;
  return new_frame;
}

static void frame_pop(struct jq_state* jq) {
  assert(jq->curr_frame);
  struct frame* fp = frame_current(jq);
  if (stack_pop_will_free(&jq->stk, jq->curr_frame)) {
    int nlocals = fp->bc->nlocals;
    for (int i=0; i<nlocals; i++) {
      jv_free(*frame_local_var(jq, i, 0));
    }
  }
  jq->curr_frame = stack_pop_block(&jq->stk, jq->curr_frame, frame_size(fp->bc));
}

void stack_push(jq_state *jq, jv val) {
  assert(jv_is_valid(val));
  jq->stk_top = stack_push_block(&jq->stk, jq->stk_top, sizeof(jv));
  jv* sval = stack_block(&jq->stk, jq->stk_top);
  *sval = val;
}

jv stack_pop(jq_state *jq) {
  jv* sval = stack_block(&jq->stk, jq->stk_top);
  jv val = *sval;
  if (!stack_pop_will_free(&jq->stk, jq->stk_top)) {
    val = jv_copy(val);
  }
  jq->stk_top = stack_pop_block(&jq->stk, jq->stk_top, sizeof(jv));
  assert(jv_is_valid(val));
  return val;
}

// Like stack_pop(), but assert !stack_pop_will_free() and replace with
// jv_null() on the stack.
jv stack_popn(jq_state *jq) {
  jv* sval = stack_block(&jq->stk, jq->stk_top);
  jv val = *sval;
  if (!stack_pop_will_free(&jq->stk, jq->stk_top)) {
    *sval = jv_null();
  }
  jq->stk_top = stack_pop_block(&jq->stk, jq->stk_top, sizeof(jv));
  assert(jv_is_valid(val));
  return val;
}


struct forkpoint {
  stack_ptr saved_data_stack;
  stack_ptr saved_curr_frame;
  int path_len, subexp_nest;
  jv value_at_path;
  uint16_t* return_address;
  uint32_t bt_desc;
};

struct stack_pos {
  stack_ptr saved_data_stack, saved_curr_frame;
};

struct stack_pos stack_get_pos(jq_state* jq) {
  struct stack_pos sp = {jq->stk_top, jq->curr_frame};
  return sp;
}

void stack_save(jq_state *jq, uint32_t bt_desc, uint16_t* retaddr, struct stack_pos sp){
  jq->fork_top = stack_push_block(&jq->stk, jq->fork_top, sizeof(struct forkpoint));
  struct forkpoint* fork = stack_block(&jq->stk, jq->fork_top);
  fork->saved_data_stack = jq->stk_top;
  fork->saved_curr_frame = jq->curr_frame;
  fork->path_len =
    jv_get_kind(jq->path) == JV_KIND_ARRAY ? jv_array_length(jv_copy(jq->path)) : 0;
  fork->value_at_path = jv_copy(jq->value_at_path);
  fork->subexp_nest = jq->subexp_nest;
  fork->return_address = retaddr;
  fork->bt_desc = bt_desc;
  jq->stk_top = sp.saved_data_stack;
  jq->curr_frame = sp.saved_curr_frame;
}

static int path_intact(jq_state *jq, jv curr) {
  if (jq->subexp_nest == 0 && jv_get_kind(jq->path) == JV_KIND_ARRAY) {
    return jv_identical(curr, jv_copy(jq->value_at_path));
  } else {
    jv_free(curr);
    return 1;
  }
}

static void path_append(jq_state* jq, jv component, jv value_at_path) {
  if (jq->subexp_nest == 0 && jv_get_kind(jq->path) == JV_KIND_ARRAY) {
    int n1 = jv_array_length(jv_copy(jq->path));
    jq->path = jv_array_append(jq->path, component);
    int n2 = jv_array_length(jv_copy(jq->path));
    assert(n2 == n1 + 1);
    jv_free(jq->value_at_path);
    jq->value_at_path = value_at_path;
  } else {
    jv_free(component);
    jv_free(value_at_path);
  }
}

static void copy_callbacks(jq_state *src, jq_state *dst) {
  dst->err_cb = src->err_cb;
  dst->err_cb_data = src->err_cb_data;
  dst->input_cb = src->input_cb;
  dst->input_cb_data = src->input_cb_data;
  dst->debug_cb = src->debug_cb;
  dst->debug_cb_data = src->debug_cb_data;
}

/* For f_getpath() */
jv _jq_path_append(jq_state *jq, jv v, jv p, jv value_at_path) {
  if (jq->subexp_nest != 0 ||
      jv_get_kind(jq->path) != JV_KIND_ARRAY ||
      !jv_is_valid(value_at_path)) {
    jv_free(v);
    jv_free(p);
    return value_at_path;
  }
  if (!jv_identical(v, jv_copy(jq->value_at_path))) {
    jv_free(p);
    return value_at_path;
  }
  if (jv_get_kind(p) == JV_KIND_ARRAY)
    jq->path = jv_array_concat(jq->path, p);
  else
    jq->path = jv_array_append(jq->path, p);
  jv_free(jq->value_at_path);
  return jv_copy(jq->value_at_path = value_at_path);
}

static void jq_reset_bt(jq_state *jq, uint32_t bt_desc) {
  jq->bt.desc = bt_desc;
  jv_free(jq->bt.payload);
  jq->bt.payload = jv_invalid();
  jq->bt.ttl = 0;
}

uint16_t* stack_restore(jq_state *jq){

  assert(bt_desc_prio(jq->bt.desc) > BT_PRIO_NONE && "stack_restore called when not backtracking");

  // figure out the target pc, if applicable
  int frame_level = BT_TAG_GET_PC_FRAME(bt_desc_tag(jq->bt.desc));
  uint16_t* target_pc = 0;

  if (frame_level >= 0) {
    // this is a PC
    struct frame* fr = stack_block(&jq->stk, frame_get_level(jq, frame_level));
    target_pc = fr->bc->code + BT_TAG_GET_PC_OFFSET(bt_desc_tag(jq->bt.desc));
  }

  // search for a matching forkpoint
  while (jq->stk.limit < jq->restore_limit) {
    while (jq->stk.limit < jq->restore_limit && !stack_pop_will_free(&jq->stk, jq->fork_top)) {
      if (stack_pop_will_free(&jq->stk, jq->stk_top)) {
        jv_free(stack_pop(jq));
      } else if (stack_pop_will_free(&jq->stk, jq->curr_frame)) {
        frame_pop(jq);
      } else {
        assert(0);
      }
    }

    if (jq->fork_top == 0 || jq->stk.limit >= jq->restore_limit) {
      return 0;
    }

    struct forkpoint* fork = stack_block(&jq->stk, jq->fork_top);
    uint16_t* retaddr = fork->return_address;
    jq->stk_top = fork->saved_data_stack;
    jq->curr_frame = fork->saved_curr_frame;
    int path_len = fork->path_len;
    if (jv_get_kind(jq->path) == JV_KIND_ARRAY) {
      assert(path_len >= 0);
      jq->path = jv_array_slice(jq->path, 0, path_len);
    } else {
      fork->path_len = 0;
    }
    jv_free(jq->value_at_path);
    jq->value_at_path = fork->value_at_path;
    jq->subexp_nest = fork->subexp_nest;
    jq->fork_top = stack_pop_block(&jq->stk, jq->fork_top, sizeof(struct forkpoint));

    // now determine if this forkpoint is mathing the request

    if (bt_desc_prio(jq->bt.desc) == bt_desc_prio(fork->bt_desc)) {
      // this is the matching priority
      // check for a matching tag for SIGNAL priority

      // resolve the advertised forkpoint pc
      frame_level = BT_TAG_GET_PC_FRAME(bt_desc_tag(fork->bt_desc));
      uint16_t* match_pc = 0;
      if (frame_level >= 0) {
        struct frame* fr = stack_block(&jq->stk, frame_get_level(jq, frame_level));
        match_pc = fr->bc->code + BT_TAG_GET_PC_OFFSET(bt_desc_tag(fork->bt_desc));
      }

      int full_match = (jq->bt.desc == fork->bt_desc) 
                       || (bt_desc_prio(jq->bt.desc) > BT_PRIO_SIGNAL)
                       || ((frame_level >= 0) && (target_pc == match_pc));

      if (full_match) {
        // this is a matching forkpoint, respect the TTL
        if (jq->bt.ttl > 0) {
          jq->bt.ttl--;
        } else {
          return retaddr;
        }
      }
    }
    else if(bt_desc_prio(jq->bt.desc) < bt_desc_prio(fork->bt_desc)){
      // the forkpoint priority is higher than the backtracking request
      // stop here disregarding the TTL
      return retaddr;
    }
  }
  return 0;
}

static void jq_reset(jq_state *jq) {
  jq_reset_bt(jq, bt_desc_make(BT_PRIO_TERMINATE, 0));

  while (stack_restore(jq)) {}

  assert(jq->parent || jq->stk_top == 0);
  assert(jq->parent || jq->fork_top == 0);
  assert(jq->parent || jq->curr_frame == 0);
  if (!jq->parent)
    stack_reset(&jq->stk, 0);
  jq_reset_bt(jq, 0);
  jq->halted = 0;
  jq->finished = 0;
  jv_free(jq->exit_code);
  jv_free(jq->error_message);
  jq->exit_code = jv_invalid();
  jq->error_message = jv_invalid();

  if (jq->handles == &jq->handles_s) {
    if (jv_is_valid(jq->handles->jhandles)) {
      jv_array_foreach(jq->handles->jhandles, i, v) {
        if (jv_is_valid(v) && i > -1 && (size_t)i < jq->handles->nhandles)
          jv_free(jq_handle_close(jq, v));
      }
    }
    jv_free(jq->handles->jhandles);
    free(jq->handles->handles);
    jq->handles->handles = 0;
    jq->handles->nhandles = 0;
    jq->handles->jhandles = jv_array();
  }

  if (jv_get_kind(jq->path) != JV_KIND_INVALID)
    jv_free(jq->path);
  jq->path = jv_null();
  jv_free(jq->value_at_path);
  jq->value_at_path = jv_null();
  jq->subexp_nest = 0;
}

void jq_report_error(jq_state *jq, jv value) {
  assert(jq->err_cb);
  // callback must jv_free() its jv argument
  jq->err_cb(jq->err_cb_data, value);
}

static void set_error(jq_state *jq, jv value) {
  jq_reset_bt(jq, BT_DESC_ERROR);
  jq->bt.payload = value;
}

#define ON_BACKTRACK(op) ((op)+NUM_OPCODES)

static jq_state *coinit(jq_state *, uint16_t*);
static void co_err_cb(void *, jv);

jv jq_next(jq_state *jq) {
  jv cfunc_input[MAX_CFUNCTION_ARGS];

  jv_nomem_handler(jq->nomem_handler, jq->nomem_handler_data);

  if (jq->halted) {
    if (jq->debug_trace_enabled)
      printf("\t<halted>\n");
    return jv_invalid();
  }

  if (jq->finished) {
    if (jq->debug_trace_enabled)
      printf("\t<finished>\n");
    return jv_invalid();
  }

  jq_reset_bt(jq, BT_DESC_NEXT_VALUE);

  uint16_t* pc = stack_restore(jq);
  assert(pc);

  if (jq->initial_execution) {
    jq->initial_execution = 0;
    jq_reset_bt(jq, 0);
  } 

  assert(jq->parent == 0 || jq->restore_limit >= jq->stk.limit);
  while (1) {
    if (jq->halted) {
      if (jq->debug_trace_enabled)
        printf("\t<halted>\n");
      return jv_invalid();
    }
    uint16_t opcode = *pc;

    if (jq->debug_trace_enabled) {
      dump_operation(frame_current(jq)->bc, pc);
      const struct opcode_description* opdesc = opcode_describe(opcode);
      stack_ptr param = 0;
      if (!jq->bt.desc) {
        int stack_in = opdesc->stack_in;
        if (stack_in == -1) stack_in = pc[1];
        param = jq->stk_top;

        printf("\t(( ");
        for (int i=0; i<stack_in; i++) {
          if (i != 0) {
            printf(" | ");
            param = *stack_block_next(&jq->stk, param);
          }
          if (!param) break;
          jv_dump(jv_copy(*(jv*)stack_block(&jq->stk, param)), JV_PRINT_REFCOUNT);
          //printf("<%d>", jv_get_refcnt(param->val));
          //printf(" -- ");
          //jv_dump(jv_copy(jq->path), 0);
        }
        printf(" )) ");

        if (jq->debug_trace_enabled & JQ_DEBUG_TRACE_DETAIL && param < jq->restore_limit) {
          int first = 1;
          if(stack_in == 0) {
            // we haven't printed the top of the stack yet, print now
            jv_dump(jv_copy(*(jv*)stack_block(&jq->stk, param)), JV_PRINT_REFCOUNT);
          }
          while ((param = *stack_block_next(&jq->stk, param))) {
            if (!first || !stack_in) {
              printf(" || ");
            } else {
              first = 0;
            }
            jv_dump(jv_copy(*(jv*)stack_block(&jq->stk, param)), JV_PRINT_REFCOUNT);
          }
        }
      } else {
        const char* prio_name[] = {
          "BT_PRIO_NONE",
          "BT_PRIO_SIGNAL",
          "BT_PRIO_PROTECT",
          "BT_PRIO_TERMINATE"
        };

        assert(bt_desc_prio(jq->bt.desc) < (sizeof(prio_name) / sizeof(*prio_name)) && "Unknown backtrack priority");
        printf("\t[^] %s", prio_name[bt_desc_prio(jq->bt.desc)]);
        if (bt_desc_prio(jq->bt.desc) == BT_PRIO_SIGNAL) {
          uint32_t tag = bt_desc_tag(jq->bt.desc);
          if (BT_TAG_IS_OPCODE(tag)) {
            printf(", op: %s", opcode_describe(tag)->name);
          }
          else if (BT_TAG_IS_HARD(tag)) {
            printf(", tag: %d", tag);
          } 
          else if (BT_TAG_IS_SOFT(tag)) {
            printf(", soft tag: %d", BT_TAG_GET_SOFT(tag));
          } 
          else {
            // this is a frame offset
            printf(", frame: %d, pc: %04d", (int)((bt_desc_tag(jq->bt.desc) - BT_TAG_PC_START) >> 16), (int)(bt_desc_tag(jq->bt.desc)&0xffff));
          }
        }  
        if (jv_is_valid(jq->bt.payload)) {
          printf(", payload: ");
          jv_dump(jv_copy(jq->bt.payload), JV_PRINT_REFCOUNT);
        }      
      }

      if (jq->parent)
        printf("\t (CHILD %p of %p)", jq, jq->parent);

      printf("\n");
    }

    if (jq->bt.desc != 0) {
      opcode = ON_BACKTRACK(opcode);
    }
    pc++;

    switch (opcode) {
    default: assert(0 && "invalid instruction");
    case LOADK: {
      jv v = jv_array_get(jv_copy(frame_current(jq)->bc->constants), *pc++);
      assert(jv_is_valid(v));
      jv_free(stack_pop(jq));
      stack_push(jq, v);
      break;
    }

    case DUP: {
      jv v = stack_pop(jq);
      stack_push(jq, jv_copy(v));
      stack_push(jq, v);
      break;
    }

    case DUPN: {
      jv v = stack_popn(jq);
      stack_push(jq, jv_copy(v));
      stack_push(jq, v);
      break;
    }

    case DUP2: {
      jv keep = stack_pop(jq);
      jv v = stack_pop(jq);
      stack_push(jq, jv_copy(v));
      stack_push(jq, keep);
      stack_push(jq, v);
      break;
    }

    case SUBEXP_BEGIN: {
      jv v = stack_pop(jq);
      stack_push(jq, jv_copy(v));
      stack_push(jq, v);
      jq->subexp_nest++;
      break;
    }

    case SUBEXP_END: {
      assert(jq->subexp_nest > 0);
      jq->subexp_nest--;
      jv a = stack_pop(jq);
      jv b = stack_pop(jq);
      stack_push(jq, a);
      stack_push(jq, b);
      break;
    }

    case PUSHK_UNDER: {
      jv v = jv_array_get(jv_copy(frame_current(jq)->bc->constants), *pc++);
      assert(jv_is_valid(v));
      jv v2 = stack_pop(jq);
      stack_push(jq, v);
      stack_push(jq, v2);
      break;
    }

    case POP: {
      jv_free(stack_pop(jq));
      break;
    }

    case APPEND: {
      jv v = stack_pop(jq);
      uint16_t level = *pc++;
      uint16_t vidx = *pc++;
      jv* var = frame_local_var(jq, vidx, level);
      assert(jv_get_kind(*var) == JV_KIND_ARRAY);
      *var = jv_array_append(*var, v);
      break;
    }

    case INSERT: {
      jv stktop = stack_pop(jq);
      jv v = stack_pop(jq);
      jv k = stack_pop(jq);
      jv objv = stack_pop(jq);
      assert(jv_get_kind(objv) == JV_KIND_OBJECT);
      if (jv_get_kind(k) == JV_KIND_STRING) {
        stack_push(jq, jv_object_set(objv, k, v));
        stack_push(jq, stktop);
      } else {
        char errbuf[15];
        set_error(jq, jv_string_fmt("Cannot use %s (%s) as object key",
                                    jv_kind_name(jv_get_kind(k)),
                                    jv_dump_string_trunc(jv_copy(k), errbuf, sizeof(errbuf))));
        jv_free(stktop);
        jv_free(v);
        jv_free(k);
        jv_free(objv);
        goto do_backtrack;
      }
      break;
    }

    case ON_BACKTRACK(RANGE):
    case RANGE: {
      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      jv max = stack_pop(jq);

      if (jv_get_kind(*var) != JV_KIND_NUMBER ||
          jv_get_kind(max) != JV_KIND_NUMBER) {
        set_error(jq, jv_string_fmt("Range bounds must be numeric"));
        jv_free(max);
        goto do_backtrack;
      } else if (jv_number_value(*var) >= jv_number_value(max)) {
        /* finished iterating */
        jv_free(max);
        jq_reset_bt(jq, BT_DESC_NEXT_VALUE);
        goto do_backtrack;
      } else {
        jv curr = *var;
        *var = jv_number(jv_number_value(*var) + 1);

        struct stack_pos spos = stack_get_pos(jq);
        stack_push(jq, max);
        stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 3, spos);

        stack_push(jq, curr);
      }
      break;
    }

      // FIXME: loadv/storev may do too much copying/freeing
    case LOADV: {
      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      if (jq->debug_trace_enabled) {
        printf("V%d = ", v);
        jv_dump(jv_copy(*var), 0);
        printf(" <%d>\n", jv_get_refcnt(*var));
      }
      jv_free(stack_pop(jq));
      stack_push(jq, jv_copy(*var));
      break;
    }

      // Does a load but replaces the variable with null
    case LOADVN: {
      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      if (jq->debug_trace_enabled) {
        printf("V%d = ", v);
        jv_dump(jv_copy(*var), 0);
        printf(" (%d)\n", jv_get_refcnt(*var));
      }
      jv_free(stack_popn(jq));
      stack_push(jq, *var);
      *var = jv_null();
      break;
    }

    case STOREVN:
      stack_save(jq, BT_DESC_PROTECT,  pc - 1, stack_get_pos(jq));
      /* fallthru */
    case STOREV: {
      uint16_t level = *pc++;  
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      jv val = stack_pop(jq);
      if (jq->debug_trace_enabled) {
        printf("V%d = ", v);
        jv_dump(jv_copy(val), 0);
        printf(" <%d>\n", jv_get_refcnt(val));
      }
      jv_free(*var);
      *var = val;
      break;
    }

    case ON_BACKTRACK(STOREVN): {
      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      jv_free(*var);
      *var = jv_null();
      goto do_backtrack;
    }

    case STORE_PC: {
      uint16_t* my_pc = pc-1;
      uint16_t pc_offset = my_pc - frame_current(jq)->bc->code;

      jv val = stack_pop(jq);
      uint16_t level = *pc++;
      uint16_t v = *pc++;

      assert(level == 0 && "STORE_PC must define a fresh variable");

      stack_save(jq, BT_DESC_FRAME_PC(0, pc_offset),  my_pc, stack_get_pos(jq));
      stack_push(jq, val);

      jv* var = frame_local_var(jq, v, level);
      jv_free(*var);
      *var = jv_number(pc_offset);

      if (jq->debug_trace_enabled) {
        printf("V(pc)%d = ", v);
        jv_dump(jv_copy(*var), 0);
        printf("\n");
      }
      break;
    }

    case ON_BACKTRACK(STORE_PC): 
      // this is my signal
      // lower the priority of backtracking to data and continue backtracking
      jq->bt.desc = BT_DESC_NEXT_VALUE;
      goto do_backtrack;

    case BACKTRACK_PC: {
      jv_free(stack_pop(jq));

      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);

      int offset = (int)jv_number_value(jv_copy(*var));

      if (jq->debug_trace_enabled) {
        printf("V(pc)%d = ", v);
        jv_dump(jv_copy(*var), 0);
        printf("\n");
      }

      jq_reset_bt(jq, BT_DESC_FRAME_PC(level, offset));

      goto do_backtrack;
    }

    case STORE_GLOBAL: {
      // Get the constant
      jv val = jv_array_get(jv_copy(frame_current(jq)->bc->constants), *pc++);
      assert(jv_is_valid(val));

      // Store the var
      uint16_t level = *pc++;
      uint16_t v = *pc++;
      jv* var = frame_local_var(jq, v, level);
      if (jq->debug_trace_enabled) {
        printf("V%d = ", v);
        jv_dump(jv_copy(val), 0);
        printf(" (%d)\n", jv_get_refcnt(val));
      }
      jv_free(*var);
      *var = val;
      break;
    }

    case PATH_BEGIN: {
      jv v = stack_pop(jq);
      stack_push(jq, jq->path);

      stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, stack_get_pos(jq));

      stack_push(jq, jv_number(jq->subexp_nest));
      stack_push(jq, jq->value_at_path);
      stack_push(jq, jv_copy(v));

      jq->path = jv_array();
      jq->value_at_path = v; // next INDEX operation must index into v
      jq->subexp_nest = 0;
      break;
    }

    case PATH_END: {
      jv v = stack_pop(jq);
      // detect invalid path expression like path(.a | reverse)
      if (!path_intact(jq, jv_copy(v))) {
        char errbuf[30];
        jv msg = jv_string_fmt(
            "Invalid path expression with result %s",
            jv_dump_string_trunc(v, errbuf, sizeof(errbuf)));
        set_error(jq, msg);
        goto do_backtrack;
      }
      jv_free(v); // discard value, only keep path

      jv old_value_at_path = stack_pop(jq);
      int old_subexp_nest = (int)jv_number_value(stack_pop(jq));

      jv path = jq->path;
      jq->path = stack_pop(jq);

      struct stack_pos spos = stack_get_pos(jq);
      stack_push(jq, jv_copy(path));
      stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, spos);

      stack_push(jq, path);
      jq->subexp_nest = old_subexp_nest;
      jv_free(jq->value_at_path);
      jq->value_at_path = old_value_at_path;
      break;
    }

    case ON_BACKTRACK(PATH_BEGIN):
    case ON_BACKTRACK(PATH_END): {
      jv_free(jq->path);
      jq->path = stack_pop(jq);
      goto do_backtrack;
    }

    case INDEX:
    case INDEX_OPT: {
      jv t = stack_pop(jq);
      jv k = stack_pop(jq);
      // detect invalid path expression like path(reverse | .a)
      if (!path_intact(jq, jv_copy(t))) {
        char keybuf[15];
        char objbuf[30];
        jv msg = jv_string_fmt(
            "Invalid path expression near attempt to access element %s of %s",
            jv_dump_string_trunc(k, keybuf, sizeof(keybuf)),
            jv_dump_string_trunc(t, objbuf, sizeof(objbuf)));
        set_error(jq, msg);
        goto do_backtrack;
      }
      jv v = jv_get(t, jv_copy(k));
      if (jv_is_valid(v)) {
        path_append(jq, k, jv_copy(v));
        stack_push(jq, v);
      } else {
        jv_free(k);
        if (opcode == INDEX)
          set_error(jq, jv_invalid_get_msg(v));
        else {
          jv_free(v);
          jq_reset_bt(jq, BT_DESC_NEXT_VALUE);
        }
        goto do_backtrack;
      }
      break;
    }


    case JUMP: {
      uint16_t offset = *pc++;
      pc += offset;
      break;
    }

    case JUMP_F: {
      uint16_t offset = *pc++;
      jv t = stack_pop(jq);
      jv_kind kind = jv_get_kind(t);
      if (kind == JV_KIND_FALSE || kind == JV_KIND_NULL) {
        pc += offset;
      }
      stack_push(jq, t); // FIXME do this better
      break;
    }

    case EACH:
      /* fallthru */
    case EACH_OPT: {
      jv container = stack_pop(jq);
      // detect invalid path expression like path(reverse | .[])
      if (!path_intact(jq, jv_copy(container))) {
        char errbuf[30];
        jv msg = jv_string_fmt(
            "Invalid path expression near attempt to iterate through %s",
            jv_dump_string_trunc(container, errbuf, sizeof(errbuf)));
        set_error(jq, msg);
        goto do_backtrack;
      }
      stack_push(jq, container);
      stack_push(jq, jv_number(-1));
    }
    /* fallthru */
    case ON_BACKTRACK(EACH):
    case ON_BACKTRACK(EACH_OPT): {
      int idx = jv_number_value(stack_pop(jq));
      jv container = stack_pop(jq);

      int keep_going, is_last = 0;
      jv key, value;
      if (jv_get_kind(container) == JV_KIND_ARRAY) {
        if (opcode == EACH || opcode == EACH_OPT) idx = 0;
        else idx = idx + 1;
        int len = jv_array_length(jv_copy(container));
        keep_going = idx < len;
        is_last = idx == len - 1;
        if (keep_going) {
          key = jv_number(idx);
          value = jv_array_get(jv_copy(container), idx);
        }
      } else if (jv_get_kind(container) == JV_KIND_OBJECT) {
        if (opcode == EACH || opcode == EACH_OPT) idx = jv_object_iter(container);
        else idx = jv_object_iter_next(container, idx);
        keep_going = jv_object_iter_valid(container, idx);
        if (keep_going) {
          key = jv_object_iter_key(container, idx);
          value = jv_object_iter_value(container, idx);
        }
      } else {
        assert(opcode == EACH || opcode == EACH_OPT);
        if (opcode == EACH) {
          char errbuf[15];
          set_error(jq,
                    jv_invalid_with_msg(jv_string_fmt("Cannot iterate over %s (%s)",
                                                      jv_kind_name(jv_get_kind(container)),
                                                      jv_dump_string_trunc(jv_copy(container), errbuf, sizeof(errbuf)))));
        }
        keep_going = 0;
      }

      if (!keep_going) {
        if (keep_going)
          jv_free(value);
        jv_free(container);
        jq_reset_bt(jq, BT_DESC_NEXT_VALUE);
        goto do_backtrack;
      } else if (is_last) {
        // we don't need to make a backtrack point
        jv_free(container);
        path_append(jq, key, jv_copy(value));
        stack_push(jq, value);
      } else {
        struct stack_pos spos = stack_get_pos(jq);
        stack_push(jq, container);
        stack_push(jq, jv_number(idx));
        stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, spos);
        path_append(jq, key, jv_copy(value));
        stack_push(jq, value);
      }
      break;
    }
    case BACKTRACK_N: {

      jq_reset_bt(jq, BT_DESC_NEXT_VALUE);

      jv n = jv_array_get(jv_copy(frame_current(jq)->bc->constants), *pc++);

      if(jv_get_kind(n) == JV_KIND_NUMBER) {
        jq->bt.ttl = (uint32_t)jv_number_value(jv_copy(n));
      } 
      else if (jv_get_kind(n) != JV_KIND_NULL) { // null is equivalent to 0
        jq_reset_bt(jq, BT_DESC_ERROR);
        jq->bt.payload = jv_string("argument to break should be a number");
      }

      jv_free(n);

      goto do_backtrack;
    }

    case BACKTRACK_0:
      jq_reset_bt(jq, BT_DESC_NEXT_VALUE);

    do_backtrack:
      assert(jq->bt.desc > 0 && "Invalid backtracking descriptor");
      pc = stack_restore(jq);
      if (!pc) {
        // shouldn't be getting here
        // since START should intercept all backtracks 
        assert( 0 && "START should intercept all backtracks" );
      }
      // avoid reaching the bottom of the loop
      // because the backtracking is reset there;
      continue;

    case RAISE:
      set_error(jq, stack_pop(jq));
      goto do_backtrack;

    case TRY_BEGIN: {
      uint16_t* my_pc = pc-1;
      uint16_t offset = *pc++;
      uint16_t* handler_pc = pc+offset;
      uint16_t handler_offset = handler_pc - frame_current(jq)->bc->code;

      // save a forkpoint with a tag equal to the handler pc
      // this forkpoint will fire when TRY_END signals us
      stack_save(jq, BT_DESC_FRAME_PC(0, handler_offset),  my_pc, stack_get_pos(jq));

      // save another forkpoint with the ERROR tag
      // this will fire when the protected code raises an error
      stack_save(jq, BT_DESC_ERROR,  my_pc, stack_get_pos(jq));

      // neither of the forkpoints above will fire on data backtrack
      break;
    }

    case TRY_END:
      // install an error forkpoint so that we can signal about it 
      // to the TRY_BEGIN handler for a re-rise
      stack_save(jq, BT_DESC_ERROR,  pc - 1, stack_get_pos(jq));

      // jump past the handler to the rest of the code
      uint16_t offset = *pc++;
      pc += offset;
      break;

    case ON_BACKTRACK(TRY_BEGIN): {
      uint16_t offset = *pc++;
      uint16_t* handler_pc = pc+offset;
      uint16_t handler_offset = handler_pc - frame_current(jq)->bc->code;

      if (jq->bt.desc != BT_DESC_ERROR) {
        // we were signalled by TRY_END about and exception
        // which happend after the protected block
        // we should change the tag to ERROR and proceed
        assert((jq->bt.desc == BT_DESC_FRAME_PC(0, handler_offset)) && "Unexpected backtrack");
        jq->bt.desc = BT_DESC_ERROR;
        goto do_backtrack;
      }
      /*
       * Else we caught an error from the protected block, so we jump to
       * the handler.
       *
       * See commentary in gen_try().
       */

      jv_free(stack_pop(jq)); // free the input
      stack_push(jq, jv_copy(jq->bt.payload));  // push the error
      jq_reset_bt(jq, 0);
      pc += offset;
      break;
    }
    case ON_BACKTRACK(TRY_END): {
      assert(jq->bt.desc == BT_DESC_ERROR && "Unexpected backtrack");
      pc++;  // unused offset

      // Signal the error to the matching TRY_BEGIN
      // TRY_BEGIN will be awaiting a tag equal to the PC of the first
      // instruction of the handler. This is the next instruction
      jq->bt.desc = BT_DESC_FRAME_PC(0, pc - frame_current(jq)->bc->code);

      goto do_backtrack;
    }

    case DESTRUCTURE_ALT:
      stack_save(jq, BT_DESC_ERROR,  pc - 1, stack_get_pos(jq));
      pc++; // skip offset this time
      break;

    case FORK: {
      stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, stack_get_pos(jq));
      pc++; // skip offset this time
      break;
    }

    case ON_BACKTRACK(DESTRUCTURE_ALT): {
      assert(jq->bt.desc == BT_DESC_ERROR && "Unexpected backtrack (DESTRUCTURE_ALT)");
      // DESTRUCTURE_ALT doesn't want the error message on the stack,
      // as we would just want to throw it away anyway.
      jq_reset_bt(jq, 0);
      uint16_t offset = *pc++;
      pc += offset;
      break;
    }
    case ON_BACKTRACK(FORK): {
      uint16_t offset = *pc++;
      pc += offset;
      break;
    }

    case CALL_BUILTIN: {
      int nargs = *pc++;
      jv top = stack_pop(jq);
      jv* in = cfunc_input;
      in[0] = top;
      for (int i = 1; i < nargs; i++) {
        in[i] = stack_pop(jq);
      }
      struct cfunction* function = &frame_current(jq)->bc->globals->cfunctions[*pc++];
      typedef jv (*func_1)(jq_state*,jv);
      typedef jv (*func_2)(jq_state*,jv,jv);
      typedef jv (*func_3)(jq_state*,jv,jv,jv);
      typedef jv (*func_4)(jq_state*,jv,jv,jv,jv);
      typedef jv (*func_5)(jq_state*,jv,jv,jv,jv,jv);
      switch (function->nargs) {
      case 1: top = ((func_1)function->fptr)(jq, in[0]); break;
      case 2: top = ((func_2)function->fptr)(jq, in[0], in[1]); break;
      case 3: top = ((func_3)function->fptr)(jq, in[0], in[1], in[2]); break;
      case 4: top = ((func_4)function->fptr)(jq, in[0], in[1], in[2], in[3]); break;
      case 5: top = ((func_5)function->fptr)(jq, in[0], in[1], in[2], in[3], in[4]); break;
      // FIXME: a) up to 7 arguments (input + 6), b) should assert
      // because the compiler should not generate this error.
      default: return jv_invalid_with_msg(jv_string("Function takes too many arguments"));
      }

      if (jv_is_valid(top)) {
        stack_push(jq, top);
      } else if (jv_invalid_has_msg(jv_copy(top))) {
        set_error(jq, jv_invalid_get_msg(top));
        goto do_backtrack;
      } else {
        // C-coded function returns invalid w/o msg? -> backtrack, as if
        // it had returned `empty`
        jq_reset_bt(jq, BT_DESC_NEXT_VALUE);
        goto do_backtrack;
      }
      break;
    }

    case TAIL_CALL_JQ:
    case CALL_JQ: {
      /*
       * Bytecode layout here:
       *
       *  CALL_JQ
       *  <nclosures>                       (i.e., number of call arguments)
       *  <callee closure>                  (what we're calling)
       *  <nclosures' worth of closures>    (frame reference + code pointer)
       *
       *  <next instruction (to return to)>
       *
       * Each closure consists of two uint16_t values: a "level"
       * identifying the frame to be closed over, and an index.
       *
       * The level is a relative number of call frames reachable from
       * the currently one; 0 -> current frame, 1 -> previous frame, and
       * so on.
       *
       * The index is either an index of the closed frame's subfunctions
       * or of the closed frame's parameter closures.  If the latter,
       * that closure will be passed, else the closed frame's pointer
       * and the subfunction's code will form the closure to be passed.
       *
       * See make_closure() for more information.
       */
      jv input = stack_pop(jq);
      uint16_t nclosures = *pc++;
      uint16_t* retaddr = pc + 2 + nclosures*2;
      stack_ptr retdata = jq->stk_top;
      struct frame* new_frame;
      struct closure cl = make_closure(jq, pc);
      if (opcode == TAIL_CALL_JQ) {
        retaddr = frame_current(jq)->retaddr;
        retdata = frame_current(jq)->retdata;
        frame_pop(jq);
      }
      new_frame = frame_push(jq, cl, pc + 2, nclosures);
      new_frame->retdata = retdata;
      new_frame->retaddr = retaddr;
      pc = new_frame->bc->code;
      stack_push(jq, input);
      break;
    }

    case PROTECT: {
      stack_push(jq, jv_null());
      stack_save(jq, BT_DESC_PROTECT,  pc - 1, stack_get_pos(jq));
      uint16_t offset = *pc++;
      // jump over the handler
      pc += offset;
      break;
    }
    case ON_BACKTRACK(PROTECT): {
      // something happened down the stream
      // we need to save this state and run the handler
      jv desc = stack_pop(jq);

      if(jv_get_kind(desc) == JV_KIND_NULL) {
        // this is the first backtrack;
        // we should save the backtrack state and run the handler

        jv raising = jq->bt.desc == BT_DESC_ERROR ? jv_true() : jv_false();
        jv error = jv_copy(jq->bt.payload);

        if(jv_is_valid(jq->bt.payload)) {
          stack_push(jq, jv_copy(jq->bt.payload));
          stack_push(jq, jv_true()); // has payload
        } else {
          stack_push(jq, jv_false()); // no payload
        }
        stack_push(jq, jv_number(jq->bt.ttl));
        stack_push(jq, jv_number(jq->bt.desc));

        jq_reset_bt(jq, 0);
        stack_save(jq, BT_DESC_PROTECT,  pc - 1, stack_get_pos(jq));

        // prepare the input for the protect handler

        jv input = jv_object();
        if(jv_get_kind(raising) == JV_KIND_TRUE) {
          jv_object_set(input, jv_string("error"), error);
        } else {
          jv_free(error);
        }
        jv_object_set(input, jv_string("raising"), raising);

        stack_push(jq, input);

        // proceed to the handler now
        break;
      }
      else {
        // this is the second backtrack
        // restore the original bt signal and move on
        assert(jv_get_kind(desc) == JV_KIND_NUMBER);

        jq_reset_bt(jq, (uint32_t)jv_number_value(desc));
        jq->bt.ttl = (uint32_t)jv_number_value(stack_pop(jq));
        jv has_payload = stack_pop(jq); 
        if (jv_get_kind(has_payload) == JV_KIND_TRUE) {
          jq->bt.payload = stack_pop(jq);
        }
        jv_free(has_payload);
        goto do_backtrack;
      }

      
      break;
    }

    case COCREATE: {
      /* Create a child co-routine with the following code
       * The child side will start happily at the next instruction
       *
       *
       * coinit() makes a copy of the parent jq_state, but arranges for the
       * child never to stack_restore() past this point.  See jq_reset().
       *
       * This all works because we never store pointers into the stack on the
       * stack without first encoding them to be relative to the stack, and
       * because the child cannot backtrack past this point, the parent will
       * teardown the child if the parent backtracks past this point, and there
       * is no concurrent access to the stack.
       */
      jv input = stack_pop(jq);

      uint16_t body_len = *pc++;

      assert(*pc == START && "COCREATE must be followed by START");
      jq_state *child = coinit(jq, pc);
      jq_start(child, input, jq->debug_trace_enabled ? JQ_DEBUG_TRACE_ALL : 0);

      // jump over the cobody
      pc += body_len;

      // and return its handle
      extern struct jq_io_table jq__covt;
      jv handle = jq_handle_new(jq, "coroutine", &jq__covt, child);
      stack_push(jq, handle);

      break;
    }
    case START:
    case ON_BACKTRACK(START): {

      if(jq->bt.desc == BT_DESC_ERROR) {
        // we are raising and backtracked up to here
        // this is an unhandled user error, halt and return it
        jv error = jv_invalid_with_msg(jv_copy(jq->bt.payload));
        jq_reset_bt(jq, 0);
        jq->finished = 1;
        return error;
      } else if (bt_desc_prio(jq->bt.desc) > BT_DESC_ERROR) {
        return jv_invalid_with_msg(jv_string_fmt("jq VM unwinded with priority %d", bt_desc_prio(jq->bt.desc)));
      }

      jv start_input = jq->start_input;
      jq->start_input = jv_invalid();

      if (!jv_is_valid(start_input)) {
        if (jv_invalid_has_msg(jv_copy(start_input))) {
          // in case start input is an invalid with message
          // then return this invalid and halt
          jq->finished = 1;
          return start_input;
        }
        switch (jq->input_mode) {
          case JQ_INPUT_RETURN_EMPTY: {
            // the usual mode:
            // save the stack and request more input by returning empty
            stack_save(jq, BT_DESC_ALWAYS,  pc - 1, stack_get_pos(jq));
            return start_input /*jv_invalid()*/;
          }
          case JQ_INPUT_CALLBACK: {
            while(!jv_is_valid(start_input) && !jv_invalid_has_msg(jv_copy(start_input))) {
              if (jq->input_cb == NULL) {
                return jv_invalid_with_msg(jv_string("Input callback mode requested but input callback hasn't been set"));
              }

              start_input = jq->input_cb(jq, jq->input_cb_data);
            }

            if(!jv_is_valid(start_input) && jv_invalid_has_msg(jv_copy(start_input))) {
              jq->finished = 1;
              return start_input;
            }
          }
        }
      }
      
      // geting here means that start_input is a valid value
      stack_save(jq, BT_DESC_ALWAYS,  pc - 1, stack_get_pos(jq));
      stack_push(jq, start_input);

      break;
    }

    case COEVAL: {
      /*
       * Eval.  Setup a new jq_state to run the given program.  Treat it like a co-routine.
       */
      jv program = stack_pop(jq);
      jv options;
      jv input;
      jv args;
      if (jv_get_kind(program) == JV_KIND_ARRAY) {
        input = jv_array_get(jv_copy(program), 1);
        args = jv_array_get(jv_copy(program), 2);
        options = jv_array_get(jv_copy(program), 3);
        program = jv_array_get(program, 0);
      } else {
        options = jv_invalid();
        args = JV_OBJECT(jv_string("positional"), jv_array());
      }
      if (jv_get_kind(args) != JV_KIND_OBJECT && jv_get_kind(args) != JV_KIND_NULL) {
        set_error(jq, jv_string("Eval program arguments must be an object or null"));
        jv_free(program);
        jv_free(options);
        jv_free(input);
        jv_free(args);
        goto do_backtrack;
      }
      if (jv_get_kind(args) == JV_KIND_NULL) {
        jv_free(args);
        args = JV_OBJECT(jv_string("positional"), jv_array());
      }
      if (jv_get_kind(options) != JV_KIND_OBJECT && jv_get_kind(options) != JV_KIND_NULL) {
        set_error(jq, jv_string("Eval program options must be an object"));
        jv_free(program);
        jv_free(options);
        jv_free(input);
        jv_free(args);
        goto do_backtrack;
      }
      if (jv_get_kind(options) == JV_KIND_OBJECT &&
          jv_object_has(jv_copy(options), jv_string("ALLOW_IO"))) {
        jv allow_io = jv_object_get(jv_copy(jq->attrs), jv_string("ALLOW_IO"));
        if (jv_get_kind(allow_io) == JV_KIND_TRUE) {
          /* Parent allows I/O; child may allow I/O */
          jv_free(allow_io);
          options = jv_object_set(jv_copy(jq->attrs), jv_string("ALLOW_IO"),
                                  jv_object_get(options, jv_string("ALLOW_IO")));
        } else {
          /* No I/O for child because no I/O for parent */
          jv_free(allow_io);
          jv_free(options);
          options = jv_copy(jq->attrs);
        }
      } else {
        /* Child will not allow I/O regardless of whether the parent does */
        jv_free(options);
        options = jv_object_set(jv_copy(jq->attrs), jv_string("ALLOW_IO"),
                                jv_false());
      }
      if (jv_get_kind(program) != JV_KIND_STRING) {
        set_error(jq, jv_string("Eval program must be a string"));
        jv_free(program);
        jv_free(options);
        jv_free(input);
        jv_free(args);
        goto do_backtrack;
      }

      jq_state *child = jq_init();
      if (jv_is_valid(options))
        jq_set_attrs(jq, jv_copy(options));

      child->input_mode = JQ_INPUT_RETURN_EMPTY;

      /* Give the child a way to report errors */
      jv *p = jv_mem_calloc(1, sizeof(jv));
      *p = jv_null();
      jq_set_error_cb(child, co_err_cb, p);
      if (!jq_compile_args(child, jv_string_value(program), jv_copy(args))) {
        set_error(jq, jv_string_fmt("Eval program failed to compile: %s", jv_string_value(*p)));
        jq_teardown(&child);
        jv_free(program);
        jv_free(options);
        jv_free(input);
        jv_free(args);
        jv_free(*p);
        free(p);
        goto do_backtrack;
      }
      /* Now let the child send errors to stderr again? */
      jq_set_error_cb(child, NULL, NULL);
      jq_start(child, jv_copy(input), jq->debug_trace_enabled ? JQ_DEBUG_TRACE_ALL : 0);
      copy_callbacks(jq, child);
      jv_free(program);
      jv_free(options);
      jv_free(input);
      jv_free(args);
      jv_free(*p);
      free(p);

      extern struct jq_io_table jq__covt;
      stack_push(jq, jq_handle_new(jq, "coroutine", &jq__covt, child));
      stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, stack_get_pos(jq));
      break;
    }
    case ON_BACKTRACK(COEVAL): {
      jv cohandle = stack_pop(jq);
      jq_handle_close(jq, cohandle);
      goto do_backtrack;
    }

    case TAIL_OUT: {
      // optimized version of OUT;BACKTRACK
      jv value = stack_pop(jq);
      return value;
    }
    case OUT: {
      jv value = stack_pop(jq);
      stack_save(jq, BT_DESC_NEXT_VALUE,  pc - 1, stack_get_pos(jq));
      return value;
    }
    case ON_BACKTRACK(OUT): {
      // stop backtracking, 
      // move on
      break;
    }

    case RET_JQ: {
      jv value = stack_pop(jq);
      assert(jq->stk_top == frame_current(jq)->retdata);
      uint16_t* retaddr = frame_current(jq)->retaddr;

      assert(retaddr);
      
      pc = retaddr;
      frame_pop(jq);

      stack_push(jq, value);
      break;
    }
    }
  
    jq_reset_bt(jq, 0);
  }
}

jv jq_format_error(jv msg) {
  if (jv_get_kind(msg) == JV_KIND_NULL ||
      (jv_get_kind(msg) == JV_KIND_INVALID && !jv_invalid_has_msg(jv_copy(msg)))) {
    jv_free(msg);
    fprintf(stderr, "jq: error: out of memory\n");
    return jv_null();
  }

  if (jv_get_kind(msg) == JV_KIND_STRING)
    return msg;                         // expected to already be formatted

  if (jv_get_kind(msg) == JV_KIND_INVALID)
    msg = jv_invalid_get_msg(msg);

  if (jv_get_kind(msg) == JV_KIND_NULL)
    return jq_format_error(msg);        // ENOMEM

  // Invalid with msg; prefix with "jq: error: "

  if (jv_get_kind(msg) != JV_KIND_INVALID) {
    if (jv_get_kind(msg) == JV_KIND_STRING)
      return jv_string_fmt("jq: error: %s", jv_string_value(msg));

    msg = jv_dump_string(msg, JV_PRINT_INVALID);
    if (jv_get_kind(msg) == JV_KIND_STRING)
      return jv_string_fmt("jq: error: %s", jv_string_value(msg));
    return jq_format_error(jv_null());  // ENOMEM
  }

  // An invalid inside an invalid!
  return jq_format_error(jv_invalid_get_msg(msg));
}

static void init_vtable(struct jq_plugin_vtable *);

// XXX Refactor into a utility function that returns a jv and one that
// uses it and then prints that jv's string as the complete error
// message.
static void default_err_cb(void *data, jv msg) {
  msg = jq_format_error(msg);
  fprintf((FILE *)data, "%s\n", jv_string_value(msg));
  jv_free(msg);
}

jq_state *jq_init(void) {
  jq_state *jq;
  jq = jv_mem_alloc_unguarded(sizeof(*jq));
  if (jq == NULL)
    return NULL;

  init_vtable(&vtable);
  jq->vtable = &vtable;
  jq->bc = 0;
  jq->libs = 0;
  jq->next_label = 0;

  stack_init(&jq->stk);
  jq->parent = 0;
  jq->start_pc = 0;
  jq->restore_limit = 0;
  jq->stk_top = 0;
  jq->fork_top = 0;
  jq->curr_frame = 0;
  jq->error = jv_null();

  jq->halted = 0;
  jq->finished = 0;
  jq->exit_code = jv_invalid();
  jq->error_message = jv_invalid();
  jq_reset_bt(jq, 0);

  jq->err_cb = default_err_cb;
  jq->err_cb_data = stderr;

  jq->input_cb = 0;
  jq->input_cb_data = 0;

  jq->start_input = jv_invalid();
  jq->input_mode = JQ_INPUT_DEFAULT;

  jq->vmid = jv_number_random_int();
  jq->rnd = jv_number_random_int();
  jq->attrs = jv_object();
  jq->path = jv_null();
  jq->value_at_path = jv_null();

  jq->nomem_handler = NULL;
  jq->nomem_handler_data = NULL;
  jq->handles = &jq->handles_s;
  jq->handles->jhandles = jv_array();
  jq->handles->nhandles = 0;
  jq->handles->handles = 0;

  jq->io_policy = NULL;
  jq->io_policy_data = jv_null();

  return jq;
}

static void co_err_cb(void *d, jv msg) {
  jv prev = *(jv *)d;

  if (jv_get_kind(prev) == JV_KIND_NULL) {
    *(jv *)d = msg;
  } else if (jv_get_kind(prev) == JV_KIND_STRING) {
    *(jv *)d = jv_string_fmt("%s; %s", jv_string_value(prev), jv_string_value(msg));
    jv_free(prev);
    jv_free(msg);
  } else {
    jv_free(prev);
    *(jv *)d = msg;
  }
}

static jv coinput_cb(jq_state *child, void *vjv) {
  jv *jvp = vjv;
  jv ret;

  if (jv_is_valid(*jvp) || jv_invalid_has_msg(jv_copy(*jvp))) {
    ret = *jvp;
    *jvp = jv_invalid();
    return ret;
  }

  return jv_invalid();
}

/* Not intended to be called by apps */
static jq_state *coinit(jq_state *parent, uint16_t* start_pc) {
  jq_state *child;
  child = jv_mem_calloc(1, sizeof(*child));

  /* First, copy all fields */
  *child = *parent;

  /* Copy the stack (see notes in exec_stack.h) */
  stack_copy(&child->stk, &parent->stk);

  /* Linkt the parent */
  child->parent = parent;

  /* Stop the child freeing things that belong to the parent */
  child->restore_limit = child->stk.limit;
  child->start_pc = start_pc;

  child->halted = 0;
  child->libs = 0;
  child->initial_execution = 1;

  /* Give the client a VM ID */
  child->rnd = jv_number_random_int();
  child->vmid = jv_invalid();
  do {
    jv_free(child->vmid);
    child->vmid = jv_number_random_int();
  } while (jv_is_valid(child->vmid) && jv_is_valid(parent->vmid) &&
           jv_equal(jv_copy(parent->vmid), jv_copy(child->vmid)));  /* paranoia */

  /* Copy or null out various jv values not kept on the stack */
  child->value_at_path = jv_copy(parent->value_at_path);
  child->error_message = jv_invalid();
  child->exit_code = jv_invalid();
  child->error = jv_null();
  child->attrs = jv_copy(parent->attrs);
  child->path = jv_copy(parent->path);
  child->io_policy_data = jv_copy(parent->io_policy_data);
  child->start_input = jv_invalid();

  /* Share I/O handles with the parent */
  child->handles = parent->handles;

  /*
   * Give the child a way to get inputs from writes to the I/O handle for the
   * client.
   */
  jq_set_input_cb(child, coinput_cb, &child->start_input);
  child->input_mode = JQ_INPUT_RETURN_EMPTY;

  return child;
}

jv jq_get_vmid(jq_state *jq) { return jv_copy(jq->vmid); }

void jq_set_error_cb(jq_state *jq, jq_msg_cb cb, void *data) {
  if (cb == NULL) {
    jq->err_cb = default_err_cb;
    jq->err_cb_data = stderr;
  } else {
    jq->err_cb = cb;
    jq->err_cb_data = data;
  }
}

void jq_get_error_cb(jq_state *jq, jq_msg_cb *cb, void **data) {
  *cb = jq->err_cb;
  *data = jq->err_cb_data;
}

void jq_set_nomem_handler(jq_state *jq, void (*nomem_handler)(void *), void *data) {
  jv_nomem_handler(nomem_handler, data);
  jq->nomem_handler = nomem_handler;
  jq->nomem_handler_data = data;
}


void jq_start(jq_state *jq, jv input, int flags) {
  jv_nomem_handler(jq->nomem_handler, jq->nomem_handler_data);

  if (!jq->parent) {
    jq_reset(jq);
    struct closure top = {jq->bc, -1};
    struct frame* top_frame = frame_push(jq, top, 0, 0);
    top_frame->retdata = 0;
    top_frame->retaddr = 0;
  }

  jq->start_input = input;
  stack_save(jq, BT_DESC_NEXT_VALUE,  jq->parent ? jq->start_pc : jq->bc->code, stack_get_pos(jq));

  jq->debug_trace_enabled = flags & JQ_DEBUG_TRACE_ALL;
  jq->initial_execution = 1;
  jq->halted = 0;
  jq->finished = 0;
}

void jq_teardown(jq_state **jqp) {
  jq_state *jq = *jqp;
  if (jq == NULL)
    return;
  *jqp = NULL;

  jq_reset(jq);
  if (jq->parent)
    stack_reset(&jq->stk, jq->restore_limit);

  if (jq->handles == &jq->handles_s)
    jv_free(jq->handles->jhandles);

  if (!jq->parent) {
    bytecode_free(jq->bc);
    libraries_free(jq->libs);
    if (jq->io_policy)
      jq_teardown(&jq->io_policy);
  }
  jv_free(jq->rnd);
  jv_free(jq->vmid);
  jv_free(jq->attrs);
  jv_free(jq->io_policy_data);

  jv_mem_free(jq);
}

static int ret_follows(uint16_t *pc) {
  if (*pc == RET_JQ)
    return 1;
  if (*pc++ != JUMP)
    return 0;
  return ret_follows(pc + *pc + 1); // FIXME, might be ironic
}

/*
 * Look for tail calls that can be optimized: tail calls with no
 * references left to the current frame.
 *
 * We're staring at this bytecode layout:
 *
 *   CALL_JQ
 *   <nclosures>
 *   <callee closure>       (2 units)
 *   <nclosures closures>   (2 units each)
 *   <next instruction>
 *
 * A closure is:
 *
 *   <level>    (a relative frame count chased via the current frame's env)
 *   <index>    (an index of a subfunction or closure in that frame)
 *
 * We're looking for:
 *
 * a) the next instruction is a RET_JQ or a chain of unconditional JUMPs
 * that ends in a RET_JQ, and
 *
 * b) none of the closures -callee included- have level == 0.
 */
static uint16_t tail_call_analyze(uint16_t *pc) {
  assert(*pc == CALL_JQ);
  pc++;
  // + 1 for the callee closure
  for (uint16_t nclosures = *pc++ + 1; nclosures > 0; pc++, nclosures--) {
    if (*pc++ == 0)
      return CALL_JQ;
  }
  if (ret_follows(pc))
    return TAIL_CALL_JQ;
  return CALL_JQ;
}

static struct bytecode *optimize_code(struct bytecode *bc) {
  uint16_t *pc = bc->code;
  // FIXME: Don't mutate bc->code...
  while (pc < bc->code + bc->codelen) {
    switch (*pc) {
    case CALL_JQ:
      *pc = tail_call_analyze(pc);
      break;

    // Other bytecode optimizations here.  A peephole optimizer would
    // fit right in.
    default: break;
    }
    pc += bytecode_operation_length(pc);
  }
  return bc;
}

static struct bytecode *optimize(struct bytecode *bc) {
  for (int i=0; i<bc->nsubfunctions; i++) {
    bc->subfunctions[i] = optimize(bc->subfunctions[i]);
  }
  return optimize_code(bc);
}

static jv
args2obj(jv args)
{
  if (jv_get_kind(args) == JV_KIND_OBJECT)
    return args;
  assert(jv_get_kind(args) == JV_KIND_ARRAY);
  jv r = jv_object();
  jv kk = jv_string("name");
  jv vk = jv_string("value");
  jv_array_foreach(args, i, v)
    r = jv_object_set(r, jv_object_get(jv_copy(v), kk), jv_object_get(v, vk));
  jv_free(args);
  jv_free(kk);
  jv_free(vk);
  return r;
}

int jq_compile_args(jq_state *jq, const char* str, jv args) {
  jv_nomem_handler(jq->nomem_handler, jq->nomem_handler_data);
  assert(jv_get_kind(args) == JV_KIND_ARRAY || jv_get_kind(args) == JV_KIND_OBJECT);
  struct locfile* locations;
  locations = locfile_init(jq, "<top-level>", str, strlen(str));
  block program;
  jq_reset(jq);
  if (jq->bc) {
    bytecode_free(jq->bc);
    jq->bc = 0;
    jq->start_pc = 0;
  }
  if (jq->libs) {
    libraries_free(jq->libs);
    jq->libs = 0;
  }
  int nerrors = load_program(jq, locations, &program, &jq->libs);
  if (nerrors == 0) {
    nerrors = builtins_bind(jq, &program);
    if (nerrors == 0) {
      nerrors = block_compile(program, &jq->bc, locations, args = args2obj(args));
    }
  } else
    jv_free(args);
  if (nerrors)
    jq_report_error(jq, jv_string_fmt("jq: %d compile %s", nerrors, nerrors > 1 ? "errors" : "error"));
  if (jq->bc) {
    jq->bc = optimize(jq->bc);
    jq->start_pc = jq->bc->code;
  }
  locfile_free(locations);
  return jq->bc != NULL;
}

int jq_compile(jq_state *jq, const char* str) {
  return jq_compile_args(jq, str, jv_object());
}

static int get_handle_index(jq_state *jq, jv handle) {
  int i = -1;
  jv n = jv_invalid();

  if (jq->handles->nhandles > 0 &&
      jv_get_kind(handle) == JV_KIND_OBJECT &&
      jv_get_kind((n = jv_object_get(jv_copy(handle), jv_string("n")))) == JV_KIND_NUMBER &&
      jv_number_value(n) < jq->handles->nhandles &&
      jv_number_value(n) < INT_MAX &&
      jv_equal(jv_copy(handle),
               jv_array_get(jv_copy(jq->handles->jhandles), jv_number_value(n))))
    i = jv_number_value(n);
  jv_free(handle);
  jv_free(n);
  return i;
}

void *jq_handle_get(jq_state *jq, jv handle) {
  int i = get_handle_index(jq, handle);

  if (i == -1 || !jq->handles->handles[i])
    return 0;
  return jq->handles->handles[i]->handle;
}

jv jq_handle_get_kind(jq_state *jq, jv handle) {
  void *h = jq_handle_get(jq, handle);

  if (h && jv_get_kind(handle) == JV_KIND_OBJECT)
    return jv_getpath(handle, JV_ARRAY(jv_string("v"), jv_string("kind")));

  jv_free(handle);
  return jv_invalid();
}

static jv make_verifier(const char *kind, void *handle, void *junk) {
  jv n = jv_number_random_int();

  if (jv_is_valid(n))
    return JV_OBJECT(jv_string("kind"), jv_string(kind),
                     jv_string("r"), n);

  /* No system RNG?  Use handle and junk ptr values as verifier "randoms" */
  return JV_OBJECT(jv_string("kind"), jv_string(kind),
                   jv_string("r"), JV_ARRAY(jv_number((uintptr_t)handle),
                                            jv_number((uintptr_t)junk)));
}

static int new_handle_slot(jq_state *jq) {
  size_t additions = jq->handles->nhandles == 0 ? 16 : (jq->handles->nhandles>>1) + 4;
  size_t k;

  /* Find a slot */
  for (k = 0; k < jq->handles->nhandles; k++) {
    if (jq->handles->handles[k] == 0 || jq->handles->handles[k]->handle == 0) {
      assert(k < INT_MAX);
      return k;
    }
  }

  /* Allocate additional slots */
  jq->handles->handles = jv_mem_realloc(jq->handles->handles,
                                        sizeof(jq->handles->handles[0]) * (jq->handles->nhandles + additions));
  memset(&jq->handles->handles[jq->handles->nhandles], 0, sizeof(jq->handles->handles[0]) * additions);
  jq->handles->nhandles += additions;
  assert(k < INT_MAX);
  return k;
}

jv jq_handle_new(jq_state *jq, const char *kind, struct jq_io_table *vt, void *hdl) {
  int i = new_handle_slot(jq);

  if (i < 0) {
    if (vt->fhclose)
      jv_free(vt->fhclose(jq, jv_invalid(), hdl));
    return jv_invalid();
  }

  jv jhandle = JV_OBJECT(jv_string("n"), jv_number(i),
                         jv_string("v"), make_verifier(kind, hdl, jq->handles->handles));
  jq->handles->jhandles = jv_array_set(jq->handles->jhandles, i, jv_copy(jhandle));
  jq->handles->handles[i] = jv_mem_alloc(sizeof(jq->handles->handles[i][0]));
  jq->handles->handles[i]->handle = hdl;
  jq->handles->handles[i]->refcnt = 1;
  jq->handles->handles[i]->vt = vt;
  return jhandle;
}

jv jq_handle_close(jq_state *jq, jv handle) {
  struct raw_handle *rhp;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0) {
    jv_free(handle);
    return jv_invalid_with_msg(jv_string("No such open file handle"));
  }

  rhp = jq->handles->handles[i];
  jq->handles->handles[i] = 0;
  if (--(rhp->refcnt)) {
    jv_free(handle);
    return jv_true();
  }

  /* We cleanup first, then call the close function to avoid reentry via coclose() */
  struct raw_handle rh = *rhp;
  jq->handles->jhandles = jv_array_set(jq->handles->jhandles, i, jv_invalid());
  rhp->handle = 0;
  rhp->vt = 0;
  free(rhp);
  jv ret = rh.vt->fhclose ? rh.vt->fhclose(jq, jv_copy(handle), rh.handle) : jv_true();
  jv_free(handle);
  return ret;
}

jv jq_handle_reset(jq_state *jq, jv handle) {
  struct raw_handle *rh;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0)
    return jv_invalid_with_msg(jv_string("No such open file handle"));

  rh = jq->handles->handles[i];
  if (rh->vt->fhreset)
    return rh->vt->fhreset(jq, handle, rh->handle);
  jv_free(handle);
  return jv_false();
}

jv jq_handle_write(jq_state *jq, jv handle, jv v) {
  struct raw_handle *rh;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0)
    return jv_invalid_with_msg(jv_string("No such open file handle"));

  rh = jq->handles->handles[i];
  if (rh->vt->fhwrite)
    return rh->vt->fhwrite(jq, handle, rh->handle, v);
  else
    jv_free(handle);
  return jv_invalid_with_msg(jv_string("Failed to write to file")); /* XXX filename would be nice */
}

jv jq_handle_read(jq_state *jq, jv handle) {
  struct raw_handle *rh;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0)
    return jv_invalid_with_msg(jv_string("No such open file handle"));

  rh = jq->handles->handles[i];
  if (rh->vt->fhread)
    return rh->vt->fhread(jq, handle, rh->handle);
  jv_free(handle);
  return jv_invalid_with_msg(jv_string("Failed to read from file")); /* XXX filename would be nice */
}

jv jq_handle_stat(jq_state *jq, jv handle) {
  struct raw_handle *rh;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0)
    return jv_invalid_with_msg(jv_string("No such open file handle"));

  rh = jq->handles->handles[i];
  if (rh->vt->fhstat)
    return rh->vt->fhstat(jq, handle, rh->handle);
  jv_free(handle);
  return jv_invalid_with_msg(jv_string("Failed to stat file")); /* XXX filename would be nice */
}

jv jq_handle_eof(jq_state *jq, jv handle) {
  struct raw_handle *rh;
  int i = get_handle_index(jq, jv_copy(handle));

  if (i < 0)
    return jv_invalid_with_msg(jv_string("No such open file handle"));

  rh = jq->handles->handles[i];
  if (rh->vt->fheof)
    return rh->vt->fheof(jq, handle, rh->handle);
  jv_free(handle);
  return jv_invalid_with_msg(jv_string("Failed to determine if file handle at EOF"));
}

jv jq_get_jq_origin(jq_state *jq) {
  return jq_get_attr(jq, jv_string("JQ_ORIGIN"));
}

jv jq_get_prog_origin(jq_state *jq) {
  return jq_get_attr(jq, jv_string("PROGRAM_ORIGIN"));
}

jv jq_get_lib_dirs(jq_state *jq) {
  return jq_get_attr(jq, jv_string("JQ_LIBRARY_PATH"));
}

void jq_set_attrs(jq_state *jq, jv attrs) {
  assert(jv_get_kind(attrs) == JV_KIND_OBJECT);
  jv_free(jq->attrs);
  jq->attrs = attrs;
}

/*
 * Set an I/O policy -- a jq program.
 *
 * If, when given `null` as input, the policy jq program produces an object or
 * an array, or `true` or `null` or `false`, then that will be the policy data
 * and the program used at run time will be the `default_io_policy_check`
 * builtin using the constant data produced by running the original program
 * with `null` as input.
 *
 * Otherwise the given jq program will be the policy checker.
 *
 * At run-time the policy will be checked by calling the policy program (on a
 * separate jq_state!) with a request descriptor and the policy data as input.
 */
jv jq_set_io_policy(jq_state *jq, jv policy) {
  if (jq->io_policy)
    jq_teardown(&jq->io_policy);

  switch (jv_get_kind(policy)) {
  case JV_KIND_STRING:
    break;
  case JV_KIND_INVALID:
    jv_free(policy);
    return jv_invalid_with_msg(jv_string_fmt("Invalid I/O policy given"));
  case JV_KIND_NULL:
  case JV_KIND_TRUE:
  case JV_KIND_FALSE:
    jv_free(jq->io_policy_data);
    jq->io_policy_data = policy;
    return jv_true();
  default:
    jv_free(jq->io_policy_data);
    jq->io_policy_data = policy;
    policy = jv_string("default_io_policy_check");
    break;
  }

  jq_state *io_policy = jq_init();
  if (!io_policy)
    return jv_invalid_with_msg(jv_string("Out of memory"));

  int r = jq_compile_args(io_policy, jv_string_value(policy), jv_array());
  if (!r) {
    jq_teardown(&io_policy);
    jv ret = jv_invalid_with_msg(jv_string_fmt("Failed to compile I/O policy program: %s\n",
                                               jv_string_value(policy)));
    jv_free(policy);
    return ret;
  }

  /* Check if the program produces policy data, or evaluates policy */
  jq_start(io_policy, jv_null(), 0);
  copy_callbacks(jq, io_policy);
  jv v = jq_next(io_policy);
  jv ret;
  switch (jv_get_kind(v)) {
  case JV_KIND_OBJECT:
  case JV_KIND_ARRAY:
    /* Produces data to evaluate with the default policy checker */
    jv_free(jq->io_policy_data);
    jq->io_policy_data = v;
    v = jv_null();
    jq_teardown(&io_policy);
    io_policy = jq_init();
    if (!io_policy) {
      jv_free(policy);
      return jv_invalid_with_msg(jv_string("Out of memory")); // XXX leak
    }
    r = jq_compile_args(io_policy, "default_io_policy_check", jv_array());
    if (!r) {
      jq_teardown(&io_policy);
      jv ret = jv_invalid_with_msg(jv_string("Failed to compile default I/O policy program\n"));
      jv_free(policy);
      return ret;
    }
    jq->io_policy = io_policy;
    return jv_true();
  case JV_KIND_NULL:
  case JV_KIND_TRUE:
  case JV_KIND_FALSE:
    /* Produces simple data */
    jv_free(jq->io_policy_data);
    jq->io_policy_data = v;
    jq_teardown(&io_policy);
    return jv_true();
  case JV_KIND_INVALID:
    /* The program is a policy evaluator */
    jq->io_policy = io_policy;
    jv_free(policy);
    jv_free(v);
    return jv_true();
  case JV_KIND_STRING:
    /* Usage error */
    ret = jv_invalid_with_msg(jv_string_fmt("Strange policy value: %s", jv_string_value(policy)));
    jv_free(policy);
    return ret;
  default:
    /* Usage error */
    ret = jv_invalid_with_msg(jv_string_fmt("Strange policy value"));
    jv_free(policy);
    return ret;
  }
}

void jq_set_attr(jq_state *jq, jv attr, jv val) {
  jq->attrs = jv_object_set(jq->attrs, attr, val);
}

jv jq_get_attr(jq_state *jq, jv attr) {
  return jv_object_get(jv_copy(jq->attrs), attr);
}

void jq_dump_disassembly(jq_state *jq, int indent) {
  dump_disassembly(indent, jq->bc);
}

void jq_set_input_cb(jq_state *jq, jq_input_cb cb, void *data) {
  jq->input_cb = cb;
  jq->input_cb_data = data;
}

void jq_get_input_cb(jq_state *jq, jq_input_cb *cb, void **data) {
  *cb = jq->input_cb;
  *data = jq->input_cb_data;
}

void jq_set_debug_cb(jq_state *jq, jq_msg_cb cb, void *data) {
  jq->debug_cb = cb;
  jq->debug_cb_data = data;
}

void jq_get_debug_cb(jq_state *jq, jq_msg_cb *cb, void **data) {
  *cb = jq->debug_cb;
  *data = jq->debug_cb_data;
}

void
jq_halt(jq_state *jq, jv exit_code, jv error_message)
{
  assert(!jq->halted);
  jq->halted = 1;
  jq->exit_code = exit_code;
  jq->error_message = error_message;
}

int
jq_halted(jq_state *jq)
{
  return jq->halted;
}

int
jq_finished(jq_state *jq)
{
  return jq->halted || jq->finished;
}

jv jq_get_exit_code(jq_state *jq)
{
  return jv_copy(jq->exit_code);
}

jv jq_get_error_message(jq_state *jq)
{
  return jv_copy(jq->error_message);
}

void jq_set_input_mode(jq_state* jq, jq_input_mode mode) {
  jq->input_mode = mode;
}

jq_input_mode jq_get_input_mode(jq_state* jq) {
  return jq->input_mode;
}

void jq_set_input(jq_state* jq, jv start_input) {
  jv_free(jq->start_input);
  jq->start_input = start_input;
}

jv jq_get_input_copy(jq_state* jq) {
  return jv_copy(jq->start_input);
}

jv jq_io_policy_check(jq_state *jq, jv req) {
  if (!jq->io_policy) {
    jv_free(req);
    return jv_copy(jq->io_policy_data);
  }
  jq_start(jq->io_policy,
           JV_OBJECT(jv_string("io_policy"), jv_copy(jq->io_policy_data),
                     jv_string("io_request"), req), 0);
  copy_callbacks(jq, jq->io_policy);
  jv res = jq_next(jq->io_policy);
  switch (jv_get_kind(res)) {
  case JV_KIND_TRUE:
  case JV_KIND_FALSE:
    return res;
  case JV_KIND_INVALID:
    if (jv_invalid_has_msg(jv_copy(res)))
      return res;
    jv_free(res);
    return jv_false();
  default:
    jv_free(res);
    return jv_false();
  }
}

static void init_vtable(struct jq_plugin_vtable *vtable) {
  vtable->jv_string_append_buf = jv_string_append_buf;
  vtable->jv_object_iter_next = jv_object_iter_next;
  vtable->jq_get_lib_dirs = jq_get_lib_dirs;
  vtable->jv_mem_free = jv_mem_free;
  vtable->jq_get_error_cb = jq_get_error_cb;
  vtable->jv_string_vfmt = jv_string_vfmt;
  vtable->jv_number_value = jv_number_value;
  vtable->jv_mem_alloc_unguarded = jv_mem_alloc_unguarded;
  vtable->jv_true = jv_true;
  vtable->jv_object_set = jv_object_set;
  vtable->jv_mem_calloc = jv_mem_calloc;
  vtable->jv_parser_next = jv_parser_next;
  vtable->jv_dump_string = jv_dump_string;
  vtable->jv_invalid_get_msg = jv_invalid_get_msg;
  vtable->jq_util_input_set_parser = jq_util_input_set_parser;
  vtable->jv_copy = jv_copy;
  vtable->jv_get = jv_get;
  vtable->jv_mem_calloc_unguarded = jv_mem_calloc_unguarded;
  vtable->jv_string_value = jv_string_value;
  vtable->jv_identical = jv_identical;
  vtable->jq_get_prog_origin = jq_get_prog_origin;
  vtable->jv_invalid_with_msg = jv_invalid_with_msg;
  vtable->jv_equal = jv_equal;
  vtable->jq_dump_disassembly = jq_dump_disassembly;
  vtable->jv_object_iter = jv_object_iter;
  vtable->jv_dump = jv_dump;
  vtable->jv_array_sized = jv_array_sized;
  vtable->jv_string_append_str = jv_string_append_str;
  vtable->jq_get_exit_code = jq_get_exit_code;
  vtable->jv_getpath = jv_getpath;
  vtable->jq_get_attr = jq_get_attr;
  vtable->jq_halted = jq_halted;
  vtable->jq_finished = jq_finished;
  vtable->jv_array_length = jv_array_length;
  vtable->jv_array_concat = jv_array_concat;
  vtable->jv_string_implode = jv_string_implode;
  vtable->jv_string_explode = jv_string_explode;
  vtable->jq_start = jq_start;
  vtable->jq_teardown = jq_teardown;
  vtable->jv_parser_remaining = jv_parser_remaining;
  vtable->jq_util_input_get_position = jq_util_input_get_position;
  vtable->jv_group = jv_group;
  vtable->jv_false = jv_false;
  vtable->jv_string_fmt = jv_string_fmt;
  vtable->jq_set_input_cb = jq_set_input_cb;
  vtable->jq_util_input_init = jq_util_input_init;
  vtable->jv_null = jv_null;
  vtable->jv_setpath = jv_setpath;
  vtable->jv_string_split = jv_string_split;
  vtable->jv_set = jv_set;
  vtable->jv_mem_strdup_unguarded = jv_mem_strdup_unguarded;
  vtable->jq_get_jq_origin = jq_get_jq_origin;
  vtable->jq_halt = jq_halt;
  vtable->jv_show = jv_show;
  vtable->jq_init = jq_init;
  vtable->jv_kind_name = jv_kind_name;
  vtable->jv_string_append_codepoint = jv_string_append_codepoint;
  vtable->jq_get_error_message = jq_get_error_message;
  vtable->jv_array_get = jv_array_get;
  vtable->jv_object_merge = jv_object_merge;
  vtable->jq_compile_args = jq_compile_args;
  vtable->jq_util_input_next_input = jq_util_input_next_input;
  vtable->jv_dump_string_trunc = jv_dump_string_trunc;
  vtable->jq_next = jq_next;
  vtable->jv_parse_sized = jv_parse_sized;
  vtable->jv_cmp = jv_cmp;
  vtable->jv_number = jv_number;
  vtable->jv_dumpf = jv_dumpf;
  vtable->jv_object_get = jv_object_get;
  vtable->jv_parse = jv_parse;
  vtable->jq_set_attr = jq_set_attr;
  vtable->jq_util_input_get_current_line = jq_util_input_get_current_line;
  vtable->jv_parser_new = jv_parser_new;
  vtable->jv_object_iter_value = jv_object_iter_value;
  vtable->jv_invalid_has_msg = jv_invalid_has_msg;
  vtable->jv_string_length_bytes = jv_string_length_bytes;
  vtable->jv_object_iter_key = jv_object_iter_key;
  vtable->jq_report_error = jq_report_error;
  vtable->jv_is_integer = jv_is_integer;
  vtable->jv_string = jv_string;
  vtable->jv_contains = jv_contains;
  vtable->jv_mem_alloc = jv_mem_alloc;
  vtable->jv_string_hash = jv_string_hash;
  vtable->jv_string_slice = jv_string_slice;
  vtable->jv_load_file = jv_load_file;
  vtable->jq_compile = jq_compile;
  vtable->jv_get_kind = jv_get_kind;
  vtable->jv_object_merge_recursive = jv_object_merge_recursive;
  vtable->jv_bool = jv_bool;
  vtable->jq_util_input_get_current_filename = jq_util_input_get_current_filename;
  vtable->jv_object_iter_valid = jv_object_iter_valid;
  vtable->jv_has = jv_has;
  vtable->jv_array_set = jv_array_set;
  vtable->jv_object_delete = jv_object_delete;
  vtable->jq_set_debug_cb = jq_set_debug_cb;
  vtable->jv_string_empty = jv_string_empty;
  vtable->jq_realpath = jq_realpath;
  vtable->jq_set_error_cb = jq_set_error_cb;
  vtable->jq_util_input_next_input_cb = jq_util_input_next_input_cb;
  vtable->jv_get_refcnt = jv_get_refcnt;
  vtable->jq_set_colors = jq_set_colors;
  vtable->jv_array_slice = jv_array_slice;
  vtable->jv_mem_realloc = jv_mem_realloc;
  vtable->jv_array = jv_array;
  vtable->jv_parser_set_buf = jv_parser_set_buf;
  vtable->jv_keys_unsorted = jv_keys_unsorted;
  vtable->jv_delpaths = jv_delpaths;
  vtable->jv_sort = jv_sort;
  vtable->jv_parser_free = jv_parser_free;
  vtable->jv_array_indexes = jv_array_indexes;
  vtable->jv_string_concat = jv_string_concat;
  vtable->jv_object_has = jv_object_has;
  vtable->jv_invalid = jv_invalid;
  vtable->jq_util_input_free = jq_util_input_free;
  vtable->jv_object_length = jv_object_length;
  vtable->jq_get_input_cb = jq_get_input_cb;
  vtable->jv_string_sized = jv_string_sized;
  vtable->jv_string_length_codepoints = jv_string_length_codepoints;
  vtable->jq_format_error = jq_format_error;
  vtable->jv_array_append = jv_array_append;
  vtable->jv_string_indexes = jv_string_indexes;
  vtable->jv_free = jv_free;
  vtable->jq_get_debug_cb = jq_get_debug_cb;
  vtable->jq_util_input_errors = jq_util_input_errors;
  vtable->jv_mem_strdup = jv_mem_strdup;
  vtable->jq_util_input_add_input = jq_util_input_add_input;
  vtable->jv_object = jv_object;
  vtable->jv_keys = jv_keys;
  vtable->jq_set_attrs = jq_set_attrs;
  vtable->jq_handle_get_kind = jq_handle_get_kind;
  vtable->jq_handle_get = jq_handle_get;
  vtable->jq_handle_new = jq_handle_new;
  vtable->jq_handle_reset = jq_handle_reset;
  vtable->jq_handle_close = jq_handle_close;
  vtable->jq_handle_write = jq_handle_write;
  vtable->jq_handle_read = jq_handle_read;
  vtable->jq_handle_stat = jq_handle_stat;
  vtable->jq_handle_eof = jq_handle_eof;
}
