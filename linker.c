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

// This should be an array of structs, not a struct of arrays.
struct lib_loading_state {
  char **names;
  block *defs;
  uint64_t ct;
};

struct lib {
  jv name;
  jv metadata;
  block blocks;
  uint64_t ct;
};

struct loaded_libs {
  struct lib *libs; // Array
  jv indices;       // Object; values are indices into libs[]
  int nlibs;
};

static int load_library(jq_state *jq, jv lib_path, block *out_block, struct lib_loading_state *lib_state);

// Given a lib_path to search first, creates a chain of search paths
// in the following order:
// 1. lib_path
// 2. -L paths passed in on the command line (from jq_state*)
// 3. JQ_LIBRARY_PATH environment variable
jv build_lib_search_chain(jq_state *jq, jv lib_path) {
  assert(jv_get_kind(lib_path) == JV_KIND_STRING);

  jv out_paths = jv_array();
  if (jv_string_length_bytes(jv_copy(lib_path)))
    out_paths = jv_array_append(out_paths, lib_path);
  else
    jv_free(lib_path);
  jv lib_dirs = jq_get_lib_dirs(jq);
  jv_array_foreach(lib_dirs, i, path) {
    if (jv_string_length_bytes(jv_copy(path)) == 0)  {
      jv_free(path);
      continue;
    }
    path = expand_path(path);
    if (jv_is_valid(path)) {
      out_paths = jv_array_append(out_paths, path);
    } else {
      jv emsg = jv_invalid_get_msg(path);
      jq_report_error(jq, jv_string_fmt("jq: warning: skipping search path: %s\n",jv_string_value(emsg)));
      jv_free(emsg);
    } 
  }
  jv_free(lib_dirs);
  return out_paths;
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
  assert(jv_get_kind(lib_search_path) == JV_KIND_STRING);
  assert(jv_get_kind(lib_name) == JV_KIND_STRING);

  jv rel_path = name2relpath(jv_copy(lib_name));
  if (!jv_is_valid(rel_path)) {
    jv_free(lib_name);
    return rel_path;
  }

  struct stat st;
  int ret;

  jv lib_search_paths = build_lib_search_chain(jq, expand_path(lib_search_path));

  jv_array_foreach(lib_search_paths, i, spath) {
    jv testpath = jq_realpath(jv_string_fmt("%s/%s.jq",
                                            jv_string_value(spath),
                                            jv_string_value(rel_path)));
    if (!jv_is_valid(testpath)) {
      jv_free(testpath);
      testpath = jq_realpath(jv_string_fmt("%s/%s/%s.jq",
                                           jv_string_value(spath),
                                           jv_string_value(rel_path),
                                           jv_string_value(lib_name)));
    }
    if (jv_is_valid(testpath)) {
      ret = stat(jv_string_value(testpath),&st);
      if (ret == 0) {
        jv_free(spath);
        jv_free(rel_path);
        jv_free(lib_name);
        jv_free(lib_search_paths);
        return testpath;
      }
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

static int semver_match_2obj(jv impv, jv modv) {
  jv_kind impv_k = jv_get_kind(impv);
  jv_kind modv_k = jv_get_kind(modv);
  int impv_is_obj = (impv_k == JV_KIND_OBJECT);
  int modv_is_obj = (modv_k == JV_KIND_OBJECT);
  assert((impv_is_obj && !modv_is_obj) || (!impv_is_obj && modv_is_obj));
  
  jv o = (impv_is_obj ? impv : modv);
  jv x = (impv_is_obj ? modv : impv);

  if (jv_object_contains(jv_copy(o), jv_string("hash"))) {
    if (!jv_equal(jv_object_get(jv_copy(o), jv_string("hash")), x)) {
      jv_free(o);
      jv_free(x);
      return 0;
    }
  }
  if (jv_object_contains(jv_copy(o), jv_string("commit"))) {
    if (!jv_equal(jv_object_get(jv_copy(o), jv_string("commit")), x)) {
      jv_free(o);
      jv_free(x);
      return 0;
    }
  }
  if (jv_get_kind(x) != JV_KIND_NUMBER) {
    jv_free(o);
    jv_free(x);
    return 0;
  }
  if (impv_is_obj && jv_object_contains(jv_copy(impv), jv_string("major"))) {
    jv_free(o);
    jv_free(x);
    return 0;
  }
  if (jv_get_kind(x) != JV_KIND_NUMBER) {
    jv_free(o);
    jv_free(x);
    return 0;
  }
  double ovnum = 0.0;
  if (jv_object_contains(jv_copy(o), jv_string("minor"))) {
    jv n = jv_object_get(jv_copy(o), jv_string("minor"));
    ovnum = jv_number_value(n);
    jv_free(n);
  }
  if ((impv_is_obj && ovnum > jv_number_value(x)) ||
      (!impv_is_obj && ovnum < jv_number_value(x))) {
    jv_free(o);
    jv_free(x);
    return 0;
  }
  if (!jv_object_contains(jv_copy(o), jv_string("micro")))
    return 1;
  jv n = jv_object_get(jv_copy(o), jv_string("micro"));
  double micro = jv_number_value(n);
  jv_free(n);
  while (micro > 1)
    micro /= 10;
  ovnum += micro;
  jv_free(o);
  jv_free(x);
  if ((impv_is_obj && ovnum > jv_number_value(x)) ||
      (!impv_is_obj && ovnum < jv_number_value(x)))
    return 0;
  return 1;
}

static int semver_match_obj2obj(jv impv, jv modv) {
  // First check assertion about hash/commit
  if (jv_object_contains(jv_copy(impv), jv_string("hash"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("hash"))) {
      jv_free(impv);
      jv_free(modv);
      return 0;
    }
    if (!jv_equal(jv_object_get(jv_copy(impv), jv_string("hash")),
                  jv_object_get(jv_copy(modv), jv_string("hash")))) {
      jv_free(impv);
      jv_free(modv);
      return 0;
    }
  }
  // XXX DRY
  if (jv_object_contains(jv_copy(impv), jv_string("commit"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("commit"))) {
      jv_free(impv);
      jv_free(modv);
      return 0;
    }
    if (!jv_equal(jv_object_get(jv_copy(impv), jv_string("commit")),
                  jv_object_get(jv_copy(modv), jv_string("commit")))) {
      jv_free(impv);
      jv_free(modv);
      return 0;
    }
  }

  // Now check version numbering
  if (jv_object_contains(jv_copy(impv), jv_string("major"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("major")) ||
        !jv_equal(jv_object_get(jv_copy(impv), jv_string("major")),
                  jv_object_get(jv_copy(modv), jv_string("major")))) {
      jv_free(impv);
      jv_free(modv);
      return 0;
    }
  }
  if (jv_object_contains(jv_copy(impv), jv_string("minor"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("minor")))
      return 0;
    jv imin = jv_object_get(jv_copy(impv), jv_string("minor"));
    jv mmin = jv_object_get(jv_copy(modv), jv_string("minor"));
    if (jv_number_value(imin) != jv_number_value(mmin)) {
      int res = (jv_number_value(mmin) >= jv_number_value(imin));
      jv_free(imin);
      jv_free(mmin);
      jv_free(impv);
      jv_free(modv);
      return res;
    }
  }
  if (jv_object_contains(jv_copy(impv), jv_string("micro"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("micro")))
      return 0;
    jv imin = jv_object_get(jv_copy(impv), jv_string("micro"));
    jv mmin = jv_object_get(jv_copy(modv), jv_string("micro"));
    int res = (jv_number_value(mmin) >= jv_number_value(imin));
    jv_free(imin);
    jv_free(mmin);
    jv_free(impv);
    jv_free(modv);
    return res;
  }
  // Note that we allow empty version objects to match!
  return 1;
}

static int semver_match_pair(jv impv, jv modv) {
  if (jv_equal(jv_copy(impv), jv_copy(modv))) {
    jv_free(impv);
    jv_free(modv);
    return 1;
  }
  if (jv_get_kind(impv) == JV_KIND_NUMBER) {
    if (jv_get_kind(modv) != JV_KIND_NUMBER) {
      // XXX If modv is an object with no major version number but with
      // a minor and (maybe) a micro version numbers, then we should be
      // able to form a number to compare to impv.
      return semver_match_2obj(impv, modv);
      return 0;
    }
    int res = jv_number_value(impv) <= jv_number_value(modv);
    jv_free(impv);
    jv_free(modv);
    return res;
  }
  if (jv_get_kind(impv) != JV_KIND_OBJECT)
    return 0;
  if (jv_get_kind(impv) == JV_KIND_OBJECT && jv_object_contains(jv_copy(impv), jv_string("version")))
    return semver_match_pair(jv_object_get(impv, jv_string("version")), modv);
  // Here impv is an object that may have "major", "minor", and/or
  // "micro" keys.  modv still could be anything at all.

  if (jv_get_kind(modv) == JV_KIND_OBJECT && jv_object_contains(jv_copy(modv), jv_string("version")))
    modv = jv_object_get(modv, jv_string("version"));
  if (jv_get_kind(modv) != JV_KIND_OBJECT)
    return semver_match_2obj(impv, modv);
  // Here both are objects that may have version keys.
  return semver_match_obj2obj(impv, modv);
}

static int semver(jq_state *jq, jv importer, jv candidate) {
  if (jv_get_kind(importer) != JV_KIND_ARRAY)
    importer = JV_ARRAY(importer);
  if (jv_get_kind(candidate) != JV_KIND_ARRAY)
    candidate = JV_ARRAY(candidate);
  int match = 0;
  jv_array_foreach(importer, i, impv) {
    jv_array_foreach(candidate, k, modv) {
      if (semver_match_pair(impv, modv)) {
        match = 1;
        break;
      }
    }
  }
  jv_free(importer);
  jv_free(candidate);
  return match;
}

static int process_dependencies(jq_state *jq, jv lib_origin, block *src_block, struct lib_loading_state *lib_state) {
  jv deps = block_take_imports(src_block);
  block bk = *src_block;
  int nerrors = 0;

  jv_array_foreach(deps, i, dep) {
    jv name = jv_object_get(jv_copy(dep), jv_string("name"));
    jv as = jv_object_get(jv_copy(dep), jv_string("as"));
    if (!jv_is_valid(as)) {
      jv_free(as);
      as = jv_string("");
    }
    jv search = jv_object_get(dep, jv_string("search"));
    if (!jv_is_valid(search)) {
      jv_free(search);
      search = jv_string("");
    }
    if (strncmp("$ORIGIN/",jv_string_value(search), sizeof ("$ORIGIN/") - 1) == 0) {
      jv tsearch = jv_string_fmt("%s/%s",
                                 jv_string_value(lib_origin),
                                 jv_string_value(search) + sizeof ("$ORIGIN/") - 1);
      jv_free(search);
      search = tsearch;
    } else if (strncmp("$JQ_ORIGIN/",jv_string_value(search), sizeof ("$JQ_ORIGIN/") - 1) == 0) {
      jv tsearch = jv_string_fmt("%s/%s",
                                 jv_string_value(jq_get_attr(jq, jv_string("JQ_ORIGIN"))),
                                 jv_string_value(search) + sizeof ("$JQ_ORIGIN/") - 1);
      jv_free(search);
      search = tsearch;
    }
    jv lib_path = find_lib(jq, name, search);
    if (!jv_is_valid(lib_path)) {
      jv emsg = jv_invalid_get_msg(lib_path);
      jq_report_error(jq, jv_string_fmt("jq: error: %s\n",jv_string_value(emsg)));
      jv_free(emsg);
      jv_free(lib_origin);
      jv_free(as);
      jv_free(deps);
      return 1;
    }
    uint64_t state_idx = 0;
    for (; state_idx < lib_state->ct; ++state_idx) {
      if (strcmp(lib_state->names[state_idx],jv_string_value(lib_path)) == 0)
        break;
    }
    if (state_idx < lib_state->ct) { // Found
      // XXX Check version matching here!
      // XXX We've lost the library's MODULEMETA by this point!  Save it!
      if (version_matches(jq, bk, lib_state->defs[state_idx]))
        bk = block_bind_library(lib_state->defs[state_idx], bk, OP_IS_CALL_PSEUDO, jv_string_value(as));
      else
        // XXX Would be nice to have the dependent's name here too
        jq_report_error(jq, jv_string_fmt("jq: error: version mismatch for %s", jv_string_value(name)));
      jv_free(lib_path);
    } else { // Not found.   Add it to the table before binding.
      block dep_def_block = gen_noop();
      nerrors += load_library(jq, lib_path, &dep_def_block, lib_state);
      if (nerrors == 0) {
        // XXX Check version matching here!
        if (version_matches(jq, bk, dep_def_block))
          bk = block_bind_library(dep_def_block, bk, OP_IS_CALL_PSEUDO, jv_string_value(as));
        else
          jq_report_error(jq, jv_string_fmt("jq: error: version mismatch for %s", jv_string_value(name)));
      }
    }
    jv_free(as);
  }
  jv_free(lib_origin);
  jv_free(deps);
  return nerrors;
}

// Loads the library at lib_path into lib_state, putting the library's defs
// into *out_block
static int load_library(jq_state *jq, jv lib_path, block *out_block, struct lib_loading_state *lib_state) {
  int nerrors = 0;
  struct locfile* src;
  block program;
  jv data = jv_load_file(jv_string_value(lib_path), 1);
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
      nerrors += process_dependencies(jq, jv_string(dirname(lib_origin)), &lib_state->defs[state_idx], lib_state);
      free(lib_origin);
      *out_block = lib_state->defs[state_idx];
    }
    locfile_free(src);
  }
  jv_free(lib_path);
  jv_free(data);
  return nerrors;
}

jv load_module_meta(jq_state *jq, jv modname) {
  jv lib_path = find_lib(jq, modname, jv_string(""));
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

  nerrors = process_dependencies(jq, jq_get_lib_origin(jq), &program, &lib_state);
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
