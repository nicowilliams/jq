
#include <assert.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#include "util.h"
#include "jv.h"

// This isn't -technically- canonicalization, since I don't
// think it's necessary.  But that's the best name I had.
jv canonicalize_path(jv path) {
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

