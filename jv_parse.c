#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "jv.h"
#include "jv_dtoa.h"
#include "jv_unicode.h"
#include "jv_alloc.h"
#include "jv_dtoa.h"

typedef const char* presult;

#define TRY(x) do {presult msg__ = (x); if (msg__) return msg__; } while(0)
#ifdef __GNUC__
#define pfunc __attribute__((warn_unused_result)) presult
#else
#define pfunc presult
#endif

struct jv_parser {
  const char* curr_buf;
  int curr_buf_length;
  int curr_buf_pos;
  int curr_buf_is_partial;
  unsigned bom_strip_position;

  jv* stack;
  int stackpos;
  int stacklen;
  jv next;
  jv path;          // for online parsing
  jv leaf;          // for online parsing
  jv online_ret;    // for online parsing (the output)

  /*
   * New plan for online parsing: we parse online separately (though
   * with parsing of scalars as common code).  Online is online all the
   * way down, not to some depth limit; consumers can deal with this be
   * reducing below the limits where they care, using setpath().
   *
   * The next function will output an array of path and scalar value.
   * Paths will be arrays of scalars.  Leaves that are empty arrays are
   * denoted as a final -1 in the path.  Leaves that are empty objects
   * are denoted as a finale null in the path.  End of top-level is
   * denoted as empty path, no value.
   *
   * Do we need a new API?  We need to indicate when we're done parsing
   * a top-level value, but that's easy: when the output of the next
   * function is an empty array!
   *
   * So no new API, just a new parser flag.  The online parser will
   * mostly consist of an online version of scan().
   */

  jv_parser_flags flags;
  
  char* tokenbuf;
  int tokenpos;
  int tokenlen;

  int line, column;
  
  struct dtoa_context dtoa;

  enum {
    JV_PARSER_NORMAL,
    JV_PARSER_STRING,
    JV_PARSER_STRING_ESCAPE
  } st;
};


static void parser_init(struct jv_parser* p, jv_parser_flags flags) {
  p->flags = flags;
  p->stack = 0;
  p->stacklen = p->stackpos = 0;
  p->next = jv_invalid();
  if (flags & JV_PARSE_ONLINE) {
    p->path = jv_array();
    p->online_ret = jv_invalid();
  } else {
    p->path = jv_invalid();
  }
  p->leaf = jv_invalid();
  p->tokenbuf = 0;
  p->tokenlen = p->tokenpos = 0;
  p->st = JV_PARSER_NORMAL;
  p->curr_buf = 0;
  p->curr_buf_length = p->curr_buf_pos = p->curr_buf_is_partial = 0;
  p->bom_strip_position = 0;
  p->line = 1;
  p->column = 0;
  jvp_dtoa_context_init(&p->dtoa);
}

static void parser_free(struct jv_parser* p) {
  jv_free(p->next);
  jvp_dtoa_context_free(&p->dtoa);
  if (p->flags & JV_PARSE_ONLINE) {
    jv_free(p->leaf);
    jv_free(p->path);
    jv_free(p->online_ret);
  } else {
    for (int i=0; i<p->stackpos; i++) 
      jv_free(p->stack[i]);
    jv_mem_free(p->stack);
    jv_mem_free(p->tokenbuf);
  }
}

static pfunc value(struct jv_parser* p, jv val) {
  if (jv_is_valid(p->next)) return "Expected separator between values";
  jv_free(p->next);
  p->next = val;
  return 0;
}

static void push(struct jv_parser* p, jv v) {
  assert(p->stackpos <= p->stacklen);
  if (p->stackpos == p->stacklen) {
    p->stacklen = p->stacklen * 2 + 10;
    p->stack = jv_mem_realloc(p->stack, p->stacklen * sizeof(jv));
  }
  assert(p->stackpos < p->stacklen);
  p->stack[p->stackpos++] = v;
}

