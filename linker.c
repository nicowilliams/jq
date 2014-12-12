#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <libgen.h>

#include "jq_parser.h"
#include "locfile.h"
#include "jv.h"
#include "jq.h"
#include "parser.h"
#include "util.h"
#include "compile.h"
#include "builtin.h"

struct lib_loading_state {
  char **names;
  block *defs;
  uint64_t ct;
};
static int load_library(jq_state *jq, jv lib_path, jv dependency_info, block *out_block, struct lib_loading_state *lib_state);

// Given a lib_path to search first, creates a chain of search paths
// in the following order:
// 1. lib_path
// 2. -L paths passed in on the command line (from jq_state*)
// 3. JQ_LIBRARY_PATH environment variable
jv build_lib_search_chain(jq_state *jq, jv search_path) {
  assert(jv_get_kind(search_path) == JV_KIND_ARRAY);
  jv expanded = jv_array();
  jv_array_foreach(search_path, i, path) {
    path = expand_path(path);
    if (jv_is_valid(path))
      expanded = jv_array_append(expanded, expand_path(path));
    else
      jv_free(path);
  }
  jv_free(search_path);
  expanded = jv_array_concat(expanded, jq_get_lib_dirs(jq));
  return expanded;
}

static jv name2relpath(jv name) {
  jv components = jv_string_split(jv_copy(name), jv_string("::"));
  jv rp = jv_array_get(jv_copy(components), 0);
  components = jv_array_slice(components, 1, jv_array_length(jv_copy(components)));
  jv_array_foreach(components, i, x) {
    if (i > 0 && jv_equal(jv_copy(x), jv_array_get(jv_copy(components), i - 1))) {
      jv_free(x);
      jv_free(rp);
      jv_free(components);
      jv res = jv_invalid_with_msg(jv_string_fmt("module names must not have equal consecutive components: %s",
                                                 jv_string_value(name)));
      jv_free(name);
      return res;
    }
    rp = jv_string_concat(rp, jv_string_concat(jv_string("/"), x));
  }
  jv_free(components);
  jv_free(name);
  return rp;
}

static jv find_lib(jq_state *jq, jv lib_name, jv lib_search_path) {
  assert(jv_get_kind(lib_search_path) == JV_KIND_ARRAY);
  assert(jv_get_kind(lib_name) == JV_KIND_STRING);

  jv rel_path = name2relpath(jv_copy(lib_name));
  if (!jv_is_valid(rel_path)) {
    jv_free(lib_name);
    return rel_path;
  }

  struct stat st;
  int ret;

  jv lib_search_paths = build_lib_search_chain(jq, lib_search_path);

  jv_array_foreach(lib_search_paths, i, spath) {
    if (jv_get_kind(spath) == JV_KIND_NULL) {
      jv_free(spath);
      break;
    }
    if (jv_get_kind(spath) != JV_KIND_STRING ||
        strcmp(jv_string_value(spath), "") == 0) {
      jv_free(spath);
      continue; /* XXX report non-strings in search path?? */
    }
    jv testpath = jq_realpath(jv_string_fmt("%s/%s.jq",
                                            jv_string_value(spath),
                                            jv_string_value(rel_path)));
    ret = stat(jv_string_value(testpath),&st);
    if (ret == -1 && errno == ENOENT) {
      jv_free(testpath);
      testpath = jq_realpath(jv_string_fmt("%s/%s/%s.jq",
                                           jv_string_value(spath),
                                           jv_string_value(rel_path),
                                           jv_string_value(lib_name)));
      ret = stat(jv_string_value(testpath),&st);
    }
    if (ret == 0) {
      jv_free(spath);
      jv_free(rel_path);
      jv_free(lib_name);
      jv_free(lib_search_paths);
      return testpath;
    }
    jv_free(testpath);
    jv_free(spath);
  }
  jv output = jv_invalid_with_msg(jv_string_fmt("module not found: %s", jv_string_value(lib_name)));
  jv_free(rel_path);
  jv_free(lib_name);
  jv_free(lib_search_paths);
  return output;
}

