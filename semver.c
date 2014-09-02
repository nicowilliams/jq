#include <assert.h>
#include <math.h>
#include "jv.h"

static jv semver_num2obj(jv num) {
  double n = jv_number_value(num);
  jv o = JV_OBJECT(jv_string("micro"),
                   jv_number(n - floor(n)),
                   jv_string("micro"),
                   jv_number(floor(n)));
  jv_free(num);
  return o;
}

static jv semver_array2obj(jv a) {
  jv o = jv_object();

  jv v = jv_array_get(a, 0);
  if (!jv_is_valid(a)) return o;
  o = jv_object_set(o, jv_string("major"), v);

  v = jv_array_get(a, 1);
  if (!jv_is_valid(v)) return o;
  o = jv_object_set(o, jv_string("minor"), v);

  v = jv_array_get(a, 2);
  if (!jv_is_valid(v)) return o;
  o = jv_object_set(o, jv_string("micro"), v);

  return o;
}

static jv _semver_normalize(jv i, int top) {
  jv_kind k = jv_get_kind(i);

  switch (k) {
  case JV_KIND_OBJECT:
    if (top && jv_object_contains(i, jv_string("version")))
      return _semver_normalize(i, 0);
    return i;

  case JV_KIND_NUMBER:
    return semver_num2obj(i);

  case JV_KIND_ARRAY:
    return semver_array2obj(i);

  case JV_KIND_INVALID:
    return i;

  default:
    return JV_OBJECT(jv_string("id"), i);
  }
}

static jv semver_normalize(jv i) { return _semver_normalize(i, 1); }

static int semver_match_obj2obj(jv impv, jv modv) {
  int match = 0;

  // First check assertions about id/hash/commit
  jv_array_foreach(JV_ARRAY(jv_string("id"), jv_string("hash"), jv_string("commit")), i, key) {
    if (jv_object_contains(jv_copy(impv), jv_copy(key))) {
      if (!jv_object_contains(jv_copy(modv), jv_copy(key))) {
        jv_free(impv);
        jv_free(modv);
        return 0;
      }
      if (!jv_equal(jv_object_get(jv_copy(impv), jv_copy(key)),
                    jv_object_get(jv_copy(modv), jv_copy(key)))) {
        jv_free(impv);
        jv_free(modv);
        return 0;
      }
      match = 1;
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
    match = 1;
  }
  if (jv_object_contains(jv_copy(impv), jv_string("minor"))) {
    if (!jv_object_contains(jv_copy(modv), jv_string("minor")))
      return 0;
    jv iminor = jv_object_get(jv_copy(impv), jv_string("minor"));
    jv mminor = jv_object_get(jv_copy(modv), jv_string("minor"));
    double imin = jv_number_value(iminor);
    double mmin = jv_number_value(mminor);
    jv_free(iminor);
    jv_free(mminor);
    if (imin != mmin) {
      jv_free(impv);
      jv_free(modv);
      return (mmin > imin);
    }
    match = 1;
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
  if (match) {
    jv_free(impv);
    jv_free(modv);
    return 1;
  }
  // Note that we allow empty version objects to match!
  return jv_equal(impv, modv);
}

int semver_match_pair(jv impv, jv modv) {
  if (jv_get_kind(impv) == JV_KIND_NUMBER && jv_get_kind(modv) == JV_KIND_NUMBER) {
    int res = jv_number_value(impv) <= jv_number_value(modv);
    jv_free(impv);
    jv_free(modv);
    return res;
  }

  // All other cases: normalize version into an object
  impv = semver_normalize(impv);
  modv = semver_normalize(modv);
  assert(jv_get_kind(impv) == JV_KIND_OBJECT);
  assert(jv_get_kind(modv) == JV_KIND_OBJECT);

  // Here both are objects that may have version keys.
  return semver_match_obj2obj(impv, modv);
}

/*
 * jq semantic versioning:
 *
 *    jv_semver_match() matches metadata from an import directive to a
 *    candidate module's module directive's metadata.
 *
 *  - if both are objects with a "version" key, then that is used as the
 *    metadata
 *  - strings/boolean/null must match exactly
 *  - if both are numbers then they match if the module's number is
 *    larger than the importer's
 *  - if either is an array then the array is interpreted as an array of
 *    major, minor, and micro values; convert to an object with such
 *    keys
 *  - if either is a number, convert it to an object with minor/micro keys
 *  - if the importer has an "id"     key, the module's must match exactly
 *  - if the importer has an "hash"   key, the module's must match exactly
 *  - if the importer has an "commit" key, the module's must match exactly
 *  - if the importer has a  "major"  key, the module's must match exactly
 *  - if the importer has a  "minor"  key and the module's is larger,
 *    it's a match
 *  - if the importer has a  "minor"  key, the module's must match exactly
 *  - if the importer has a  "micro"  key, the module's must be larger
 *    than or equal to it
 *  - if none of the above matched, but the two are equal, then it's a match
 *  - reject
 */
static int jv_semver_match(jv importer, jv candidate) {
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