static pfunc token(struct jv_parser* p, char ch) {
  switch (ch) {
  case '[':
    if (jv_is_valid(p->next)) return "Expected separator between values";
    push(p, jv_array());
    break;

  case '{':
    if (jv_is_valid(p->next)) return "Expected separator between values";
    push(p, jv_object());
    break;

  case ':':
    if (!jv_is_valid(p->next)) 
      return "Expected string key before ':'";
    if (p->stackpos == 0 || jv_get_kind(p->stack[p->stackpos-1]) != JV_KIND_OBJECT)
      return "':' not as part of an object";
    if (jv_get_kind(p->next) != JV_KIND_STRING)
      return "Object keys must be strings";
    push(p, p->next);
    p->next = jv_invalid();
    break;

  case ',':
    if (!jv_is_valid(p->next))
      return "Expected value before ','";
    if (p->stackpos == 0)
      return "',' not as part of an object or array";
    if (jv_get_kind(p->stack[p->stackpos-1]) == JV_KIND_ARRAY) {
      p->stack[p->stackpos-1] = jv_array_append(p->stack[p->stackpos-1], p->next);
      p->next = jv_invalid();
    } else if (jv_get_kind(p->stack[p->stackpos-1]) == JV_KIND_STRING) {
      assert(p->stackpos > 1 && jv_get_kind(p->stack[p->stackpos-2]) == JV_KIND_OBJECT);
      p->stack[p->stackpos-2] = jv_object_set(p->stack[p->stackpos-2], 
                                              p->stack[p->stackpos-1], p->next);
      p->stackpos--;
      p->next = jv_invalid();
    } else {
      // this case hits on input like {"a", "b"}
      return "Objects must consist of key:value pairs";
    }
    break;

  case ']':
    if (p->stackpos == 0 || jv_get_kind(p->stack[p->stackpos-1]) != JV_KIND_ARRAY)
      return "Unmatched ']'";
    if (jv_is_valid(p->next)) {
      if (p->stackpos != 1) {
        p->stack[p->stackpos-1] = jv_array_append(p->stack[p->stackpos-1], p->next);
        p->next = jv_invalid();
      }
    } else {
      if (jv_array_length(jv_copy(p->stack[p->stackpos-1])) != 0) {
        // this case hits on input like [1,2,3,]
        return "Expected another array element";
      }
    }
    jv_free(p->next);
    p->next = p->stack[--p->stackpos];
    break;

  case '}':
    if (p->stackpos == 0)
      return "Unmatched '}'";
    if (jv_is_valid(p->next)) {
      if (jv_get_kind(p->stack[p->stackpos-1]) != JV_KIND_STRING)
        return "Objects must consist of key:value pairs";
      assert(p->stackpos > 1 && jv_get_kind(p->stack[p->stackpos-2]) == JV_KIND_OBJECT);
      p->stack[p->stackpos-2] = jv_object_set(p->stack[p->stackpos-2], 
                                              p->stack[p->stackpos-1], p->next);
      p->stackpos--;
      p->next = jv_invalid();
    } else {
      if (jv_get_kind(p->stack[p->stackpos-1]) != JV_KIND_OBJECT)
        return "Unmatched '}'";
      if (jv_object_length(jv_copy(p->stack[p->stackpos-1])) != 0)
        return "Expected another key-value pair";
    }
    jv_free(p->next);
    p->next = p->stack[--p->stackpos];
    break;
  }
  return 0;
}


static void tokenadd(struct jv_parser* p, char c) {
  assert(p->tokenpos <= p->tokenlen);
  if (p->tokenpos == p->tokenlen) {
    p->tokenlen = p->tokenlen*2 + 256;
    p->tokenbuf = jv_mem_realloc(p->tokenbuf, p->tokenlen);
  }
  assert(p->tokenpos < p->tokenlen);
  p->tokenbuf[p->tokenpos++] = c;
}

static int unhex4(char* hex) {
  int r = 0;
  for (int i=0; i<4; i++) {
    char c = *hex++;
    int n;
    if ('0' <= c && c <= '9') n = c - '0';
    else if ('a' <= c && c <= 'f') n = c - 'a' + 10;
    else if ('A' <= c && c <= 'F') n = c - 'A' + 10;
    else return -1;
    r <<= 4;
    r |= n;
  }
  return r;
}

