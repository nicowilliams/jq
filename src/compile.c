#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for strdup
#endif
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "compile.h"
#include "bytecode.h"
#include "locfile.h"
#include "jv_alloc.h"
#include "linker.h"

/*
  The intermediate representation for jq filters is as a sequence of
  struct inst, which form a doubly-linked list via the next and prev
  pointers.

  A "block" represents a sequence of "struct inst", which may be
  empty.

  Blocks are generated by the parser bottom-up, so may have free
  variables (refer to things not defined). See inst.bound_by and
  inst.symbol.
 */
struct inst {
  struct inst* next;
  struct inst* prev;

  opcode op;

  struct {
    uint16_t intval;
    struct inst* target;
    jv constant;
    const struct cfunction* cfunc;
  } imm;

  struct locfile* locfile;
  location source;

  // Binding
  // An instruction requiring binding (for parameters/variables/functions)
  // is in one of three states:
  //   inst->bound_by = NULL  - Unbound free variable
  //   inst->bound_by = inst  - This instruction binds a variable
  //   inst->bound_by = other - Uses variable bound by other instruction
  // Unbound instructions (references to other things that may or may not
  // exist) are created by "gen_foo_unbound", and bindings are created by
  // block_bind(definition, body), which binds all instructions in
  // body which are unboudn and refer to "definition" by name.
  struct inst* bound_by;
  char* symbol;

  int nformals;
  int nactuals;

  block subfn;   // used by CLOSURE_CREATE (body of function)
  block arglist; // used by CLOSURE_CREATE (formals) and CALL_JQ (arguments)

  // This instruction is compiled as part of which function?
  // (only used during block_compile)
  struct bytecode* compiled;

  int bytecode_pos; // position just after this insn
};

static inst* inst_new(opcode op) {
  inst* i = jv_mem_alloc(sizeof(inst));
  i->next = i->prev = 0;
  i->op = op;
  i->bytecode_pos = -1;
  i->bound_by = 0;
  i->symbol = 0;
  i->nformals = -1;
  i->nactuals = -1;
  i->subfn = gen_noop();
  i->arglist = gen_noop();
  i->source = UNKNOWN_LOCATION;
  i->locfile = 0;
  return i;
}

static void inst_free(struct inst* i) {
  jv_mem_free(i->symbol);
  block_free(i->subfn);
  block_free(i->arglist);
  if (i->locfile)
    locfile_free(i->locfile);
  if (opcode_describe(i->op)->flags & OP_HAS_CONSTANT) {
    jv_free(i->imm.constant);
  }
  jv_mem_free(i);
}

static block inst_block(inst* i) {
  block b = {i,i};
  return b;
}

int block_is_single(block b) {
  return b.first && b.first == b.last;
}

static inst* block_take(block* b) {
  if (b->first == 0) return 0;
  inst* i = b->first;
  if (i->next) {
    i->next->prev = 0;
    b->first = i->next;
    i->next = 0;
  } else {
    b->first = 0;
    b->last = 0;
  }
  return i;
}

block gen_location(location loc, struct locfile* l, block b) {
  for (inst* i = b.first; i; i = i->next) {
    if (i->source.start == UNKNOWN_LOCATION.start &&
        i->source.end == UNKNOWN_LOCATION.end) {
      i->source = loc;
      i->locfile = locfile_retain(l);
    }
  }
  return b;
}

block gen_noop() {
  block b = {0,0};
  return b;
}

int block_is_noop(block b) {
  return (b.first == 0 && b.last == 0);
}

block gen_op_simple(opcode op) {
  assert(opcode_describe(op)->length == 1);
  return inst_block(inst_new(op));
}


block gen_const(jv constant) {
  assert(opcode_describe(LOADK)->flags & OP_HAS_CONSTANT);
  inst* i = inst_new(LOADK);
  i->imm.constant = constant;
  return inst_block(i);
}

block gen_const_global(jv constant, const char *name) {
  assert((opcode_describe(STORE_GLOBAL)->flags & (OP_HAS_CONSTANT | OP_HAS_VARIABLE | OP_HAS_BINDING)) ==
         (OP_HAS_CONSTANT | OP_HAS_VARIABLE | OP_HAS_BINDING));
  inst* i = inst_new(STORE_GLOBAL);
  i->imm.constant = constant;
  i->symbol = strdup(name);
  return inst_block(i);
}

int block_is_const(block b) {
  return (block_is_single(b) && b.first->op == LOADK);
}

int block_is_const_inf(block b) {
  return (block_is_single(b) && b.first->op == LOADK &&
          jv_get_kind(b.first->imm.constant) == JV_KIND_NUMBER &&
          isinf(jv_number_value(b.first->imm.constant)));
}

jv_kind block_const_kind(block b) {
  assert(block_is_const(b));
  return jv_get_kind(b.first->imm.constant);
}

jv block_const(block b) {
  assert(block_is_const(b));
  return jv_copy(b.first->imm.constant);
}

block gen_op_target(opcode op, block target) {
  assert(opcode_describe(op)->flags & OP_HAS_BRANCH);
  assert(target.last);
  inst* i = inst_new(op);
  i->imm.target = target.last;
  return inst_block(i);
}

