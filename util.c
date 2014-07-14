
#include <assert.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jv.h"
#include "jq.h"
#include "jq_parser.h"
#include "compile.h"
#include "util.h"

static jv get_home() {
  char *home = getenv("HOME");
  if (home)
      return jv_string(home);

#ifndef WIN32
  struct passwd* pwd = getpwuid(getuid());
  if (pwd) 
    return jv_string(pwd->pw_dir);
  return jv_invalid_with_msg(jv_string("Could not find home directory."));
#else
  home = getenv("USERPROFILE");
  if (home)
    return ret = jv_string(home);
  char *hd = getenv("HOMEDRIVE");
  if (!hd) hd = "";
  home = getenv("HOMEPATH");
  if (home)
    return jv_string_fmt("%s%s", hd, home);
  return jv_invalid_with_msg(jv_string("Could not find home directory."));
#endif
}

// This isn't -technically- canonicalization, since I don't
// think it's necessary.  But that's the best name I had.
static jv expand_path(jv path, jv origin) {
  assert(jv_get_kind(path) == JV_KIND_STRING);
  const char *pstr = jv_string_value(path);
#ifdef WIN32
  char sep = '\\';
#else
  char sep = '/';
#endif
  jv ret;
  if (strcmp(pstr, "$ORIGIN/" /* not sep */) == 0) {
    if (jv_get_kind(origin) != JV_KIND_STRING)
      ret = jv_invalid_with_msg(jv_string("Could not expand $ORIGIN/ token; no origin given"));
    else
      ret = jv_string_fmt("%s/%s", jv_string_value(origin), &pstr[sizeof("$ORIGIN")]);
    jv_free(origin);
    jv_free(path);
    return ret;
  }
  if (pstr[0] == '~' && pstr[1] == '/' /* not sep */) {
    jv home = get_home();
    if (!jv_is_valid(home)) {
      jv emsg = jv_invalid_get_msg(home);
      ret = jv_invalid_with_msg(jv_string_fmt("Could not expand %s. (%s)", pstr, jv_string_value(emsg)));
      jv_free(emsg);
      return ret;
    }
    ret = jv_string_fmt("%s%c%s", jv_string_value(home), sep, pstr+2);
    jv_free(home);
    jv_free(path);
    return ret;
  }
  return path;
}

jv build_lib_search_chain(jv lib_paths) {
  assert(jv_get_kind(lib_paths) == JV_KIND_ARRAY);
  char *penv = getenv("JQ_LIBRARY_PATH");
  if (!penv) penv = "";

  lib_paths = jv_array_concat(lib_paths, jv_string_split(jv_string(penv),jv_string(":")));
  jv out_paths = jv_array();
  jv_array_foreach(lib_paths, i, path) {
    if (jv_string_length_bytes(jv_copy(path)) == 0)  {
      jv_free(path);
      continue;
    }
    // XXX Maybe we should get an origin passed in from main() down to
    // here.  Then jq(1) itself can be relocatable.
    path = expand_path(path, jv_null());
    if (jv_is_valid(path)) {
      out_paths = jv_array_append(out_paths, path);
    } else {
      jv_free(path);
      //jv emsg = jv_invalid_get_msg(path);
      //fprintf(stderr, "jq: warning: skipping library path %s\n", jv_string_value(emsg));
      //jv_free(emsg);
    } 
  }
  jv_free(lib_paths);
  return out_paths;
}

jv find_lib(jv lib_search_paths, jv lib_name, jv origin) {
  assert(jv_get_kind(lib_search_paths) == JV_KIND_ARRAY);
  assert(jv_get_kind(lib_name) == JV_KIND_STRING);

#ifdef WIN32
  char sep = '\\';
#else
  char sep = '/';
#endif

  // Check for explicit paths
  // Since all of the methods of specifying an explicit path contain a '/',
  // ("~/some/path", "/some/path", "some/path", "./some/path"), it suffices
  // to simply check for the existence of '/', especially since it must not
  // exist in filenames.
  const char *path = jv_string_value(lib_name);

#ifdef WIN32
  if (path[0] && path[1] == ':' &&
      ((path[0] >= 'a' && path[0] <= 'z') ||
       (path[0] >= 'A' && path[0] <= 'Z')))
    return expand_path(lib_name, origin); // absolute path
  if (path[0] == '\\' && path[1] == '\\')
    return expand_path(lib_name, origin); // UNC, absolute path
#endif

  for (const char* p2 = path; *p2; p2++) {
    if (*p2 == ':')
      return jv_invalid_with_msg(jv_string("Library URIs not yet supported"));
    if (*p2 == '/')
      return expand_path(lib_name, origin);
#ifdef WIN32
    if (*p2 == sep)
      return expand_path(lib_name, origin);
#endif
  }

  struct stat st;
  int ret;

  jv lib_filename = jv_string_fmt("%s.jq", jv_string_value(lib_name));
  jv_array_foreach(lib_search_paths, i, spath) {
    jv testpath = jv_string_fmt("%s%c%s",
                                jv_string_value(spath), sep, jv_string_value(lib_filename));
    jv_free(spath);
    ret = stat(jv_string_value(testpath),&st);
    if (ret == 0) {
      jv_free(lib_filename);
      jv_free(lib_name);
      return testpath;
    }
    jv_free(testpath);
  }
  jv output = jv_invalid_with_msg(jv_string_fmt("Could not find library: %s", path));
  jv_free(lib_filename);
  jv_free(lib_name);
  return output;
}

jv compile_bind_lib(jq_state *jq, block* bb, const char* lib) {
  int nerrors = 0;
  struct locfile src;
  block funcs;
  jv data = jv_load_file(lib, 1);
  if (!jv_is_valid(data))
    return data;

  locfile_init(&src, jq, jv_string_value(data), jv_string_length_bytes(jv_copy(data)));
  nerrors = jq_parse_library(&src, &funcs);
  if (nerrors == 0)
    *bb = block_bind_referenced(funcs, *bb, OP_IS_CALL_PSEUDO);

  locfile_free(&src);
  jv_free(data);
  if (nerrors)
    return jv_invalid_with_msg(jv_string_fmt("Failed to parse library %s.",lib));
  return jv_true();
}
