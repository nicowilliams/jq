
#include <assert.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#include "util.h"
#include "jv.h"

// This isn't -technically- canonicalization, since I don't
// think it's necessary.  But that's the best name I had.
jv expand_path(jv path) {
  assert(jv_get_kind(path) == JV_KIND_STRING);
  const char *pstr = jv_string_value(path);
#ifdef WIN32
  char sep = '\\';
#else
  char sep = '/';
#endif
  if (pstr[0] == '~' && pstr[1] == '/') {
    jv ret;
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
  } else {
    return path;
  }
}

jv get_home() {
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