block gen_op_targetlater(opcode op) {
  assert(opcode_describe(op)->flags & OP_HAS_BRANCH);
  inst* i = inst_new(op);
  i->imm.target = 0;
  return inst_block(i);
}
void inst_set_target(block b, block target) {
  assert(block_is_single(b));
  assert(opcode_describe(b.first->op)->flags & OP_HAS_BRANCH);
  assert(target.last);
  b.first->imm.target = target.last;
}

block gen_op_unbound(opcode op, const char* name) {
  assert(opcode_describe(op)->flags & OP_HAS_BINDING);
  inst* i = inst_new(op);
  i->symbol = strdup(name);
  return inst_block(i);
}

block gen_op_var_fresh(opcode op, const char* name) {
  assert(opcode_describe(op)->flags & OP_HAS_VARIABLE);
  return block_bind(gen_op_unbound(op, name),
                    gen_noop(), OP_HAS_VARIABLE);
}

block gen_op_bound(opcode op, block binder) {
  assert(block_is_single(binder));
  block b = gen_op_unbound(op, binder.first->symbol);
  b.first->bound_by = binder.first;
  return b;
}


static void inst_join(inst* a, inst* b) {
  assert(a && b);
  assert(!a->next);
  assert(!b->prev);
  a->next = b;
  b->prev = a;
}

void block_append(block* b, block b2) {
  if (b2.first) {
    if (b->last) {
      inst_join(b->last, b2.first);
    } else {
      b->first = b2.first;
    }
    b->last = b2.last;
  }
}

block block_join(block a, block b) {
  block c = a;
  block_append(&c, b);
  return c;
}

int block_has_only_binders_and_imports(block binders, int bindflags) {
  bindflags |= OP_HAS_BINDING;
  for (inst* curr = binders.first; curr; curr = curr->next) {
    if ((opcode_describe(curr->op)->flags & bindflags) != bindflags && curr->op != DEPS && curr->op != MODULEMETA) {
      return 0;
    }
  }
  return 1;
}

static int inst_is_binder(inst *i, int bindflags) {
  return !((opcode_describe(i->op)->flags & bindflags) != bindflags && i->op != MODULEMETA);
}

int block_has_only_binders(block binders, int bindflags) {
  bindflags |= OP_HAS_BINDING;
  bindflags &= ~OP_BIND_WILDCARD;
  for (inst* curr = binders.first; curr; curr = curr->next) {
    if ((opcode_describe(curr->op)->flags & bindflags) != bindflags && curr->op != MODULEMETA) {
      return 0;
    }
  }
  return 1;
}

// Count a binder's (function) formal params
static int block_count_formals(block b) {
  int args = 0;
  if (b.first->op == CLOSURE_CREATE_C)
    return b.first->imm.cfunc->nargs - 1;
  for (inst* i = b.first->arglist.first; i; i = i->next) {
    assert(i->op == CLOSURE_PARAM);
    args++;
  }
  return args;
}

// Count a call site's actual params
static int block_count_actuals(block b) {
  int args = 0;
  for (inst* i = b.first; i; i = i->next) {
    switch (i->op) {
    default: assert(0 && "Unknown function type"); break;
    case CLOSURE_CREATE:
    case CLOSURE_PARAM:
    case CLOSURE_CREATE_C:
      args++;
      break;
    }
  }
  return args;
}

static int block_count_refs(block binder, block body) {
  int nrefs = 0;
  for (inst* i = body.first; i; i = i->next) {
    if (i != binder.first && i->bound_by == binder.first) {
      nrefs++;
    }
    // counting recurses into closures
    nrefs += block_count_refs(binder, i->subfn);
    // counting recurses into argument list
    nrefs += block_count_refs(binder, i->arglist);
  }
  return nrefs;
}

static int block_bind_subblock(block binder, block body, int bindflags, int break_distance) {
  assert(block_is_single(binder));
  assert((opcode_describe(binder.first->op)->flags & bindflags) == (bindflags & ~OP_BIND_WILDCARD));
  assert(binder.first->symbol);
  assert(binder.first->bound_by == 0 || binder.first->bound_by == binder.first);
  assert(break_distance >= 0);

  binder.first->bound_by = binder.first;
  if (binder.first->nformals == -1)
    binder.first->nformals = block_count_formals(binder);
  int nrefs = 0;
  for (inst* i = body.first; i; i = i->next) {
    int flags = opcode_describe(i->op)->flags;
    if ((flags & bindflags) == (bindflags & ~OP_BIND_WILDCARD) && i->bound_by == 0 &&
        (!strcmp(i->symbol, binder.first->symbol) ||
         // Check for break/break2/break3; see parser.y
         ((bindflags & OP_BIND_WILDCARD) && i->symbol[0] == '*' &&
          break_distance <= 3 && (i->symbol[1] == '1' + break_distance) &&
          i->symbol[2] == '\0'))) {
      // bind this instruction
      if (i->op == CALL_JQ && i->nactuals == -1)
        i->nactuals = block_count_actuals(i->arglist);
      if (i->nactuals == -1 || i->nactuals == binder.first->nformals) {
        i->bound_by = binder.first;
        nrefs++;
      }
    } else if ((flags & bindflags) == (bindflags & ~OP_BIND_WILDCARD) && i->bound_by != 0 &&
               !strncmp(binder.first->symbol, "*anonlabel", sizeof("*anonlabel") - 1) &&
               !strncmp(i->symbol, "*anonlabel", sizeof("*anonlabel") - 1)) {
      // Increment the break distance required for this binder to match
      // a break whenever we come across a STOREV of *anonlabel...
      break_distance++;
    }
    // binding recurses into closures
    nrefs += block_bind_subblock(binder, i->subfn, bindflags, break_distance);
    // binding recurses into argument list
    nrefs += block_bind_subblock(binder, i->arglist, bindflags, break_distance);
  }
  return nrefs;
}