static pfunc found_string(struct jv_parser* p) {
  char* in = p->tokenbuf;
  char* out = p->tokenbuf;
  char* end = p->tokenbuf + p->tokenpos;
  
  while (in < end) {
    char c = *in++;
    if (c == '\\') {
      if (in >= end)
        return "Expected escape character at end of string";
      c = *in++;
      switch (c) {
      case '\\':
      case '"':
      case '/': *out++ = c;    break;
      case 'b': *out++ = '\b'; break;
      case 'f': *out++ = '\f'; break;
      case 't': *out++ = '\t'; break;
      case 'n': *out++ = '\n'; break;
      case 'r': *out++ = '\r'; break;

      case 'u':
        /* ahh, the complicated case */
        if (in + 4 > end)
          return "Invalid \\uXXXX escape";
        int hexvalue = unhex4(in);
        if (hexvalue < 0)
          return "Invalid characters in \\uXXXX escape";
        unsigned long codepoint = (unsigned long)hexvalue;
        in += 4;
        if (0xD800 <= codepoint && codepoint <= 0xDBFF) {
          /* who thought UTF-16 surrogate pairs were a good idea? */
          if (in + 6 > end || in[0] != '\\' || in[1] != 'u')
            return "Invalid \\uXXXX\\uXXXX surrogate pair escape";
          unsigned long surrogate = unhex4(in+2);
          if (!(0xDC00 <= surrogate && surrogate <= 0xDFFF))
            return "Invalid \\uXXXX\\uXXXX surrogate pair escape";
          in += 6;
          codepoint = 0x10000 + (((codepoint - 0xD800) << 10)
                                 |(surrogate - 0xDC00));
        }
        // FIXME assert valid codepoint
        out += jvp_utf8_encode(codepoint, out);
        break;

      default:
        return "Invalid escape";
      }
    } else {
      *out++ = c;
    }
  }
  TRY(value(p, jv_string_sized(p->tokenbuf, out - p->tokenbuf)));
  p->tokenpos = 0;
  return 0;
}

static pfunc check_literal(struct jv_parser* p) {
  if (p->tokenpos == 0) return 0;

  const char* pattern = 0;
  int plen;
  jv v;
  switch (p->tokenbuf[0]) {
  case 't': pattern = "true"; plen = 4; v = jv_true(); break;
  case 'f': pattern = "false"; plen = 5; v = jv_false(); break;
  case 'n': pattern = "null"; plen = 4; v = jv_null(); break;
  }
  if (pattern) {
    if (p->tokenpos != plen) return "Invalid literal";
    for (int i=0; i<plen; i++) 
      if (p->tokenbuf[i] != pattern[i])
        return "Invalid literal";
    TRY(value(p, v));
  } else {
    // FIXME: better parser
    p->tokenbuf[p->tokenpos] = 0; // FIXME: invalid
    char* end = 0;
    double d = jvp_strtod(&p->dtoa, p->tokenbuf, &end);
    if (end == 0 || *end != 0)
      return "Invalid numeric literal";
    TRY(value(p, jv_number(d)));
  }
  p->tokenpos = 0;
  return 0;
}

typedef enum {
  LITERAL,
  WHITESPACE,
  STRUCTURE,
  QUOTE,
  INVALID
} chclass;

static chclass classify(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\r':
  case '\n':
    return WHITESPACE;
  case '"':
    return QUOTE;
  case '[':
  case ',':
  case ']':
  case '{':
  case ':':
  case '}':
    return STRUCTURE;
  default:
    return LITERAL;
  }
}


static const presult OK = "output produced";

static int check_done(struct jv_parser* p, jv* out) {
  if (p->stackpos == 0 && jv_is_valid(p->next)) {
    *out = p->next;
    p->next = jv_invalid();
    return 1;
  } else {
    return 0;
  }
}

static pfunc scan(struct jv_parser* p, char ch, jv* out) {
  p->column++;
  if (ch == '\n') {
    p->line++;
    p->column = 0;
  }
  presult answer = 0;
  if (p->st == JV_PARSER_NORMAL) {
    chclass cls = classify(ch);
    if (cls != LITERAL) {
      TRY(check_literal(p));
      if (check_done(p, out)) answer = OK;
    }
    switch (cls) {
    case LITERAL:
      tokenadd(p, ch);
      break;
    case WHITESPACE:
      break;
    case QUOTE:
      p->st = JV_PARSER_STRING;
      break;
    case STRUCTURE:
      TRY(token(p, ch));
      break;
    case INVALID:
      return "Invalid character";
    }
    if (check_done(p, out)) answer = OK;
  } else {
    if (ch == '"' && p->st == JV_PARSER_STRING) {
      TRY(found_string(p));
      p->st = JV_PARSER_NORMAL;
      if (check_done(p, out)) answer = OK;
    } else {
      tokenadd(p, ch);
      if (ch == '\\' && p->st == JV_PARSER_STRING) {
        p->st = JV_PARSER_STRING_ESCAPE;
      } else {
        p->st = JV_PARSER_STRING;
      }
    }
  }
  return answer;
}

