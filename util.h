#ifndef UTIL_H
#define UTIL_H

#include "jv.h"

jv build_lib_search_chain(jv lib_paths);
jv find_lib(jv lib_search_paths, jv lib_name, jv origin);
jv compile_bind_lib(jq_state *jq, block* bb, const char* lib);

#endif /* UTIL_H */