static int block_bind_each(block binder, block body, int bindflags) {
  assert(block_has_only_binders(binder, bindflags));
  bindflags |= OP_HAS_BINDING;
  int nrefs = 0;
  for (inst* curr = binder.first; curr; curr = curr->next) {
    nrefs += block_bind_subblock(inst_block(curr), body, bindflags, 0);
  }
  return nrefs;
}

block block_bind(block binder, block body, int bindflags) {
  block_bind_each(binder, body, bindflags);
  return block_join(binder, body);
}

block block_bind_library(block binder, block body, int bindflags, const char *libname) {
  bindflags |= OP_HAS_BINDING;
  int nrefs = 0;
  int matchlen = (libname == NULL) ? 0 : strlen(libname);
  char *matchname = jv_mem_alloc(matchlen+2+1);
  matchname[0] = '\0';
  if (libname != NULL && libname[0] != '\0') {
    strcpy(matchname,libname);
    strcpy(matchname+matchlen, "::");
    matchlen += 2;
  }
  assert(block_has_only_binders(binder, bindflags));
  for (inst *curr = binder.first; curr; curr = curr->next) {
    int bindflags2 = bindflags;
    char* cname = curr->symbol;
    char* tname = jv_mem_alloc(strlen(curr->symbol)+matchlen+1);
    strcpy(tname, matchname);
    strcpy(tname+matchlen, curr->symbol);

    // Ew
    if ((opcode_describe(curr->op)->flags & (OP_HAS_VARIABLE | OP_HAS_CONSTANT)))
      bindflags2 = OP_HAS_VARIABLE | OP_HAS_BINDING;

    // This mutation is ugly, even if we undo it
    curr->symbol = tname;
    nrefs += block_bind_subblock(inst_block(curr), body, bindflags2, 0);
    curr->symbol = cname;
    free(tname);
  }
  free(matchname);
  return body; // We don't return a join because we don't want those sticking around...
}

// Bind binder to body and throw away any defs in binder not referenced
// (directly or indirectly) from body.
block block_bind_referenced(block binder, block body, int bindflags) {
  assert(block_has_only_binders(binder, bindflags));
  bindflags |= OP_HAS_BINDING;
  block refd = gen_noop();
  block unrefd = gen_noop();
  int nrefs;
  for (int last_kept = 0, kept = 0; ; ) {
    for (inst* curr; (curr = block_take(&binder));) {
      block b = inst_block(curr);
      nrefs = block_bind_each(b, body, bindflags);
      // Check if this binder is referenced from any of the ones we
      // already know are referenced by body.
      nrefs += block_count_refs(b, refd);
      nrefs += block_count_refs(b, body);
      if (nrefs) {
        refd = BLOCK(refd, b);
        kept++;
      } else {
        unrefd = BLOCK(unrefd, b);
      }
    }
    if (kept == last_kept)
      break;
    last_kept = kept;
    binder = unrefd;
    unrefd = gen_noop();
  }
  block_free(unrefd);
  return block_join(refd, body);
}

block block_drop_unreferenced(block body) {
  inst* curr;
  block refd = gen_noop();
  block unrefd = gen_noop();
  int drop;
  do {
    drop = 0;
    while ((curr = block_take(&body)) && curr->op != TOP) {
      block b = inst_block(curr);
      if (block_count_refs(b,refd) + block_count_refs(b,body) == 0) {
        unrefd = BLOCK(unrefd, b);
        drop++;
      } else {
        refd = BLOCK(refd, b);
      }
    }
    if (curr && curr->op == TOP) {
      body = BLOCK(inst_block(curr),body);
    }
    body = BLOCK(refd, body);
    refd = gen_noop();
  } while (drop != 0);
  block_free(unrefd);
  return body;
}

jv block_take_imports(block* body) {
  jv imports = jv_array();

  inst* top = NULL;
  if (body->first && body->first->op == TOP) {
    top = block_take(body);
  }
  while (body->first && (body->first->op == MODULEMETA || body->first->op == DEPS)) {
    inst* dep = block_take(body);
    if (dep->op == DEPS) {
      imports = jv_array_append(imports, jv_copy(dep->imm.constant));
    }
    inst_free(dep);
  }
  if (top) {
    *body = block_join(inst_block(top),*body);
  }
  return imports;
}

block gen_module(block metadata) {
  inst* i = inst_new(MODULEMETA);
  i->imm.constant = block_const(metadata);
  if (jv_get_kind(i->imm.constant) != JV_KIND_OBJECT)
    i->imm.constant = jv_object_set(jv_object(), jv_string("metadata"), i->imm.constant);
  block_free(metadata);
  return inst_block(i);
}

jv block_module_meta(block b) {
  if (b.first != NULL && b.first->op == MODULEMETA)
    return jv_copy(b.first->imm.constant);
  return jv_null();
}

block gen_import(const char* name, const char* as, int is_data) {
  inst* i = inst_new(DEPS);
  jv meta = jv_object();
  if (as != NULL)
    meta = jv_object_set(meta, jv_string("as"), jv_string(as));
  meta = jv_object_set(meta, jv_string("is_data"), is_data ? jv_true() : jv_false());
  meta = jv_object_set(meta, jv_string("relpath"), jv_string(name));
  i->imm.constant = meta;
  return inst_block(i);
}