struct jv_parser* jv_parser_new(jv_parser_flags flags) {
  struct jv_parser* p = jv_mem_alloc(sizeof(struct jv_parser));
  parser_init(p, flags);
  return p;
}

void jv_parser_free(struct jv_parser* p) {
  parser_free(p);
  jv_mem_free(p);
}

static const unsigned char UTF8_BOM[] = {0xEF,0xBB,0xBF};

void jv_parser_set_buf(struct jv_parser* p, const char* buf, int length, int is_partial) {
  assert((p->curr_buf == 0 || p->curr_buf_pos == p->curr_buf_length)
         && "previous buffer not exhausted");
  while (p->bom_strip_position < sizeof(UTF8_BOM)) {
    if ((unsigned char)*buf == UTF8_BOM[p->bom_strip_position]) {
      // matched a BOM character
      buf++;
      length--;
      p->bom_strip_position++;
    } else {
      if (p->bom_strip_position == 0) {
        // no BOM in this document
        p->bom_strip_position = sizeof(UTF8_BOM);
      } else {
        // malformed BOM (prefix present, rest missing)
        p->bom_strip_position = 0xff;
      }
    }
  }
  p->curr_buf = buf;
  p->curr_buf_length = length;
  p->curr_buf_pos = 0;
  p->curr_buf_is_partial = is_partial;
}

static jv parser_next(struct jv_parser* p) {
  assert(p->curr_buf && "a buffer must be provided");
  if (p->bom_strip_position == 0xff) return jv_invalid_with_msg(jv_string("Malformed BOM"));
  jv value;
  presult msg = 0;
  while (!msg && p->curr_buf_pos < p->curr_buf_length) {
    char ch = p->curr_buf[p->curr_buf_pos++];
    msg = scan(p, ch, &value);
  }
  if (msg == OK) {
    return value;
  } else if (msg) {
    return jv_invalid_with_msg(jv_string_fmt("%s at line %d, column %d", msg, p->line, p->column));
  } else if (p->curr_buf_is_partial) {
    assert(p->curr_buf_pos == p->curr_buf_length);
    // need another buffer
    return jv_invalid();
  } else {
    assert(p->curr_buf_pos == p->curr_buf_length);
    // at EOF
    if (p->st != JV_PARSER_NORMAL) 
      return jv_invalid_with_msg(jv_string("Unfinished string"));
    if ((msg = check_literal(p)))
      return jv_invalid_with_msg(jv_string(msg));
    if (p->stackpos != 0)
      return jv_invalid_with_msg(jv_string("Unfinished JSON term"));
    // p->next is either invalid (nothing here but no syntax error)
    // or valid (this is the value). either way it's the thing to return
    value = p->next;
    p->next = jv_invalid();
    return value;
  }
}

static jv_kind path_kind_last(struct jv_parser* p) {
  jv v = jv_array_get(jv_copy(p->path), jv_array_length(jv_copy(p->path)) - 1);
  jv_kind k = jv_get_kind(v);
  jv_free(v);
  return k;
}

static int path_length(struct jv_parser* p) {
  return jv_array_length(jv_copy(p->path));
}

static jv path_last(struct jv_parser* p) {
  return jv_array_get(jv_copy(p->path), jv_array_length(jv_copy(p->path)) - 1);
}

static void path_set_last(struct jv_parser* p, jv val) {
  p->path = jv_array_set(p->path, jv_array_length(jv_copy(p->path)) - 1, val);
}

static void path_push(struct jv_parser* p, jv val) {
  p->path = jv_array_append(p->path, val);
}

static void path_pop(struct jv_parser* p) {
  p->path = jv_array_slice(p->path, 0, jv_array_length(jv_copy(p->path)) - 1);
}

