#ifndef BUILTIN_H
#define BUILTIN_H

#include "jq.h"
#include "bytecode.h"
#include "compile.h"

int builtins_bind(jq_state *, block*);

struct builtin_module_desc {
  const char *name;
  const char *source;
};

extern struct builtin_module_desc builtin_modules[];
const char *builtin_module(jv);

#endif