block gen_import_meta(block import, block metadata) {
  assert(block_is_single(import) && import.first->op == DEPS);
  assert(block_is_const(metadata) && block_const_kind(metadata) == JV_KIND_OBJECT);
  inst *i = import.first;
  i->imm.constant = jv_object_merge(block_const(metadata), i->imm.constant);
  block_free(metadata);
  return import;
}

block gen_function(const char* name, block formals, block body) {
  inst* i = inst_new(CLOSURE_CREATE);
  for (inst* i = formals.last; i; i = i->prev) {
    if (i->op == CLOSURE_PARAM_REGULAR) {
      i->op = CLOSURE_PARAM;
      body = gen_var_binding(gen_call(i->symbol, gen_noop()), i->symbol, body);
    }
    block_bind_subblock(inst_block(i), body, OP_IS_CALL_PSEUDO | OP_HAS_BINDING, 0);
  }
  i->subfn = body;
  i->symbol = strdup(name);
  i->arglist = formals;
  block b = inst_block(i);
  block_bind_subblock(b, b, OP_IS_CALL_PSEUDO | OP_HAS_BINDING, 0);
  return b;
}

block gen_param_regular(const char* name) {
  return gen_op_unbound(CLOSURE_PARAM_REGULAR, name);
}

block gen_param(const char* name) {
  return gen_op_unbound(CLOSURE_PARAM, name);
}

block gen_lambda(block body) {
  return gen_function("@lambda", gen_noop(), body);
}

block gen_call(const char* name, block args) {
  block b = gen_op_unbound(CALL_JQ, name);
  b.first->arglist = args;
  return b;
}



block gen_subexp(block a) {
  return BLOCK(gen_op_simple(SUBEXP_BEGIN), a, gen_op_simple(SUBEXP_END));
}

block gen_both(block a, block b) {
  block jump = gen_op_targetlater(JUMP);
  block fork = gen_op_target(FORK, jump);
  block c = BLOCK(fork, a, jump, b);
  inst_set_target(jump, c);
  return c;
}

block gen_const_object(block expr) {
  int is_const = 1;
  jv o = jv_object();
  jv k = jv_null();
  jv v = jv_null();
  for (inst *i = expr.first; i; i = i->next) {
    if (i->op != SUBEXP_BEGIN ||
        i->next == NULL ||
        i->next->op != LOADK ||
        i->next->next == NULL ||
        i->next->next->op != SUBEXP_END) {
      is_const = 0;
      break;
    }
    k = jv_copy(i->next->imm.constant);
    i = i->next->next->next;
    if (i == NULL ||
        i->op != SUBEXP_BEGIN ||
        i->next == NULL ||
        i->next->op != LOADK ||
        i->next->next == NULL ||
        i->next->next->op != SUBEXP_END) {
      is_const = 0;
      break;
    }
    v = jv_copy(i->next->imm.constant);
    i = i->next->next->next;
    if (i == NULL || i->op != INSERT) {
      is_const = 0;
      break;
    }
    if (jv_get_kind(k) != JV_KIND_STRING) {
      is_const = 0;
      break;
    }
    o = jv_object_set(o, k, v);
    k = jv_null();
    v = jv_null();
  }
  if (!is_const) {
    jv_free(o);
    jv_free(k);
    jv_free(v);
    block b = {0,0};
    return b;
  }
  block_free(expr);
  return gen_const(o);
}

static block gen_const_array(block expr) {
  /*
   * An expr of all constant elements looks like this:
   *
   * 0009 FORK 0027
   * 0011 FORK 0023
   * 0013 FORK 0019
   * 0015 LOADK 1
   * 0017 JUMP 0021
   * 0019 LOADK 2
   * 0021 JUMP 0025
   * 0023 LOADK 3
   * 0025 JUMP 0029
   * 0027 LOADK 4
   *
   * That's: N-1 commas for N elements, N LOADKs, and a JUMP between
   * every LOADK.  The sequence ends in a LOADK.  Any deviation and it's
   * not a list of constants.
   *
   * Here we check for this pattern almost exactly.  We don't check that
   * the targets of the FORK and JUMP instructions are in the right
   * sequence.
   */
  int all_const = 1;
  int commas = 0;
  int normal = 1;
  jv a = jv_array();
  for (inst *i = expr.first; i; i = i->next) {
    if (i->op == FORK) {
      commas++;
      if (i->imm.target == NULL || i->imm.target->op != JUMP ||
          jv_array_length(jv_copy(a)) > 0) {
        normal = 0;
        break;
      }
    } else if (all_const && i->op == LOADK) {
      if (i->next != NULL && i->next->op != JUMP) {
        normal = 0;
        break;
      }
      a = jv_array_append(a, jv_copy(i->imm.constant));
    } else if (i->op != JUMP || i->imm.target == NULL ||
               i->imm.target->op != LOADK) {
      all_const = 0;
    }
  }

  if (all_const && normal &&
      (expr.last == NULL || expr.last->op == LOADK) &&
      jv_array_length(jv_copy(a)) == commas + 1) {
    block_free(expr);
    return gen_const(a);
  }

  jv_free(a);
  block b = {0,0};
  return b;
}

