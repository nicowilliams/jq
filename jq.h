#ifndef _JQ_H_
#define _JQ_H_

#include <stdio.h>
#include <jv.h>

typedef enum jq_runtime_flags {
  JQ_DEBUG_TRACE = 1,   /* trace jq program execution */
  JQ_OPEN_FILES = 2,    /* jq program allowed to open any file */
  JQ_OPEN_WRITE = 4,    /* jq program allowed to open files for writing */
  JQ_EXEC = 4,          /* jq program allowed to popen() */
} jq_runtime_flags;

typedef struct jq_state jq_state;
typedef void (*jq_err_cb)(void *, jv);

jq_state *jq_init(void);
void jq_set_error_cb(jq_state *, jq_err_cb, void *);
void jq_get_error_cb(jq_state *, jq_err_cb *, void **);
void jq_set_nomem_handler(jq_state *, void (*)(void *), void *);
jv jq_format_error(jv msg);
void jq_report_error(jq_state *, jv);
int jq_compile(jq_state *, const char*);
int jq_compile_args(jq_state *, const char*, jv);
void jq_dump_disassembly(jq_state *, int);
void jq_start(jq_state *, jv value, jq_runtime_flags flags);
jv jq_next(jq_state *);
int jq_handle_create(jq_state *, int, const char *, void *, void (*)(void *));
int jq_handle_create_null(jq_state *, int);
int jq_handle_create_stdio(jq_state *, int, FILE *, int, int);
void jq_handle_get(jq_state *, int, const char **, void **, void (**)(void *));
void jq_handle_delete(jq_state *, int);
void jq_teardown(jq_state **);

void jq_set_attrs(jq_state *, jv);
jv jq_get_attrs(jq_state *);
jv jq_get_jq_origin(jq_state *);
jv jq_get_prog_origin(jq_state *);
jv jq_get_lib_dirs(jq_state *);
void jq_set_attr(jq_state *, jv, jv);
jv jq_get_attr(jq_state *, jv);
#endif /* !_JQ_H_ */