static int version_matches(jq_state *jq, block importer, block module) {
  return 1;
}

static jv default_search(jv value) {
  if (!jv_is_valid(value)) {
    jv_free(value);
    return JV_ARRAY(jv_string("."), jv_string("$ORIGIN"));
  }
  if (jv_get_kind(value) != JV_KIND_ARRAY)
    return JV_ARRAY(value);
  return value;
}

// XXX Split this into a util that takes a callback, and then...
static int process_dependencies(jq_state *jq,
                                jv jq_origin,
                                jv lib_origin,
                                block *src_block,
                                struct lib_loading_state *lib_state) {
  jv deps = block_take_imports(src_block);
  block bk = *src_block;
  int nerrors = 0;

  // Act as though the program starts with import jq;
  if (block_has_main(*src_block))
    deps = jv_array_append(deps, JV_OBJECT(jv_string("name"), jv_string("jq")));

  jv_array_foreach(deps, i, dep) {
    jv name = jv_object_get(jv_copy(dep), jv_string("name"));
    jv as = jv_object_get(jv_copy(dep), jv_string("as"));
    if (!jv_is_valid(as)) {
      jv_free(as);
      as = jv_string("");
    }
    jv lib_path;
    if (strcmp(jv_string_value(name), "jq") != 0) {
      jv search = default_search(jv_object_get(dep, jv_string("search")));
      jv_array_foreach(search, k, search_elt) {
        if (strcmp(".",jv_string_value(search_elt)) == 0) {
          jv tsearch = jv_copy(lib_origin);
          jv_free(search_elt);
          search = jv_array_set(search, k, tsearch);
        } else if (strncmp("./",jv_string_value(search_elt),sizeof("./")-1) == 0) {
          jv tsearch = jv_string_fmt("%s/%s",
                                     jv_string_value(lib_origin),
                                     jv_string_value(search_elt) + sizeof ("./") - 1);
          jv_free(search_elt);
          search = jv_array_set(search, k, tsearch);
        } else if (strncmp("$ORIGIN/",jv_string_value(search_elt),sizeof("$ORIGIN/")-1) == 0) {
          jv tsearch = jv_string_fmt("%s/%s",
                                     jv_string_value(jq_origin),
                                     jv_string_value(search_elt) + sizeof ("$ORIGIN/") - 1);
          jv_free(search_elt);
          search = jv_array_set(search, k, tsearch);
        } else {
          jv_free(search_elt);
        }
      }
      lib_path = find_lib(jq, jv_copy(name), search);
    } else {
      lib_path = jv_copy(name);
    }
    // XXX ...move the rest of this into a callback.
    if (!jv_is_valid(lib_path)) {
      jv emsg = jv_invalid_get_msg(lib_path);
      jq_report_error(jq, jv_string_fmt("jq: error: %s\n",jv_string_value(emsg)));
      jv_free(emsg);
      jv_free(as);
      jv_free(deps);
      jv_free(jq_origin);
      jv_free(lib_origin);
      jv_free(name);
      return 1;
    }
    uint64_t state_idx = 0;
    for (; state_idx < lib_state->ct; ++state_idx) {
      if (strcmp(lib_state->names[state_idx],jv_string_value(lib_path)) == 0)
        break;
    }
    if (state_idx < lib_state->ct) { // Found
      // XXX Check version matching here!
      if (version_matches(jq, bk, lib_state->defs[state_idx]))
        bk = block_bind_library(lib_state->defs[state_idx], bk, OP_IS_CALL_PSEUDO, jv_string_value(as));
      else
        // XXX Would be nice to have the dependent's name here too
        jq_report_error(jq, jv_string_fmt("jq: error: version mismatch for %s", jv_string_value(name)));
      jv_free(lib_path);
    } else { // Not found.   Add it to the table before binding.
      block dep_def_block = gen_noop();
      nerrors += load_library(jq, lib_path, jv_copy(dep), &dep_def_block, lib_state);
      if (nerrors == 0) {
        // XXX Check version matching here!
        if (version_matches(jq, bk, dep_def_block)) {
          bk = block_bind_library(dep_def_block, bk, OP_IS_CALL_PSEUDO, jv_string_value(as));
          if (strcmp(jv_string_value(name), "jq") != 0) {
            builtins_bind(jq, &bk);
          }
        } else {
          jq_report_error(jq, jv_string_fmt("jq: error: version mismatch for %s", jv_string_value(name)));
        }
      }
    }
    jv_free(as);
  }
  *src_block = bk;
  jv_free(lib_origin);
  jv_free(jq_origin);
  jv_free(deps);
  return nerrors;
}

