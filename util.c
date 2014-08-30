

#ifdef HAVE_MEMMEM
#define _GNU_SOURCE
#include <string.h>
#endif
#include <assert.h>
#ifndef WIN32
#include <pwd.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>


#include "util.h"
#include "jv.h"
#include "jv_alloc.h"

jv expand_path(jv path) {
  assert(jv_get_kind(path) == JV_KIND_STRING);
  const char *pstr = jv_string_value(path);
  jv ret = path;
  if (jv_string_length_bytes(jv_copy(path)) > 1 && pstr[0] == '~' && pstr[1] == '/') {
    jv home = get_home();
    if (jv_is_valid(home)) {
      ret = jv_string_fmt("%s/%s",jv_string_value(home),pstr+2);
      jv_free(home);
    } else {
      jv emsg = jv_invalid_get_msg(home);
      ret = jv_invalid_with_msg(jv_string_fmt("Could not expand %s. (%s)", pstr, jv_string_value(emsg)));
      jv_free(emsg);
    }
    jv_free(path);
  }
  return ret;
}

jv get_home() {
  jv ret;
  char *home = getenv("HOME");
  if (!home) {
#ifndef WIN32
    struct passwd* pwd = getpwuid(getuid());
    if (pwd) 
      ret = jv_string(pwd->pw_dir);
    else
      ret = jv_invalid_with_msg(jv_string("Could not find home directory."));
#else
    home = getenv("USERPROFILE");
    if (!home) {
      char *hd = getenv("HOMEDRIVE");
      if (!hd) hd = "";
      home = getenv("HOMEPATH");
      if (!home) {
        ret = jv_invalid_with_msg(jv_string("Could not find home directory."));
      } else {
        ret = jv_string_fmt("%s%s",hd,home);
      }
    } else {
      ret = jv_string(home);
    }
#endif
  } else {
    ret = jv_string(home);
  }
  return ret;
}

static long get_path_max(const char *path) {
  long path_max;

  errno = 0;

#ifdef _PC_PATH_MAX

  path_max = pathconf(path, _PC_PATH_MAX);
  if (path_max == -1) {
#ifdef PATH_MAX
    path_max = PATH_MAX;
#endif /* PATH_MAX */
  }

#else

#ifdef PATH_MAX
  path_max = PATH_MAX;
#else
#error "Don't know how to find PATH_MAX"
#endif

#endif

  return path_max;
}

jv jq_realpath(jv path) {
  long path_max = get_path_max(jv_string_value(path));
  if (path_max < 1) {
    jv_free(path);
    return jv_invalid_with_msg(jv_string_fmt("Could not discover value of PATH_MAX: %s",
                                             strerror(errno)));
  }
  char *buf = jv_mem_alloc(path_max);

#ifdef WIN32
  char *tmp = _fullpath(buf, jv_string_value(path), path_max);
#else
  char *tmp = realpath(jv_string_value(path), buf);
#endif

  if (tmp == NULL) {
    free(buf);
    return jv_invalid_with_msg(jv_string_fmt("Could not canonicalized path '%s': %s",
                                             jv_string_value(path), strerror(errno)));
  }
  jv_free(path);
  path = jv_string(tmp);
  free(tmp);
  return path;
}

jv jq_getcwd(void) {
  return jq_realpath(jv_string("."));
}

const void *_jq_memmem(const void *haystack, size_t haystacklen,
                       const void *needle, size_t needlelen) {
#ifdef HAVE_MEMMEM
  return (const void*)memmem(haystack, haystacklen, needle, needlelen);
#else
  const char *h = haystack;
  const char *n = needle;
  size_t hi, hi2, ni;

  if (haystacklen < needlelen || haystacklen == 0)
    return NULL;
  for (hi = 0; hi < (haystacklen - needlelen + 1); hi++) {
    for (ni = 0, hi2 = hi; ni < needlelen; ni++, hi2++) {
      if (h[hi2] != n[ni])
        goto not_this;
    }

    return &h[hi];

not_this:
    continue;
  }
  return NULL;
#endif /* !HAVE_MEMMEM */
}