block gen_collect(block expr) {
  block const_array = gen_const_array(expr);
  if (const_array.first != NULL)
    return const_array;

  block array_var = gen_op_var_fresh(STOREV, "collect");
  block c = BLOCK(gen_op_simple(DUP), gen_const(jv_array()), array_var);

  block tail = BLOCK(gen_op_bound(APPEND, array_var),
                     gen_op_simple(BACKTRACK));

  return BLOCK(c,
               gen_op_target(FORK, tail),
               expr,
               tail,
               gen_op_bound(LOADVN, array_var));
}

static block bind_matcher(block matcher, block body) {
  // cannot call block_bind(matcher, body) because that requires
  // block_has_only_binders(matcher), which is not true here as matchers
  // may also contain code to extract the correct elements
  for (inst* i = matcher.first; i; i = i->next) {
    if (i->op == STOREV && !i->bound_by)
      block_bind_subblock(inst_block(i), body, OP_HAS_VARIABLE, 0);
  }
  return BLOCK(matcher, body);
}

block gen_reduce(block source, block matcher, block init, block body) {
  block res_var = gen_op_var_fresh(STOREV, "reduce");
  block loop = BLOCK(gen_op_simple(DUPN),
                     source,
                     bind_matcher(matcher,
                                  BLOCK(gen_op_bound(LOADVN, res_var),
                                        body,
                                        gen_op_bound(STOREV, res_var))),
                     gen_op_simple(BACKTRACK));
  return BLOCK(gen_op_simple(DUP),
               init,
               res_var,
               gen_op_target(FORK, loop),
               loop,
               gen_op_bound(LOADVN, res_var));
}

block gen_foreach(block source, block matcher, block init, block update, block extract) {
  block output = gen_op_targetlater(JUMP);
  block state_var = gen_op_var_fresh(STOREV, "foreach");
  block loop = BLOCK(gen_op_simple(DUPN),
                     // get a value from the source expression:
                     source,
                     // destructure the value into variable(s) for all the code
                     // in the body to see
                     bind_matcher(matcher,
                                  // load the loop state variable
                                  BLOCK(gen_op_bound(LOADVN, state_var),
                                        // generate updated state
                                        update,
                                        // save the updated state for value extraction
                                        gen_op_simple(DUP),
                                        // save new state
                                        gen_op_bound(STOREV, state_var),
                                        // extract an output...
                                        extract,
                                        // ...and output it by jumping
                                        // past the BACKTRACK that comes
                                        // right after the loop body,
                                        // which in turn is there
                                        // because...
                                        //
                                        // (Incidentally, extract can also
                                        // backtrack, e.g., if it calls
                                        // empty, in which case we don't
                                        // get here.)
                                        output)));
  block foreach = BLOCK(gen_op_simple(DUP),
                        init,
                        state_var,
                        gen_op_target(FORK, loop),
                        loop,
                        // ...at this point `foreach`'s original input
                        // will be on top of the stack, and we don't
                        // want to output it, so we backtrack.
                        gen_op_simple(BACKTRACK));
  inst_set_target(output, foreach); // make that JUMP go bast the BACKTRACK at the end of the loop
  return foreach;
}

block gen_definedor(block a, block b) {
  // var found := false
  block found_var = gen_op_var_fresh(STOREV, "found");
  block init = BLOCK(gen_op_simple(DUP), gen_const(jv_false()), found_var);

  // if found, backtrack. Otherwise execute b
  block backtrack = gen_op_simple(BACKTRACK);
  block tail = BLOCK(gen_op_simple(DUP),
                     gen_op_bound(LOADV, found_var),
                     gen_op_target(JUMP_F, backtrack),
                     backtrack,
                     gen_op_simple(POP),
                     b);

  // try again
  block if_notfound = gen_op_simple(BACKTRACK);

  // found := true, produce result
  block if_found = BLOCK(gen_op_simple(DUP),
                         gen_const(jv_true()),
                         gen_op_bound(STOREV, found_var),
                         gen_op_target(JUMP, tail));

  return BLOCK(init,
               gen_op_target(FORK, if_notfound),
               a,
               gen_op_target(JUMP_F, if_found),
               if_found,
               if_notfound,
               tail);
}

int block_has_main(block top) {
  for (inst *c = top.first; c; c = c->next) {
    if (c->op == TOP)
      return 1;
  }
  return 0;
}

int block_is_funcdef(block b) {
  if (b.first != NULL && b.first->op == CLOSURE_CREATE)
    return 1;
  return 0;
}

block gen_condbranch(block iftrue, block iffalse) {
  iftrue = BLOCK(iftrue, gen_op_target(JUMP, iffalse));
  return BLOCK(gen_op_target(JUMP_F, iftrue), iftrue, iffalse);
}

block gen_and(block a, block b) {
  // a and b = if a then (if b then true else false) else false
  return BLOCK(gen_op_simple(DUP), a,
               gen_condbranch(BLOCK(gen_op_simple(POP),
                                    b,
                                    gen_condbranch(gen_const(jv_true()),
                                                   gen_const(jv_false()))),
                              BLOCK(gen_op_simple(POP), gen_const(jv_false()))));
}

block gen_or(block a, block b) {
  // a or b = if a then true else (if b then true else false)
  return BLOCK(gen_op_simple(DUP), a,
               gen_condbranch(BLOCK(gen_op_simple(POP), gen_const(jv_true())),
                              BLOCK(gen_op_simple(POP),
                                    b,
                                    gen_condbranch(gen_const(jv_true()),
                                                   gen_const(jv_false())))));
}