static void make_out(struct jv_parser* p) {
  int plen = path_length(p);
  jv leaf;
  // XXX Substitute empty array/object if path_last is -1 or null
  if (plen == 0 && jv_is_valid(p->next)) {
    assert(!jv_is_valid(p->leaf));
    leaf = p->next;
    p->next = jv_invalid();
  } else if (jv_is_valid(p->leaf)) {
    assert(plen > 0);
    assert(!jv_is_valid(p->next));
    leaf = p->leaf;
  } else {
    return;
  }
  p->online_ret = jv_array();
  p->online_ret = jv_array_append(p->online_ret, jv_copy(p->path));
  p->online_ret = jv_array_append(p->online_ret, leaf);
  p->leaf = jv_invalid();
}

static pfunc token_online(struct jv_parser* p, char ch) {
  jv_kind k;
  switch (ch) {
  case '[':
    if (jv_is_valid(p->next)) return "Expected separator between values";
    path_push(p, jv_number(-1));
    break;

  case '{':
    if (jv_is_valid(p->next)) return "Expected separator between values";
    path_push(p, jv_null());
    break;

  case ':':
    if (!jv_is_valid(p->next)) 
      return "Expected string key before ':'";
    k = path_kind_last(p);
    if (k != JV_KIND_NULL && k != JV_KIND_STRING)
      return "':' not as part of an object";
    if (jv_get_kind(p->next) != JV_KIND_STRING)
      return "Object keys must be strings";
    path_set_last(p, p->next);
    p->next = jv_invalid();
    break;

  case ',':
    if (!jv_is_valid(p->next))
      return "Expected value before ','";
    if (path_length(p) == 0)
      return "',' not as part of an object or array";
    k = path_kind_last(p);
    assert(k == JV_KIND_NUMBER || k == JV_KIND_NULL || k == JV_KIND_STRING);
    if (k == JV_KIND_NUMBER) {
      p->leaf = p->next;
      p->next = jv_invalid();
      path_set_last(p, jv_number(jv_number_value(path_last(p)) + 1));
    } else if (k == JV_KIND_NULL || k == JV_KIND_STRING) {
      if (!jv_is_valid(p->next)) {
        // this case hits on input like {"a", ...}
        return "Objects must consist of key:value pairs";
      }
      p->leaf = p->next;
      p->next = jv_invalid();
    }
    make_out(p);
    break;

  case ']':
    if (path_length(p) == 0)
      return "Unmatched ']'";
    k = path_kind_last(p);
    if (k != JV_KIND_NUMBER)
      return "Unmatched ']'";
    jv last = path_last(p);
    if (jv_number_value(last) == -1) {
      // This is a leaf
      assert(!jv_is_valid(p->next));
      p->leaf = jv_array();
      path_pop(p);
      make_out(p);
    } else {
      if (jv_is_valid(p->next)) {
        p->leaf = p->next;
        p->next = jv_invalid();
      }
      path_set_last(p, jv_number(jv_number_value(last) + 1));
      // Note: order reversed compared to -1 case above
      make_out(p);
      path_pop(p);
    }
    jv_free(last);
    // XXX We don't keep enough state to check for [1,2,3,]
    break;

  case '}':
    if (path_length(p) == 0)
      return "Unmatched '}'";
    k = path_kind_last(p);
    if (k != JV_KIND_NULL || JV_KIND_STRING)
      return "Unmatched '}'";
    if (k == JV_KIND_NULL) {
      assert(!jv_is_valid(p->next));
      p->leaf = jv_object();
      path_pop(p);
      make_out(p);
      break;
    } else {
      if (!jv_is_valid(p->next))
        return "Objects must consist of key:value pairs";
      p->leaf = p->next;
      p->next = jv_invalid();
      // Note: order reversed compared to null case above
      make_out(p);
      path_pop(p);
      // XXX We don't keep enough state to check for {"a":1,}
    }
    break;
  }
  return 0;
}

static int check_done_online(struct jv_parser* p, jv *out) {
  if (jv_is_valid(p->next) && jv_array_length(jv_copy(p->path)) == 0)
    make_out(p);
  if (jv_is_valid(p->online_ret)) {
    *out = p->online_ret;
    p->online_ret = jv_invalid();
    return 1;
  }
  return 0;
}