// Loads the library at lib_path into lib_state, putting the library's defs
// into *out_block
static int load_library(jq_state *jq, jv lib_path, jv dependency_info, block *out_block, struct lib_loading_state *lib_state) {
  int nerrors = 0;
  struct locfile* src;
  block program;
  jv data;
  
  if (strcmp(jv_string_value(lib_path), "jq") == 0) {
    data = jq_builtins(jv_object_get(dependency_info, jv_string("version")));
  } else {
    data = jv_load_file(jv_string_value(lib_path), 1);
    jv_free(dependency_info);
  }

  int state_idx;
  if (jv_is_valid(data)) {
    src = locfile_init(jq, jv_string_value(data), jv_string_length_bytes(jv_copy(data)));
    nerrors += jq_parse_library(src, &program);
    if (nerrors == 0) {
      state_idx = lib_state->ct++;
      lib_state->names = realloc(lib_state->names, lib_state->ct * sizeof(const char *));
      lib_state->defs = realloc(lib_state->defs, lib_state->ct * sizeof(block));
      lib_state->names[state_idx] = strdup(jv_string_value(lib_path));
      lib_state->defs[state_idx] = program;

      char *lib_origin = strdup(jv_string_value(lib_path));
      nerrors += process_dependencies(jq, jq_get_jq_origin(jq),
                                      jv_string(dirname(lib_origin)),
                                      &lib_state->defs[state_idx], lib_state);
      free(lib_origin);
      *out_block = lib_state->defs[state_idx];
    }
    locfile_free(src);
  }
  jv_free(lib_path);
  jv_free(data);
  return nerrors;
}

// FIXME It'd be nice to have an option to search the same search path
// as we do in process_dependencies.
jv load_module_meta(jq_state *jq, jv modname) {
  jv lib_path = find_lib(jq, modname, jv_array());
  jv meta = jv_null();
  jv data = jv_load_file(jv_string_value(lib_path), 1);
  if (jv_is_valid(data)) {
    block program;
    struct locfile* src = locfile_init(jq, jv_string_value(data), jv_string_length_bytes(jv_copy(data)));
    int nerrors = jq_parse_library(src, &program);
    if (nerrors == 0) {
      meta = block_module_meta(program);
      if (jv_get_kind(meta) == JV_KIND_NULL)
        meta = jv_object();
      meta = jv_object_set(meta, jv_string("deps"), block_take_imports(&program));
    }
    locfile_free(src);
    block_free(program);
  }
  jv_free(lib_path);
  jv_free(data);
  return meta;
}

int load_program(jq_state *jq, struct locfile* src, block *out_block) {
  int nerrors = 0;
  block program;
  struct lib_loading_state lib_state = {0,0,0};
  nerrors = jq_parse(src, &program);
  if (nerrors)
    return nerrors;

  nerrors = process_dependencies(jq, jq_get_jq_origin(jq), jq_get_prog_origin(jq), &program, &lib_state);
  block libs = gen_noop();
  for (uint64_t i = 0; i < lib_state.ct; ++i) {
    free(lib_state.names[i]);
    if (nerrors == 0)
      libs = block_join(libs, lib_state.defs[i]);
    else
      block_free(lib_state.defs[i]);
  }
  free(lib_state.names);
  free(lib_state.defs);
  if (nerrors)
    block_free(program);
  else
    *out_block = block_drop_unreferenced(block_join(libs, program));

  return nerrors;
}