block gen_var_binding(block var, const char* name, block body) {
  return gen_destructure(var, gen_op_unbound(STOREV, name), body);
}

block gen_array_matcher(block left, block curr) {
  int index;
  if (block_is_noop(left))
    index = 0;
  else {
    // `left` was returned by this function, so the third inst is the
    // constant containing the previously used index
    assert(left.first->op == DUP);
    assert(left.first->next->op == SUBEXP_BEGIN);
    assert(left.first->next->next->op == LOADK);
    index = 1 + (int) jv_number_value(left.first->next->next->imm.constant);
  }

  // `left` goes at the end so that the const index is in a predictable place
  return BLOCK(gen_op_simple(DUP), gen_subexp(gen_const(jv_number(index))),
               gen_op_simple(INDEX), curr, left);
}

block gen_object_matcher(block name, block curr) {
  return BLOCK(gen_op_simple(DUP), gen_subexp(name), gen_op_simple(INDEX),
               curr);
}

block gen_destructure(block var, block matcher, block body) {
  // var bindings can be added after coding the program; leave the TOP first.
  block top = gen_noop();
  if (body.first && body.first->op == TOP)
    top = inst_block(block_take(&body));

  return BLOCK(top, gen_op_simple(DUP), var, bind_matcher(matcher, body));
}

// Like gen_var_binding(), but bind `break`'s wildcard unbound variable
static block gen_wildvar_binding(block var, const char* name, block body) {
  return BLOCK(gen_op_simple(DUP), var,
               block_bind(gen_op_unbound(STOREV, name),
                          body, OP_HAS_VARIABLE | OP_BIND_WILDCARD));
}

block gen_cond(block cond, block iftrue, block iffalse) {
  return BLOCK(gen_op_simple(DUP), cond,
               gen_condbranch(BLOCK(gen_op_simple(POP), iftrue),
                              BLOCK(gen_op_simple(POP), iffalse)));
}

block gen_try_handler(block handler) {
  // Quite a pain just to hide jq's internal errors.
  return gen_cond(// `if type=="object" and .__jq
                  gen_and(gen_call("_equal",
                                   BLOCK(gen_lambda(gen_const(jv_string("object"))),
                                         gen_lambda(gen_noop()))),
                          BLOCK(gen_subexp(gen_const(jv_string("__jq"))),
                                gen_noop(),
                                gen_op_simple(INDEX))),
                  // `then error`
                  gen_call("error", gen_noop()),
                  // `else HANDLER end`
                  handler);
}

block gen_try(block exp, block handler) {
  /*
   * Produce something like:
   *  FORK_OPT <address of handler>
   *  <exp>
   *  JUMP <end of handler>
   *  <handler>
   *
   * If this is not an internal try/catch, then catch and re-raise
   * internal errors to prevent them from leaking.
   *
   * The handler will only execute if we backtrack to the FORK_OPT with
   * an error (exception).  If <exp> produces no value then FORK_OPT
   * will backtrack (propagate the `empty`, as it were.  If <exp>
   * produces a value then we'll execute whatever bytecode follows this
   * sequence.
   */
  if (!handler.first && !handler.last)
    // A hack to deal with `.` as the handler; we could use a real NOOP here
    handler = BLOCK(gen_op_simple(DUP), gen_op_simple(POP), handler);
  exp = BLOCK(exp, gen_op_target(JUMP, handler));
  return BLOCK(gen_op_target(FORK_OPT, exp), exp, handler);
}

block gen_label(const char *label, block exp) {
  block cond = gen_call("_equal",
                        BLOCK(gen_lambda(gen_noop()),
                              gen_lambda(gen_op_unbound(LOADV, label))));
  return gen_wildvar_binding(gen_op_simple(GENLABEL), label,
                             BLOCK(gen_op_simple(POP),
                                   // try exp catch if . == $label
                                   //               then empty
                                   //               else error end
                                   //
                                   // Can't use gen_binop(), as that's firmly
                                   // stuck in parser.y as it refers to things
                                   // like EQ.
                                   gen_try(exp,
                                           gen_cond(cond,
                                                    gen_op_simple(BACKTRACK),
                                                    gen_call("error", gen_noop())))));
}

block gen_cbinding(const struct cfunction* cfunctions, int ncfunctions, block code) {
  for (int cfunc=0; cfunc<ncfunctions; cfunc++) {
    inst* i = inst_new(CLOSURE_CREATE_C);
    i->imm.cfunc = &cfunctions[cfunc];
    i->symbol = strdup(i->imm.cfunc->name);
    code = block_bind(inst_block(i), code, OP_IS_CALL_PSEUDO);
  }
  return code;
}

static uint16_t nesting_level(struct bytecode* bc, inst* target) {
  uint16_t level = 0;
  assert(bc && target && target->compiled);
  while (bc && target->compiled != bc) {
    level++;
    bc = bc->parent;
  }
  assert(bc && bc == target->compiled);
  return level;
}

static int count_cfunctions(block b) {
  int n = 0;
  for (inst* i = b.first; i; i = i->next) {
    if (i->op == CLOSURE_CREATE_C) n++;
    n += count_cfunctions(i->subfn);
  }
  return n;
}