static pfunc scan_online(struct jv_parser* p, char ch, jv* out) {
  p->column++;
  if (ch == '\n') {
    p->line++;
    p->column = 0;
  }
  presult answer = 0;
  if (p->st == JV_PARSER_NORMAL) {
    chclass cls = classify(ch);
    if (cls != LITERAL) {
      TRY(check_literal(p));
      if (check_done_online(p, out)) answer = OK;
    }
    switch (cls) {
    case LITERAL:
      tokenadd(p, ch);
      break;
    case WHITESPACE:
      break;
    case QUOTE:
      p->st = JV_PARSER_STRING;
      break;
    case STRUCTURE:
      TRY(token_online(p, ch));
      break;
    case INVALID:
      return "Invalid character";
    }
    if (check_done_online(p, out)) answer = OK;
  } else {
    if (ch == '"' && p->st == JV_PARSER_STRING) {
      TRY(found_string(p));
      p->st = JV_PARSER_NORMAL;
      if (check_done_online(p, out)) answer = OK;
    } else {
      tokenadd(p, ch);
      if (ch == '\\' && p->st == JV_PARSER_STRING) {
        p->st = JV_PARSER_STRING_ESCAPE;
      } else {
        p->st = JV_PARSER_STRING;
      }
    }
  }
  return answer;
}

static jv parser_next_online(struct jv_parser* p) {
  assert(p->curr_buf && "a buffer must be provided");
  if (p->bom_strip_position == 0xff) return jv_invalid_with_msg(jv_string("Malformed BOM"));
  presult msg = 0;
  jv value;
  while (!msg && p->curr_buf_pos < p->curr_buf_length) {
    char ch = p->curr_buf[p->curr_buf_pos++];
    msg = scan_online(p, ch, &value);
  }
  if (msg == OK) {
    return value;
  } else if (msg) {
    return jv_invalid_with_msg(jv_string_fmt("%s at line %d, column %d", msg, p->line, p->column));
  } else if (p->curr_buf_is_partial) {
    assert(p->curr_buf_pos == p->curr_buf_length);
    // need another buffer
    return jv_invalid();
  } else {
    assert(p->curr_buf_pos == p->curr_buf_length);
    // at EOF
    if (p->st != JV_PARSER_NORMAL) 
      return jv_invalid_with_msg(jv_string("Unfinished string"));
    if ((msg = check_literal(p)))
      return jv_invalid_with_msg(jv_string(msg));
    if (path_length(p) != 0)
      return jv_invalid_with_msg(jv_string("Unfinished JSON term"));
    return jv_invalid();
  }
}

jv jv_parser_next(struct jv_parser* p) {
  if (p->flags & JV_PARSE_ONLINE)
    return parser_next_online(p);
  return parser_next(p);
}

jv jv_parse_sized(const char* string, int length) {
  struct jv_parser parser;
  parser_init(&parser, 0);
  jv_parser_set_buf(&parser, string, length, 0);
  jv value = jv_parser_next(&parser);
  if (jv_is_valid(value)) {
    jv next = jv_parser_next(&parser);
    if (jv_is_valid(next)) {
      // multiple JSON values, we only wanted one
      jv_free(value);
      jv_free(next);
      value = jv_invalid_with_msg(jv_string("Unexpected extra JSON values"));
    } else if (jv_invalid_has_msg(jv_copy(next))) {
      // parser error after the first JSON value
      jv_free(value);
      value = next;
    } else {
      // a single valid JSON value
      jv_free(next);
    }
  } else if (jv_invalid_has_msg(jv_copy(value))) {
    // parse error, we'll return it
  } else {
    // no value at all
    jv_free(value);
    value = jv_invalid_with_msg(jv_string("Expected JSON value"));
  }
  parser_free(&parser);

  if (!jv_is_valid(value) && jv_invalid_has_msg(jv_copy(value))) {
    jv msg = jv_invalid_get_msg(value);
    value = jv_invalid_with_msg(jv_string_fmt("%s (while parsing '%s')",
                                              jv_string_value(msg),
                                              string));
    jv_free(msg);
  }
  return value;
}

jv jv_parse(const char* string) {
  return jv_parse_sized(string, strlen(string));
}
