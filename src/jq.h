#ifndef JQ_H
#define JQ_H

#include <stdio.h>
#include "jv.h"

enum {
  JQ_DEBUG_TRACE = 1,
  JQ_DEBUG_TRACE_DETAIL = 2,
  JQ_DEBUG_TRACE_ALL = JQ_DEBUG_TRACE | JQ_DEBUG_TRACE_DETAIL,
};

typedef enum jq_input_mode_enum {
  // JQ VM reaches a state where it requires the next input
  // when it backtracks to START.
  // At this point the VM can behave differently, and this enum
  // describes the possible actions VM will take.


  // In the RETURN_EMPTY mode jq_next behaves like it always did:
  // it will return a jv_invalid() without any message
  // to show that additional input is needed
  // The caller in this case may choose to restart the VM
  // or set the next value and call jq_next again
  JQ_INPUT_RETURN_EMPTY,

  // In the CALLBACK mode jq_next will issue an input callback
  // in case it will require more input. This will be equivalent
  // to when the jq program reaches the `input` instruction.
  // passing an invalid to the callback will immediately request input again
  // passing an invalid with message will raise this as an error and halt
  JQ_INPUT_CALLBACK,

  // A backward compatible default mode
  JQ_INPUT_DEFAULT = JQ_INPUT_RETURN_EMPTY
} jq_input_mode;


typedef struct jq_state jq_state;
typedef void (*jq_msg_cb)(void *, jv);

jq_state *jq_init(void);
void jq_set_error_cb(jq_state *, jq_msg_cb, void *);
void jq_get_error_cb(jq_state *, jq_msg_cb *, void **);
void jq_set_nomem_handler(jq_state *, void (*)(void *), void *);
jv jq_format_error(jv msg);
void jq_report_error(jq_state *, jv);
int jq_compile(jq_state *, const char*);
int jq_compile_args(jq_state *, const char*, jv);
void jq_dump_disassembly(jq_state *, int);
void jq_start(jq_state *, jv value, int);
jv jq_next(jq_state *);
void jq_teardown(jq_state **);
void jq_set_input_mode(jq_state*, jq_input_mode);
jq_input_mode jq_get_input_mode(jq_state*);

void jq_set_input(jq_state*, jv);
jv jq_get_input_copy(jq_state*);

void jq_halt(jq_state *, jv, jv);
int jq_halted(jq_state *);
int jq_finished(jq_state *);
jv jq_get_exit_code(jq_state *);
jv jq_get_error_message(jq_state *);

typedef jv (*jq_input_cb)(jq_state *, void *);
void jq_set_input_cb(jq_state *, jq_input_cb, void *);
void jq_get_input_cb(jq_state *, jq_input_cb *, void **);

void jq_set_debug_cb(jq_state *, jq_msg_cb, void *);
void jq_get_debug_cb(jq_state *, jq_msg_cb *, void **);

jv jq_get_vmid(jq_state *);

jv jq_io_policy_check(jq_state *, jv);

jv jq_set_io_policy(jq_state *, jv);
void jq_set_attrs(jq_state *, jv);
jv jq_get_attrs(jq_state *);
jv jq_get_jq_origin(jq_state *);
jv jq_get_prog_origin(jq_state *);
jv jq_get_lib_dirs(jq_state *);
void jq_set_attr(jq_state *, jv, jv);
jv jq_get_attr(jq_state *, jv);

/*
 * We use char * instead of jf for filenames here because filenames
 * should be in the process' locale's codeset, which may not be UTF-8,
 * whereas jv string values must be in UTF-8.  This way the caller
 * doesn't have to perform any codeset conversions.
 */
typedef struct jq_util_input_state jq_util_input_state;
typedef void (*jq_util_msg_cb)(void *, const char *);

jq_util_input_state *jq_util_input_init(jq_util_msg_cb, void *);
void jq_util_input_set_parser(jq_util_input_state *, jv_parser *, int);
void jq_util_input_free(jq_util_input_state **);
void jq_util_input_add_input(jq_util_input_state *, const char *);
int jq_util_input_errors(jq_util_input_state *);
jv jq_util_input_next_input(jq_util_input_state *);
jv jq_util_input_next_input_cb(jq_state *, void *);
jv jq_util_input_get_position(jq_state*);
jv jq_util_input_get_current_filename(jq_state*);
jv jq_util_input_get_current_line(jq_state*);

int jq_set_colors(const char *);

#endif /* !JQ_H */