// Expands call instructions into a calling sequence
static int expand_call_arglist(block* b) {
  int errors = 0;
  block ret = gen_noop();
  for (inst* curr; (curr = block_take(b));) {
    if (opcode_describe(curr->op)->flags & OP_HAS_BINDING) {
      if (!curr->bound_by) {
        if (curr->symbol[0] == '*' && curr->symbol[1] >= '1' && curr->symbol[1] <= '3' && curr->symbol[2] == '\0')
          locfile_locate(curr->locfile, curr->source, "jq: error: break used outside labeled control structure");
        else
          locfile_locate(curr->locfile, curr->source, "jq: error: %s/%d is not defined", curr->symbol, block_count_actuals(curr->arglist));
        errors++;
        // don't process this instruction if it's not well-defined
        ret = BLOCK(ret, inst_block(curr));
        continue;
      }
    }

    block prelude = gen_noop();
    if (curr->op == CALL_JQ) {
      int actual_args = 0, desired_args = 0;
      // We expand the argument list as a series of instructions
      switch (curr->bound_by->op) {
      default: assert(0 && "Unknown function type"); break;
      case CLOSURE_CREATE:
      case CLOSURE_PARAM: {
        block callargs = gen_noop();
        for (inst* i; (i = block_take(&curr->arglist));) {
          assert(opcode_describe(i->op)->flags & OP_IS_CALL_PSEUDO);
          block b = inst_block(i);
          switch (i->op) {
          default: assert(0 && "Unknown type of parameter"); break;
          case CLOSURE_REF:
            block_append(&callargs, b);
            break;
          case CLOSURE_CREATE:
            block_append(&prelude, b);
            block_append(&callargs, gen_op_bound(CLOSURE_REF, b));
            break;
          }
          actual_args++;
        }
        curr->imm.intval = actual_args;
        curr->arglist = callargs;

        if (curr->bound_by->op == CLOSURE_CREATE) {
          for (inst* i = curr->bound_by->arglist.first; i; i = i->next) {
            assert(i->op == CLOSURE_PARAM);
            desired_args++;
          }
        }
        break;
      }

      case CLOSURE_CREATE_C: {
        for (inst* i; (i = block_take(&curr->arglist)); ) {
          assert(i->op == CLOSURE_CREATE); // FIXME
          block body = i->subfn;
          i->subfn = gen_noop();
          inst_free(i);
          // arguments should be pushed in reverse order, prepend them to prelude
          errors += expand_call_arglist(&body);
          prelude = BLOCK(gen_subexp(body), prelude);
          actual_args++;
        }
        assert(curr->op == CALL_JQ);
        curr->op = CALL_BUILTIN;
        curr->imm.intval = actual_args + 1 /* include the implicit input in arg count */;
        assert(curr->bound_by->op == CLOSURE_CREATE_C);
        desired_args = curr->bound_by->imm.cfunc->nargs - 1;
        assert(!curr->arglist.first);
        break;
      }
      }

      assert(actual_args == desired_args); // because now handle this above
    }
    ret = BLOCK(ret, prelude, inst_block(curr));
  }
  *b = ret;
  return errors;
}

static int compile(struct bytecode* bc, block b, struct locfile* lf) {
  int errors = 0;
  int pos = 0;
  int var_frame_idx = 0;
  bc->nsubfunctions = 0;
  errors += expand_call_arglist(&b);
  b = BLOCK(b, gen_op_simple(RET));
  jv localnames = jv_array();
  for (inst* curr = b.first; curr; curr = curr->next) {
    if (!curr->next) assert(curr == b.last);
    int length = opcode_describe(curr->op)->length;
    if (curr->op == CALL_JQ) {
      for (inst* arg = curr->arglist.first; arg; arg = arg->next) {
        length += 2;
      }
    }
    pos += length;
    curr->bytecode_pos = pos;
    curr->compiled = bc;

    assert(curr->op != CLOSURE_REF && curr->op != CLOSURE_PARAM);

    if ((opcode_describe(curr->op)->flags & OP_HAS_VARIABLE) &&
        curr->bound_by == curr) {
      curr->imm.intval = var_frame_idx++;
      localnames = jv_array_append(localnames, jv_string(curr->symbol));
    }

    if (curr->op == CLOSURE_CREATE) {
      assert(curr->bound_by == curr);
      curr->imm.intval = bc->nsubfunctions++;
    }
    if (curr->op == CLOSURE_CREATE_C) {
      assert(curr->bound_by == curr);
      int idx = bc->globals->ncfunctions++;
      bc->globals->cfunc_names = jv_array_append(bc->globals->cfunc_names,
                                                 jv_string(curr->symbol));
      bc->globals->cfunctions[idx] = *curr->imm.cfunc;
      curr->imm.intval = idx;
    }
  }
  if (pos > 0xFFFF) {
    // too long for program counter to fit in uint16_t
    locfile_locate(lf, UNKNOWN_LOCATION,
        "function compiled to %d bytes which is too long", pos);
    errors++;
  }
  bc->codelen = pos;
  bc->debuginfo = jv_object_set(bc->debuginfo, jv_string("locals"), localnames);
  if (bc->nsubfunctions) {
    bc->subfunctions = jv_mem_alloc(sizeof(struct bytecode*) * bc->nsubfunctions);
    for (inst* curr = b.first; curr; curr = curr->next) {
      if (curr->op == CLOSURE_CREATE) {
        struct bytecode* subfn = jv_mem_alloc(sizeof(struct bytecode));
        bc->subfunctions[curr->imm.intval] = subfn;
        subfn->globals = bc->globals;
        subfn->parent = bc;
        subfn->nclosures = 0;
        subfn->debuginfo = jv_object_set(jv_object(), jv_string("name"), jv_string(curr->symbol));
        jv params = jv_array();
        for (inst* param = curr->arglist.first; param; param = param->next) {
          assert(param->op == CLOSURE_PARAM);
          assert(param->bound_by == param);
          param->imm.intval = subfn->nclosures++;
          param->compiled = subfn;
          params = jv_array_append(params, jv_string(param->symbol));
        }
        subfn->debuginfo = jv_object_set(subfn->debuginfo, jv_string("params"), params);
        errors += compile(subfn, curr->subfn, lf);
        curr->subfn = gen_noop();
      }
    }
  } else {
    bc->subfunctions = 0;
  }
  uint16_t* code = jv_mem_alloc(sizeof(uint16_t) * bc->codelen);
  bc->code = code;
  pos = 0;
  jv constant_pool = jv_array();
  int maxvar = -1;
  if (!errors) for (inst* curr = b.first; curr; curr = curr->next) {
    const struct opcode_description* op = opcode_describe(curr->op);
    int op_pos = pos;
    if (op->length == 0)
      continue;
    code[pos++] = curr->op;
    assert(curr->op != CLOSURE_REF && curr->op != CLOSURE_PARAM);
    if (curr->op == CALL_BUILTIN) {
      assert(curr->bound_by->op == CLOSURE_CREATE_C);
      assert(!curr->arglist.first);
      code[pos++] = (uint16_t)curr->imm.intval;
      code[pos++] = curr->bound_by->imm.intval;
      if (curr->bound_by->imm.cfunc->is_generator)
        code[op_pos] = CALL_BUILTIN_GENERATOR;
    } else if (curr->op == CALL_JQ) {
      assert(curr->bound_by->op == CLOSURE_CREATE ||
             curr->bound_by->op == CLOSURE_PARAM);
      code[pos++] = (uint16_t)curr->imm.intval;
      code[pos++] = nesting_level(bc, curr->bound_by);
      code[pos++] = curr->bound_by->imm.intval |
        (curr->bound_by->op == CLOSURE_CREATE ? ARG_NEWCLOSURE : 0);
      for (inst* arg = curr->arglist.first; arg; arg = arg->next) {
        assert(arg->op == CLOSURE_REF && arg->bound_by->op == CLOSURE_CREATE);
        code[pos++] = nesting_level(bc, arg->bound_by);
        code[pos++] = arg->bound_by->imm.intval | ARG_NEWCLOSURE;
      }
    } else if ((op->flags & OP_HAS_CONSTANT) && (op->flags & OP_HAS_VARIABLE)) {
      // STORE_GLOBAL: constant global, basically
      code[pos++] = jv_array_length(jv_copy(constant_pool));
      constant_pool = jv_array_append(constant_pool, jv_copy(curr->imm.constant));
      code[pos++] = nesting_level(bc, curr->bound_by);
      uint16_t var = (uint16_t)curr->bound_by->imm.intval;
      code[pos++] = var;
    } else if (op->flags & OP_HAS_CONSTANT) {
      code[pos++] = jv_array_length(jv_copy(constant_pool));
      constant_pool = jv_array_append(constant_pool, jv_copy(curr->imm.constant));
    } else if (op->flags & OP_HAS_VARIABLE) {
      code[pos++] = nesting_level(bc, curr->bound_by);
      uint16_t var = (uint16_t)curr->bound_by->imm.intval;
      code[pos++] = var;
      if (var > maxvar) maxvar = var;
    } else if (op->flags & OP_HAS_BRANCH) {
      assert(curr->imm.target->bytecode_pos != -1);
      assert(curr->imm.target->bytecode_pos > pos); // only forward branches
      code[pos] = curr->imm.target->bytecode_pos - (pos + 1);
      pos++;
    } else if (op->length > 1) {
      assert(0 && "codegen not implemented for this operation");
    }
  }
  bc->constants = constant_pool;
  bc->nlocals = maxvar + 2; // FIXME: frames of size zero?
  block_free(b);
  return errors;
}

int block_compile(block b, struct bytecode** out, struct locfile* lf) {
  struct bytecode* bc = jv_mem_alloc(sizeof(struct bytecode));
  bc->parent = 0;
  bc->nclosures = 0;
  bc->globals = jv_mem_alloc(sizeof(struct symbol_table));
  int ncfunc = count_cfunctions(b);
  bc->globals->ncfunctions = 0;
  bc->globals->cfunctions = jv_mem_alloc(sizeof(struct cfunction) * ncfunc);
  bc->globals->cfunc_names = jv_array();
  bc->debuginfo = jv_object_set(jv_object(), jv_string("name"), jv_null());
  int nerrors = compile(bc, b, lf);
  assert(bc->globals->ncfunctions == ncfunc);
  if (nerrors > 0) {
    bytecode_free(bc);
    *out = 0;
  } else {
    *out = bc;
  }
  return nerrors;
}

void block_free(block b) {
  struct inst* next;
  for (struct inst* curr = b.first; curr; curr = next) {
    next = curr->next;
    inst_free(curr);
  }
}
