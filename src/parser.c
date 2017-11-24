/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "src/parser.y" /* yacc.c:339  */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "compile.h"
#include "jv_alloc.h"
#define YYMALLOC jv_mem_alloc
#define YYFREE jv_mem_free

#line 77 "src/parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_SRC_PARSER_H_INCLUDED
# define YY_YY_SRC_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 11 "src/parser.y" /* yacc.c:355  */

#include "locfile.h"
struct lexer_param;

#define YYLTYPE location
#define YYLLOC_DEFAULT(Loc, Rhs, N)             \
  do {                                          \
    if (N) {                                    \
      (Loc).start = YYRHSLOC(Rhs, 1).start;     \
      (Loc).end = YYRHSLOC(Rhs, N).end;         \
    } else {                                    \
      (Loc).start = YYRHSLOC(Rhs, 0).end;       \
      (Loc).end = YYRHSLOC(Rhs, 0).end;         \
    }                                           \
  } while (0)

#line 124 "src/parser.c" /* yacc.c:355  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INVALID_CHARACTER = 258,
    IDENT = 259,
    FIELD = 260,
    LITERAL = 261,
    FORMAT = 262,
    REC = 263,
    SETMOD = 264,
    EQ = 265,
    NEQ = 266,
    DEFINEDOR = 267,
    AS = 268,
    DEF = 269,
    EXPORT = 270,
    MODULE = 271,
    IMPORT = 272,
    INCLUDE = 273,
    IF = 274,
    THEN = 275,
    ELSE = 276,
    ELSE_IF = 277,
    REDUCE = 278,
    FOREACH = 279,
    END = 280,
    AND = 281,
    OR = 282,
    TRY = 283,
    CATCH = 284,
    LABEL = 285,
    BREAK = 286,
    LOC = 287,
    SETPIPE = 288,
    SETPLUS = 289,
    SETMINUS = 290,
    SETMULT = 291,
    SETDIV = 292,
    SETDEFINEDOR = 293,
    LESSEQ = 294,
    GREATEREQ = 295,
    ALTERNATION = 296,
    QQSTRING_START = 297,
    QQSTRING_TEXT = 298,
    QQSTRING_INTERP_START = 299,
    QQSTRING_INTERP_END = 300,
    QQSTRING_END = 301,
    FUNCDEF = 302,
    NONOPT = 303
  };
#endif
/* Tokens.  */
#define INVALID_CHARACTER 258
#define IDENT 259
#define FIELD 260
#define LITERAL 261
#define FORMAT 262
#define REC 263
#define SETMOD 264
#define EQ 265
#define NEQ 266
#define DEFINEDOR 267
#define AS 268
#define DEF 269
#define EXPORT 270
#define MODULE 271
#define IMPORT 272
#define INCLUDE 273
#define IF 274
#define THEN 275
#define ELSE 276
#define ELSE_IF 277
#define REDUCE 278
#define FOREACH 279
#define END 280
#define AND 281
#define OR 282
#define TRY 283
#define CATCH 284
#define LABEL 285
#define BREAK 286
#define LOC 287
#define SETPIPE 288
#define SETPLUS 289
#define SETMINUS 290
#define SETMULT 291
#define SETDIV 292
#define SETDEFINEDOR 293
#define LESSEQ 294
#define GREATEREQ 295
#define ALTERNATION 296
#define QQSTRING_START 297
#define QQSTRING_TEXT 298
#define QQSTRING_INTERP_START 299
#define QQSTRING_INTERP_END 300
#define QQSTRING_END 301
#define FUNCDEF 302
#define NONOPT 303

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 31 "src/parser.y" /* yacc.c:355  */

  jv literal;
  block blk;

#line 237 "src/parser.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr);

#endif /* !YY_YY_SRC_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 125 "src/parser.y" /* yacc.c:358  */

#include "lexer.h"
struct lexer_param {
  yyscan_t lexer;
};
#define FAIL(loc, msg)                                             \
  do {                                                             \
    location l = loc;                                              \
    yyerror(&l, answer, errors, locations, lexer_param_ptr, msg);  \
    /*YYERROR*/;                                                   \
  } while (0)

void yyerror(YYLTYPE* loc, block* answer, int* errors,
             struct locfile* locations, struct lexer_param* lexer_param_ptr, const char *s){
  (*errors)++;
  if (strstr(s, "unexpected")) {
#ifdef WIN32
      locfile_locate(locations, *loc, "jq: error: %s (Windows cmd shell quoting issues?)", s);
#else
      locfile_locate(locations, *loc, "jq: error: %s (Unix shell quoting issues?)", s);
#endif
  } else {
      locfile_locate(locations, *loc, "jq: error: %s", s);
  }
}

int yylex(YYSTYPE* yylval, YYLTYPE* yylloc, block* answer, int* errors,
          struct locfile* locations, struct lexer_param* lexer_param_ptr) {
  yyscan_t lexer = lexer_param_ptr->lexer;
  int tok = jq_yylex(yylval, yylloc, lexer);
  if ((tok == LITERAL || tok == QQSTRING_TEXT) && !jv_is_valid(yylval->literal)) {
    jv msg = jv_invalid_get_msg(jv_copy(yylval->literal));
    if (jv_get_kind(msg) == JV_KIND_STRING) {
      FAIL(*yylloc, jv_string_value(msg));
    } else {
      FAIL(*yylloc, "Invalid literal");
    }
    jv_free(msg);
    jv_free(yylval->literal);
    yylval->literal = jv_null();
  }
  return tok;
}

/* Returns string message if the block is a constant that is not valid as an
 * object key. */
static jv check_object_key(block k) {
  if (block_is_const(k) && block_const_kind(k) != JV_KIND_STRING) {
    char errbuf[15];
    return jv_string_fmt("Cannot use %s (%s) as object key",
        jv_kind_name(block_const_kind(k)),
        jv_dump_string_trunc(jv_copy(block_const(k)), errbuf, sizeof(errbuf)));
  }
  return jv_invalid();
}

static block gen_index(block obj, block key) {
  return BLOCK(gen_subexp(key), obj, gen_op_simple(INDEX));
}

static block gen_index_opt(block obj, block key) {
  return BLOCK(gen_subexp(key), obj, gen_op_simple(INDEX_OPT));
}

static block gen_slice_index(block obj, block start, block end, opcode idx_op) {
  block key = BLOCK(gen_subexp(gen_const(jv_object())),
                    gen_subexp(gen_const(jv_string("start"))),
                    gen_subexp(start),
                    gen_op_simple(INSERT),
                    gen_subexp(gen_const(jv_string("end"))),
                    gen_subexp(end),
                    gen_op_simple(INSERT));
  return BLOCK(key, obj, gen_op_simple(idx_op));
}

static block constant_fold(block a, block b, int op) {
  if (!block_is_single(a) || !block_is_const(a) ||
      !block_is_single(b) || !block_is_const(b))
    return gen_noop();
  if (op == '+') {
    if (block_const_kind(a) == JV_KIND_NULL) {
      block_free(a);
      return b;
    }
    if (block_const_kind(b) == JV_KIND_NULL) {
      block_free(b);
      return a;
    }
  }
  if (block_const_kind(a) != block_const_kind(b))
    return gen_noop();

  jv res = jv_invalid();

  if (block_const_kind(a) == JV_KIND_NUMBER) {
    double na = jv_number_value(block_const(a));
    double nb = jv_number_value(block_const(b));
    switch (op) {
    case '+': res = jv_number(na + nb); break;
    case '-': res = jv_number(na - nb); break;
    case '*': res = jv_number(na * nb); break;
    case '/': res = jv_number(na / nb); break;
    case EQ:  res = (na == nb ? jv_true() : jv_false()); break;
    case NEQ: res = (na != nb ? jv_true() : jv_false()); break;
    case '<': res = (na < nb ? jv_true() : jv_false()); break;
    case '>': res = (na > nb ? jv_true() : jv_false()); break;
    case LESSEQ: res = (na <= nb ? jv_true() : jv_false()); break;
    case GREATEREQ: res = (na >= nb ? jv_true() : jv_false()); break;
    default: break;
    }
  } else if (op == '+' && block_const_kind(a) == JV_KIND_STRING) {
    res = jv_string_concat(block_const(a),  block_const(b));
  } else {
    return gen_noop();
  }

  if (jv_get_kind(res) == JV_KIND_INVALID)
    return gen_noop();

  block_free(a);
  block_free(b);
  return gen_const(res);
}

static block gen_binop(block a, block b, int op) {
  block folded = constant_fold(a, b, op);
  if (!block_is_noop(folded))
    return folded;

  const char* funcname = 0;
  switch (op) {
  case '+': funcname = "_plus"; break;
  case '-': funcname = "_minus"; break;
  case '*': funcname = "_multiply"; break;
  case '/': funcname = "_divide"; break;
  case '%': funcname = "_mod"; break;
  case EQ: funcname = "_equal"; break;
  case NEQ: funcname = "_notequal"; break;
  case '<': funcname = "_less"; break;
  case '>': funcname = "_greater"; break;
  case LESSEQ: funcname = "_lesseq"; break;
  case GREATEREQ: funcname = "_greatereq"; break;
  }
  assert(funcname);

  return gen_call(funcname, BLOCK(gen_lambda(a), gen_lambda(b)));
}

static block gen_format(block a, jv fmt) {
  return BLOCK(a, gen_call("format", gen_lambda(gen_const(fmt))));
}

static block gen_definedor_assign(block object, block val) {
  block tmp = gen_op_var_fresh(STOREV, "tmp");
  return BLOCK(gen_op_simple(DUP),
               val, tmp,
               gen_call("_modify", BLOCK(gen_lambda(object),
                                         gen_lambda(gen_definedor(gen_noop(),
                                                                  gen_op_bound(LOADV, tmp))))));
}

static block gen_update(block object, block val, int optype) {
  block tmp = gen_op_var_fresh(STOREV, "tmp");
  return BLOCK(gen_op_simple(DUP),
               val,
               tmp,
               gen_call("_modify", BLOCK(gen_lambda(object),
                                         gen_lambda(gen_binop(gen_noop(),
                                                              gen_op_bound(LOADV, tmp),
                                                              optype)))));
}


#line 440 "src/parser.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2143

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  170
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  340

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    63,    57,     2,     2,
      61,    62,    55,    53,    49,    54,    67,    56,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    64,    60,
      51,    50,    52,    59,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    66,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,    48,    69,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    58
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   301,   301,   305,   310,   313,   324,   327,   332,   335,
     338,   343,   348,   351,   355,   359,   363,   366,   371,   375,
     379,   384,   391,   395,   399,   403,   407,   411,   415,   419,
     423,   427,   431,   435,   439,   443,   447,   451,   455,   461,
     467,   471,   475,   479,   483,   487,   491,   495,   499,   504,
     507,   524,   533,   540,   548,   559,   564,   570,   575,   582,
     585,   588,   591,   596,   601,   607,   615,   615,   619,   619,
     626,   629,   632,   638,   641,   646,   649,   652,   658,   661,
     664,   672,   676,   679,   682,   685,   688,   691,   694,   697,
     700,   704,   709,   715,   718,   721,   724,   727,   730,   733,
     736,   739,   742,   745,   748,   751,   758,   761,   764,   767,
     774,   778,   782,   794,   799,   800,   801,   802,   805,   808,
     813,   818,   821,   826,   829,   834,   838,   841,   846,   849,
     854,   857,   862,   865,   868,   871,   874,   877,   885,   891,
     894,   897,   900,   903,   906,   909,   912,   915,   918,   921,
     924,   927,   930,   933,   936,   939,   942,   945,   948,   953,
     956,   957,   958,   961,   964,   967,   970,   974,   978,   982,
     990
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INVALID_CHARACTER", "IDENT", "FIELD",
  "LITERAL", "FORMAT", "\"..\"", "\"%=\"", "\"==\"", "\"!=\"", "\"//\"",
  "\"as\"", "\"def\"", "\"export\"", "\"module\"", "\"import\"",
  "\"include\"", "\"if\"", "\"then\"", "\"else\"", "\"elif\"",
  "\"reduce\"", "\"foreach\"", "\"end\"", "\"and\"", "\"or\"", "\"try\"",
  "\"catch\"", "\"label\"", "\"break\"", "\"__loc__\"", "\"|=\"", "\"+=\"",
  "\"-=\"", "\"*=\"", "\"/=\"", "\"//=\"", "\"<=\"", "\">=\"", "\"?//\"",
  "QQSTRING_START", "QQSTRING_TEXT", "QQSTRING_INTERP_START",
  "QQSTRING_INTERP_END", "QQSTRING_END", "FUNCDEF", "'|'", "','", "'='",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "NONOPT", "'?'", "';'",
  "'('", "')'", "'$'", "':'", "'['", "']'", "'.'", "'{'", "'}'", "$accept",
  "TopLevel", "Module", "Imports", "FuncDefs", "Exp", "Import",
  "ImportWhat", "ImportFrom", "FuncDef", "ExportOnlyFuncDef", "Params",
  "Param", "VarargParam", "String", "@1", "@2", "QQString", "ElseBody",
  "ExpD", "Term", "Args", "Arg", "RepPatterns", "Patterns", "Pattern",
  "ArrayPats", "ObjPats", "ObjPat", "Keyword", "MkDict", "MkDictPair", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   124,    44,
      61,    60,    62,    43,    45,    42,    47,    37,   303,    63,
      59,    40,    41,    36,    58,    91,    93,    46,   123,   125
};
# endif

#define YYPACT_NINF -165

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-165)))

#define YYTABLE_NINF -160

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-160)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   915,    71,    26,    24,    53,  -165,    74,  -165,   116,
     915,   171,   171,   915,    66,    13,  -165,   915,   124,   590,
      21,   522,   286,   353,  1477,   915,  -165,    -3,  -165,    17,
      17,   980,    26,   785,   915,  -165,  -165,   -24,  1825,    -2,
       8,   101,   127,  -165,   128,  -165,    22,    70,    76,  1307,
    -165,  -165,    73,  -165,  1117,  -165,   135,    74,    81,   -23,
      78,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
    -165,   915,   139,    80,    89,    87,   108,   915,   915,   915,
     915,   915,   915,   915,   915,   915,   915,   915,   915,   915,
     915,   915,   915,   915,   915,   915,   915,   915,   915,   915,
     915,  -165,  -165,  1995,    99,   -11,   454,    17,   146,  -165,
    -165,    35,  -165,  1995,   980,   147,  -165,  -165,  1512,  1995,
      61,  -165,  -165,    11,   915,   655,   -11,   -11,   720,   112,
    -165,    12,   915,  -165,  -165,  -165,  -165,  -165,  -165,   411,
     162,  -165,   162,  1341,  -165,   162,   162,  -165,   411,  2063,
     348,   348,  2029,   436,  2084,  2063,  2063,  2063,  2063,  2063,
    2063,   348,   348,  1995,  2029,  2063,   348,   348,    22,    22,
     103,   103,   103,  -165,   160,   -11,  1045,   130,   117,   131,
     107,   915,   115,  1083,   122,    19,   178,  -165,  -165,   147,
    -165,   915,  -165,    23,   118,   180,    64,  -165,  -165,  1547,
    -165,  1774,   125,   126,  -165,  -165,   915,  -165,   915,  -165,
    1151,   -13,  -165,   162,   140,     3,   140,   132,   140,   140,
    -165,  -165,  -165,   -18,   133,   134,   915,   185,   137,   -20,
    -165,   141,   -11,   915,  -165,  1185,  -165,   850,   144,  -165,
    -165,   188,   -19,  -165,  -165,   136,  -165,    11,   142,  -165,
     915,   915,  -165,   915,   915,  1995,  1859,  -165,  -165,   162,
     162,   -11,  -165,   -11,   -11,  1375,   143,   -11,  1045,  -165,
     -11,   153,  1995,   149,   150,  1219,  -165,  -165,    11,   915,
     206,  -165,  -165,   915,  1910,  1961,  1582,  1617,  -165,   140,
     140,  -165,  -165,  -165,   148,   -11,  -165,  -165,  -165,  -165,
    -165,   152,    65,  1652,   154,  1687,  -165,   915,   915,   915,
     -11,  -165,  -165,   157,  -165,  -165,  -165,  1774,  1409,  1253,
    -165,   915,  -165,  -165,   915,  -165,  1722,  1443,  -165,  -165
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     6,   112,    87,   103,   105,    79,     0,
       0,     0,     0,     0,     0,     0,    66,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104,    48,     1,     0,
       0,     8,     6,     0,     0,    83,    68,     0,     0,     0,
       0,    19,     0,    81,     0,    70,    33,     0,     0,     0,
     111,   110,     0,   108,     0,    90,     0,     0,    89,     0,
     168,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,     0,   166,     0,     0,   160,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     5,    11,    86,     0,     0,     0,     0,    54,
      53,     0,     3,     2,     8,     8,     7,    49,     0,   120,
       0,   118,    70,     0,     0,     0,     0,     0,     0,     0,
      80,     0,     0,   114,   106,   115,   107,    91,    85,     0,
       0,   117,     0,     0,   167,     0,     0,   109,     0,    41,
      42,    43,    26,    25,    24,    28,    32,    35,    37,    40,
      27,    46,    47,    29,    30,    23,    44,    45,    31,    34,
      36,    38,    39,    82,     0,     0,     0,     0,     0,   124,
       0,     0,    96,     0,    88,     0,     0,     9,    10,     8,
      50,     0,   113,     0,    64,     0,     0,    60,    59,     0,
      17,     0,     0,     0,    20,    18,     0,    71,     0,    67,
       0,     0,   162,     0,   170,    77,   163,     0,   165,   164,
     161,   125,   128,     0,     0,     0,     0,     0,     0,     0,
     130,     0,     0,     0,   116,     0,    95,     0,    94,    84,
      52,     0,     0,   119,    69,     0,    63,     0,     0,    55,
       0,     0,    16,     0,     0,    21,     0,    92,    76,     0,
       0,     0,   126,     0,     0,     0,   132,     0,     0,   127,
       0,   123,    12,   102,   101,     0,    93,    51,     0,     0,
       0,    61,    62,     0,     0,     0,     0,     0,    72,    75,
     169,   129,   138,   134,     0,     0,   136,   131,   135,    99,
      98,   100,     0,     0,     0,     0,    74,     0,     0,     0,
       0,   133,    97,     0,    57,    65,    56,     0,     0,     0,
     137,     0,    73,    13,     0,    15,     0,     0,    58,    14
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -165,  -165,  -165,   182,   -12,    -1,  -165,  -165,   192,   -14,
    -165,   -64,   -31,   -26,     5,  -165,  -165,   105,   -94,  -151,
      -5,  -165,    39,  -165,   -53,  -125,  -165,  -165,   -37,  -164,
    -111,  -165
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    31,   197,   113,    32,    33,   118,    25,
     125,   206,   207,   208,    26,    45,   132,   141,   262,   224,
      27,   130,   131,   187,   188,   189,   233,   239,   240,    84,
      85,    86
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      24,   226,   114,   114,   228,   229,    39,    40,   114,    38,
     115,   136,    41,   114,    43,   204,    46,   124,    49,   122,
      54,   137,   241,   250,    57,    50,   149,    58,    83,   278,
     123,   271,   128,   129,   119,   119,   149,   133,   222,    37,
     134,   150,   288,    29,    30,   289,   151,   230,   272,   279,
     196,   150,   184,    51,   185,   217,   218,   186,   219,    16,
     232,     1,   116,   116,   117,   117,   217,   218,   116,   254,
     117,    28,   268,   116,   205,   117,    44,   108,   109,   110,
     153,   111,   251,   212,   213,    34,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     124,   199,    35,   198,   241,   193,    36,   281,   299,   300,
      37,   201,   194,   202,   257,   257,   258,   323,    47,    42,
     138,   139,   140,   209,   211,   142,   147,   215,   143,   145,
     148,   220,   152,   154,   155,   225,   301,   225,   302,   303,
     225,   225,   306,   156,    83,   308,   157,   158,   183,   195,
     216,   121,   111,    83,   231,   243,     4,     5,     6,     7,
       8,   242,  -122,   244,   246,     4,     5,     6,     7,     8,
     321,   249,   252,   255,   256,   199,   263,   264,   269,   276,
     245,   238,   287,    15,  -121,   330,   270,   273,   274,   290,
     129,   277,    15,   286,    16,   280,   293,   305,   309,   310,
     314,   322,   320,    16,   126,   265,   223,   266,   225,    18,
     325,   331,   120,    19,   312,    20,   291,    21,    18,    22,
      23,   292,    19,   332,    20,   275,    21,   203,    22,    23,
     253,   307,   282,     0,     0,     0,   285,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   294,
     295,     0,   296,   297,   225,   225,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   238,     0,     0,   -78,    55,   313,     0,
      56,   -78,   315,    57,     0,   -78,   -78,   -78,   -78,   -78,
       0,     0,     0,     0,     0,     0,   -78,   -78,   -78,     0,
       0,   -78,   -78,   -78,     0,   -78,   327,   328,   329,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,     0,    16,     0,
     336,   -78,     0,   337,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,     0,   -78,   -78,     0,   -78,     0,
     -78,   -78,   -78,   -78,    59,   -78,     0,    60,  -160,  -160,
      57,     0,     0,     0,     0,     0,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,     0,  -160,  -160,     0,
       0,     0,     0,     0,     0,    16,     0,     0,     0,  -160,
    -160,   106,   107,   108,   109,   110,     0,   111,     0,     0,
       0,     0,   221,     0,    81,    60,    82,     0,    57,     0,
       0,     0,  -159,     0,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,     0,     0,    88,    89,     0,     0,
       0,     0,     0,    16,     0,   190,     0,     0,     4,     5,
       6,     7,     8,     0,     0,     0,     0,     0,     9,     0,
       0,     0,    81,    10,    82,    99,   100,    11,    12,     0,
    -159,     0,    13,     0,    14,    15,     0,   104,   105,   106,
     107,   108,   109,   110,     0,   111,    16,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
       0,    18,     0,     0,     0,    19,     0,    20,   191,    21,
     192,    22,    23,    52,     0,     0,     4,     5,     6,     7,
       8,     0,     0,     0,     0,     0,     9,     0,     0,     0,
       0,    10,     0,     0,     0,    11,    12,     0,     0,     0,
      13,     0,    14,    15,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    16,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    17,     0,     0,    18,
       0,     0,     0,    19,     0,    20,     0,    21,    53,    22,
      23,    48,     0,     0,     4,     5,     6,     7,     8,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,    10,
       0,     0,     0,    11,    12,     0,     0,     0,    13,     0,
      14,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,    18,     0,     0,
       0,    19,     0,    20,     0,    21,   210,    22,    23,     4,
       5,     6,     7,     8,     0,     0,     0,     0,     0,     9,
       0,     0,     0,     0,    10,     0,     0,     0,    11,    12,
       0,     0,     0,    13,     0,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,    18,     0,     0,     0,    19,     0,    20,     0,
      21,   214,    22,    23,     4,     5,     6,     7,     8,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,    10,
       0,     0,     0,    11,    12,     0,     0,     0,    13,     0,
      14,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,    18,     0,     0,
       0,    19,     0,    20,     0,    21,     0,    22,    23,     4,
       5,     6,     7,     8,     0,     0,     0,     0,     0,     9,
       0,     0,     0,     0,    10,     0,     0,     0,    11,    12,
       0,     0,     0,    13,     0,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,    18,     0,     0,   127,    19,     0,    20,     0,
      21,     0,    22,    23,     4,     5,     6,     7,     8,     0,
       0,     0,     0,     0,     9,     0,     0,     0,     0,    10,
       0,     0,     0,    11,    12,     0,     0,     0,    13,     0,
      14,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,    18,     0,     0,
       0,    19,     0,    20,     0,    21,   284,    22,    23,     4,
       5,     6,     7,     8,     0,     0,     0,     0,     0,     9,
       0,     0,     0,     0,    10,     0,     0,     0,    11,    12,
       0,     0,     0,    13,     0,    14,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,     0,    18,     0,     0,     0,    19,     0,    20,     0,
      21,     0,    22,    23,     4,     5,     6,     7,     8,     0,
       0,     0,     0,     0,   121,     0,     0,     0,     0,    10,
       0,     0,     0,    11,    12,     0,     0,     0,    13,     0,
      14,    15,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,    18,     0,     0,
       0,    19,     0,    20,     0,    21,   234,    22,    23,   235,
       0,     0,    57,     0,     0,     0,     0,     0,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,    87,    88,    89,    90,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   236,     0,   237,    91,
      92,     0,     0,     0,     0,     0,    93,    94,    95,    96,
      97,    98,    99,   100,     0,     0,    87,    88,    89,    90,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,    91,    92,     0,     0,   247,     0,   248,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
      87,    88,    89,    90,     0,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,    91,    92,     0,
       0,     0,     0,   146,    93,    94,    95,    96,    97,    98,
      99,   100,     0,     0,    87,    88,    89,    90,     0,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
     111,    91,    92,     0,     0,     0,     0,   267,    93,    94,
      95,    96,    97,    98,    99,   100,     0,     0,    87,    88,
      89,    90,     0,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,   111,    91,    92,     0,     0,     0,
       0,   283,    93,    94,    95,    96,    97,    98,    99,   100,
       0,     0,    87,    88,    89,    90,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,    91,
      92,     0,     0,     0,     0,   311,    93,    94,    95,    96,
      97,    98,    99,   100,     0,     0,     0,     0,     0,     0,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,   334,     0,   335,    87,    88,    89,    90,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    91,    92,     0,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
      87,    88,    89,    90,     0,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,    91,    92,   144,
       0,     0,     0,     0,    93,    94,    95,    96,    97,    98,
      99,   100,     0,     0,    87,    88,    89,    90,     0,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
     111,    91,    92,   227,     0,     0,     0,     0,    93,    94,
      95,    96,    97,    98,    99,   100,     0,     0,    87,    88,
      89,    90,     0,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,   111,    91,    92,   304,     0,     0,
       0,     0,    93,    94,    95,    96,    97,    98,    99,   100,
       0,     0,    87,    88,    89,    90,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,    91,
      92,   333,     0,     0,     0,     0,    93,    94,    95,    96,
      97,    98,    99,   100,     0,     0,    87,    88,    89,    90,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,    91,    92,   339,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
       0,    87,    88,    89,    90,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,   112,    91,    92,
       0,     0,     0,     0,     0,    93,    94,    95,    96,    97,
      98,    99,   100,     0,     0,     0,    87,    88,    89,    90,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       0,   111,   200,    91,    92,     0,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
       0,    87,    88,    89,    90,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,   259,    91,    92,
       0,     0,     0,     0,     0,    93,    94,    95,    96,    97,
      98,    99,   100,     0,     0,     0,    87,    88,    89,    90,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       0,   111,   318,    91,    92,     0,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
       0,    87,    88,    89,    90,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,   319,    91,    92,
       0,     0,     0,     0,     0,    93,    94,    95,    96,    97,
      98,    99,   100,     0,     0,     0,    87,    88,    89,    90,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       0,   111,   324,    91,    92,     0,     0,     0,     0,     0,
      93,    94,    95,    96,    97,    98,    99,   100,     0,     0,
       0,    87,    88,    89,    90,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,   111,   326,    91,    92,
       0,     0,     0,     0,     0,    93,    94,    95,    96,    97,
      98,    99,   100,     0,     0,     0,     0,     0,     0,     0,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       0,   111,   338,    87,    88,    89,    90,     0,     0,     0,
       0,     0,     0,     0,     0,   260,   261,     0,     0,     0,
      91,    92,     0,     0,     0,     0,     0,    93,    94,    95,
      96,    97,    98,    99,   100,     0,     0,     0,     0,     0,
       0,     0,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,     0,   111,    87,    88,    89,    90,     0,     0,
       0,     0,     0,     0,     0,   135,     0,     0,     0,     0,
       0,    91,    92,     0,     0,     0,     0,     0,    93,    94,
      95,    96,    97,    98,    99,   100,     0,     0,    87,    88,
      89,    90,     0,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,   111,    91,    92,     0,     0,     0,
       0,     0,    93,    94,    95,    96,    97,    98,    99,   100,
       0,     0,     0,     0,   298,     0,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,    87,
      88,    89,    90,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   316,    91,    92,     0,     0,
       0,     0,     0,    93,    94,    95,    96,    97,    98,    99,
     100,     0,     0,     0,     0,     0,     0,     0,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,     0,   111,
      87,    88,    89,    90,     0,     0,     0,     0,     0,     0,
       0,   317,     0,     0,     0,     0,     0,    91,    92,     0,
       0,     0,     0,     0,    93,    94,    95,    96,    97,    98,
      99,   100,     0,     0,    87,    88,    89,    90,     0,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
     111,    91,    92,     0,     0,     0,     0,     0,    93,    94,
      95,    96,    97,    98,    99,   100,     0,     0,    87,    88,
      89,    90,     0,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,     0,   111,    91,    92,     0,     0,     0,
       0,     0,    93,    94,    95,    96,    97,    98,    99,   100,
       0,     0,  -160,    88,    89,     0,     0,     0,     0,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,    91,
      92,     0,     0,     0,    88,    89,  -160,  -160,  -160,  -160,
    -160,  -160,    99,   100,     0,     0,     0,     0,     0,     0,
      91,     0,     0,  -160,   104,   105,   106,   107,   108,   109,
     110,     0,   111,    99,   100,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   104,   105,   106,   107,   108,
     109,   110,     0,   111
};

static const yytype_int16 yycheck[] =
{
       1,   152,     5,     5,   155,   156,    11,    12,     5,    10,
      13,    13,    13,     5,     1,     4,    17,    31,    19,    31,
      21,    13,   186,     4,     7,     4,    49,    22,    23,    49,
      31,    49,    33,    34,    29,    30,    49,    61,   149,     4,
      64,    64,    61,    17,    18,    64,    69,   158,    66,    69,
      15,    64,    63,    32,    65,    43,    44,    68,    46,    42,
     185,    16,    65,    65,    67,    67,    43,    44,    65,    46,
      67,     0,   223,    65,    63,    67,    63,    55,    56,    57,
      81,    59,    63,   136,   137,    61,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     124,   125,    59,   125,   278,   116,    42,   242,   269,   270,
       4,    60,   117,    62,    60,    60,    62,    62,     4,    63,
      29,     4,     4,   134,   135,    65,     1,   138,    62,    66,
      59,   142,    64,     4,    64,   150,   271,   152,   273,   274,
     155,   156,   277,    64,   149,   280,    69,    49,    59,    13,
      48,    14,    59,   158,     4,    48,     4,     5,     6,     7,
       8,    41,    41,    66,    59,     4,     5,     6,     7,     8,
     305,    59,     4,    65,     4,   199,    61,    61,    48,     4,
     191,   186,     4,    31,    41,   320,    64,    64,    64,    63,
     201,    64,    31,    59,    42,    64,    64,    64,    59,    59,
       4,    59,    64,    42,    32,   216,    54,   218,   223,    57,
      66,    64,    30,    61,   288,    63,   257,    65,    57,    67,
      68,   257,    61,   327,    63,   236,    65,   132,    67,    68,
     201,   278,   243,    -1,    -1,    -1,   247,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   260,
     261,    -1,   263,   264,   269,   270,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   278,    -1,    -1,     0,     1,   289,    -1,
       4,     5,   293,     7,    -1,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,    -1,
      -1,    25,    26,    27,    -1,    29,   317,   318,   319,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    42,    -1,
     331,    45,    -1,   334,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    -1,    62,    -1,
      64,    65,    66,    67,     1,    69,    -1,     4,    10,    11,
       7,    -1,    -1,    -1,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    -1,    -1,
      -1,    -1,     1,    -1,    61,     4,    63,    -1,     7,    -1,
      -1,    -1,    69,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    10,    11,    -1,    -1,
      -1,    -1,    -1,    42,    -1,     1,    -1,    -1,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      -1,    -1,    61,    19,    63,    39,    40,    23,    24,    -1,
      69,    -1,    28,    -1,    30,    31,    -1,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    -1,    -1,    -1,    61,    -1,    63,    64,    65,
      66,    67,    68,     1,    -1,    -1,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    -1,    23,    24,    -1,    -1,    -1,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      -1,    -1,    -1,    61,    -1,    63,    -1,    65,    66,    67,
      68,     1,    -1,    -1,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    -1,    -1,
      -1,    61,    -1,    63,    -1,    65,     1,    67,    68,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    14,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,
      -1,    -1,    -1,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    -1,    -1,    -1,    61,    -1,    63,    -1,
      65,     1,    67,    68,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    -1,    -1,
      -1,    61,    -1,    63,    -1,    65,    -1,    67,    68,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    14,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,
      -1,    -1,    -1,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    -1,    -1,    60,    61,    -1,    63,    -1,
      65,    -1,    67,    68,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    -1,    -1,
      -1,    61,    -1,    63,    -1,    65,    66,    67,    68,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    14,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,
      -1,    -1,    -1,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    -1,    -1,    -1,    61,    -1,    63,    -1,
      65,    -1,    67,    68,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    -1,    -1,
      -1,    61,    -1,    63,    -1,    65,     1,    67,    68,     4,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    26,
      27,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,     9,    10,    11,    12,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    26,    27,    -1,    -1,    64,    -1,    66,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
       9,    10,    11,    12,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    26,    27,    -1,
      -1,    -1,    -1,    66,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,     9,    10,    11,    12,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    26,    27,    -1,    -1,    -1,    -1,    66,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,     9,    10,
      11,    12,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    26,    27,    -1,    -1,    -1,
      -1,    66,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,     9,    10,    11,    12,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    26,
      27,    -1,    -1,    -1,    -1,    66,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,    62,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    -1,    -1,    -1,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
       9,    10,    11,    12,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    26,    27,    62,
      -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,     9,    10,    11,    12,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    26,    27,    62,    -1,    -1,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,     9,    10,
      11,    12,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    26,    27,    62,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,     9,    10,    11,    12,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    26,
      27,    62,    -1,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,     9,    10,    11,    12,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    26,    27,    62,    -1,    -1,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,     9,    10,    11,    12,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    26,    27,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,     9,    10,    11,    12,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    26,    27,    -1,    -1,    -1,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,     9,    10,    11,    12,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    26,    27,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,     9,    10,    11,    12,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    26,    27,    -1,    -1,    -1,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,     9,    10,    11,    12,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    26,    27,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,     9,    10,    11,    12,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    26,    27,    -1,    -1,    -1,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,     9,    10,    11,    12,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,    26,    27,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,     9,    10,    11,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    21,    22,    -1,    -1,    -1,
      26,    27,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,     9,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    26,    27,    -1,    -1,    -1,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,     9,    10,
      11,    12,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    26,    27,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,     9,
      10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    26,    27,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    -1,    26,    27,    -1,
      -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,     9,    10,    11,    12,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    26,    27,    -1,    -1,    -1,    -1,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,     9,    10,
      11,    12,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    26,    27,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    26,
      27,    -1,    -1,    -1,    10,    11,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    52,    53,    54,    55,
      56,    57,    -1,    59
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    16,    71,    72,     4,     5,     6,     7,     8,    14,
      19,    23,    24,    28,    30,    31,    42,    54,    57,    61,
      63,    65,    67,    68,    75,    79,    84,    90,     0,    17,
      18,    73,    76,    77,    61,    59,    42,     4,    75,    90,
      90,    75,    63,     1,    63,    85,    75,     4,     1,    75,
       4,    32,     1,    66,    75,     1,     4,     7,    84,     1,
       4,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    61,    63,    84,    99,   100,   101,     9,    10,    11,
      12,    26,    27,    33,    34,    35,    36,    37,    38,    39,
      40,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    59,    60,    75,     5,    13,    65,    67,    78,    84,
      78,    14,    74,    75,    79,    80,    73,    60,    75,    75,
      91,    92,    86,    61,    64,    20,    13,    13,    29,     4,
       4,    87,    65,    62,    62,    66,    66,     1,    59,    49,
      64,    69,    64,    75,     4,    64,    64,    69,    49,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    59,    63,    65,    68,    93,    94,    95,
       1,    64,    66,    75,    84,    13,    15,    74,    74,    79,
      60,    60,    62,    87,     4,    63,    81,    82,    83,    75,
       1,    75,    94,    94,     1,    75,    48,    43,    44,    46,
      75,     1,   100,    54,    89,    90,    89,    62,    89,    89,
     100,     4,    95,    96,     1,     4,    61,    63,    84,    97,
      98,    99,    41,    48,    66,    75,    59,    64,    66,    59,
       4,    63,     4,    92,    46,    65,     4,    60,    62,    60,
      21,    22,    88,    61,    61,    75,    75,    66,    89,    48,
      64,    49,    66,    64,    64,    75,     4,    64,    49,    69,
      64,    95,    75,    66,    66,    75,    59,     4,    61,    64,
      63,    82,    83,    64,    75,    75,    75,    75,    45,    89,
      89,    95,    95,    95,    62,    64,    95,    98,    95,    59,
      59,    66,    81,    75,     4,    75,    25,    20,    60,    60,
      64,    95,    59,    62,    60,    66,    60,    75,    75,    75,
      95,    64,    88,    62,    60,    62,    75,    75,    60,    62
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    71,    72,    72,    73,    73,    74,    74,
      74,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    76,
      76,    77,    77,    77,    78,    79,    79,    80,    80,    81,
      81,    81,    81,    82,    82,    83,    85,    84,    86,    84,
      87,    87,    87,    88,    88,    89,    89,    89,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    91,    91,
      92,    93,    93,    94,    94,    95,    95,    95,    96,    96,
      97,    97,    98,    98,    98,    98,    98,    98,    98,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,   100,
     100,   100,   100,   101,   101,   101,   101,   101,   101,   101,
     101
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     3,     0,     3,     0,     2,     0,     2,
       2,     2,     5,     9,    11,     9,     5,     4,     4,     2,
       4,     5,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       3,     5,     4,     2,     1,     5,     8,     6,     9,     1,
       1,     3,     3,     2,     1,     5,     0,     4,     0,     5,
       0,     2,     4,     5,     3,     3,     2,     1,     1,     1,
       3,     2,     3,     2,     4,     3,     2,     1,     3,     2,
       2,     3,     5,     5,     4,     4,     3,     7,     6,     6,
       6,     5,     5,     1,     1,     1,     3,     3,     2,     3,
       2,     2,     1,     4,     3,     3,     4,     3,     1,     3,
       1,     3,     1,     3,     1,     2,     3,     3,     1,     3,
       1,     3,     2,     4,     3,     3,     3,     5,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     3,     3,     3,     3,     3,     1,     2,     1,     5,
       3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, answer, errors, locations, lexer_param_ptr, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, answer, errors, locations, lexer_param_ptr); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (answer);
  YYUSE (errors);
  YYUSE (locations);
  YYUSE (lexer_param_ptr);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, answer, errors, locations, lexer_param_ptr);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , answer, errors, locations, lexer_param_ptr);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, answer, errors, locations, lexer_param_ptr); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (answer);
  YYUSE (errors);
  YYUSE (locations);
  YYUSE (lexer_param_ptr);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 4: /* IDENT  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 1940 "src/parser.c" /* yacc.c:1257  */
        break;

    case 5: /* FIELD  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 1946 "src/parser.c" /* yacc.c:1257  */
        break;

    case 6: /* LITERAL  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 1952 "src/parser.c" /* yacc.c:1257  */
        break;

    case 7: /* FORMAT  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 1958 "src/parser.c" /* yacc.c:1257  */
        break;

    case 43: /* QQSTRING_TEXT  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 1964 "src/parser.c" /* yacc.c:1257  */
        break;

    case 72: /* Module  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 1970 "src/parser.c" /* yacc.c:1257  */
        break;

    case 73: /* Imports  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 1976 "src/parser.c" /* yacc.c:1257  */
        break;

    case 74: /* FuncDefs  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 1982 "src/parser.c" /* yacc.c:1257  */
        break;

    case 75: /* Exp  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 1988 "src/parser.c" /* yacc.c:1257  */
        break;

    case 76: /* Import  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 1994 "src/parser.c" /* yacc.c:1257  */
        break;

    case 77: /* ImportWhat  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2000 "src/parser.c" /* yacc.c:1257  */
        break;

    case 78: /* ImportFrom  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2006 "src/parser.c" /* yacc.c:1257  */
        break;

    case 79: /* FuncDef  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2012 "src/parser.c" /* yacc.c:1257  */
        break;

    case 80: /* ExportOnlyFuncDef  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2018 "src/parser.c" /* yacc.c:1257  */
        break;

    case 81: /* Params  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2024 "src/parser.c" /* yacc.c:1257  */
        break;

    case 82: /* Param  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2030 "src/parser.c" /* yacc.c:1257  */
        break;

    case 83: /* VarargParam  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2036 "src/parser.c" /* yacc.c:1257  */
        break;

    case 84: /* String  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2042 "src/parser.c" /* yacc.c:1257  */
        break;

    case 87: /* QQString  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2048 "src/parser.c" /* yacc.c:1257  */
        break;

    case 88: /* ElseBody  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2054 "src/parser.c" /* yacc.c:1257  */
        break;

    case 89: /* ExpD  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2060 "src/parser.c" /* yacc.c:1257  */
        break;

    case 90: /* Term  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2066 "src/parser.c" /* yacc.c:1257  */
        break;

    case 91: /* Args  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2072 "src/parser.c" /* yacc.c:1257  */
        break;

    case 92: /* Arg  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2078 "src/parser.c" /* yacc.c:1257  */
        break;

    case 93: /* RepPatterns  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2084 "src/parser.c" /* yacc.c:1257  */
        break;

    case 94: /* Patterns  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2090 "src/parser.c" /* yacc.c:1257  */
        break;

    case 95: /* Pattern  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2096 "src/parser.c" /* yacc.c:1257  */
        break;

    case 96: /* ArrayPats  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2102 "src/parser.c" /* yacc.c:1257  */
        break;

    case 97: /* ObjPats  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2108 "src/parser.c" /* yacc.c:1257  */
        break;

    case 98: /* ObjPat  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2114 "src/parser.c" /* yacc.c:1257  */
        break;

    case 99: /* Keyword  */
#line 36 "src/parser.y" /* yacc.c:1257  */
      { jv_free(((*yyvaluep).literal)); }
#line 2120 "src/parser.c" /* yacc.c:1257  */
        break;

    case 100: /* MkDict  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2126 "src/parser.c" /* yacc.c:1257  */
        break;

    case 101: /* MkDictPair  */
#line 37 "src/parser.y" /* yacc.c:1257  */
      { block_free(((*yyvaluep).blk)); }
#line 2132 "src/parser.c" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (block* answer, int* errors, struct locfile* locations, struct lexer_param* lexer_param_ptr)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, answer, errors, locations, lexer_param_ptr);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 301 "src/parser.y" /* yacc.c:1646  */
    {
  (yyvsp[0].blk) = block_link_unbounds((yyvsp[0].blk), NULL, NULL);
  *answer = BLOCK((yyvsp[-2].blk), (yyvsp[-1].blk), gen_op_simple(TOP), (yyvsp[0].blk));
}
#line 2429 "src/parser.c" /* yacc.c:1646  */
    break;

  case 3:
#line 305 "src/parser.y" /* yacc.c:1646  */
    {
  *answer = BLOCK((yyvsp[-2].blk), (yyvsp[-1].blk), (yyvsp[0].blk));
}
#line 2437 "src/parser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 310 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_noop();
}
#line 2445 "src/parser.c" /* yacc.c:1646  */
    break;

  case 5:
#line 313 "src/parser.y" /* yacc.c:1646  */
    {
  if (!block_is_const((yyvsp[-1].blk))) {
    FAIL((yyloc), "Module metadata must be constant");
    (yyval.blk) = gen_noop();
    block_free((yyvsp[-1].blk));
  } else {
    (yyval.blk) = gen_module((yyvsp[-1].blk));
  }
}
#line 2459 "src/parser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 324 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_noop();
}
#line 2467 "src/parser.c" /* yacc.c:1646  */
    break;

  case 7:
#line 327 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-1].blk), (yyvsp[0].blk));
}
#line 2475 "src/parser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 332 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_noop();
}
#line 2483 "src/parser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 335 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = block_bind((yyvsp[-1].blk), (yyvsp[0].blk), OP_IS_CALL_PSEUDO);
}
#line 2491 "src/parser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 338 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-1].blk), (yyvsp[0].blk));
}
#line 2499 "src/parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 343 "src/parser.y" /* yacc.c:1646  */
    {
  (yyvsp[0].blk) = block_link_unbounds((yyvsp[0].blk), NULL, NULL);
  (yyval.blk) = block_bind_referenced((yyvsp[-1].blk), (yyvsp[0].blk), OP_IS_CALL_PSEUDO);
}
#line 2508 "src/parser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 348 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_destructure((yyvsp[-4].blk), (yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2516 "src/parser.c" /* yacc.c:1646  */
    break;

  case 13:
#line 351 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_reduce((yyvsp[-7].blk), (yyvsp[-5].blk), (yyvsp[-3].blk), (yyvsp[-1].blk));
}
#line 2524 "src/parser.c" /* yacc.c:1646  */
    break;

  case 14:
#line 355 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_foreach((yyvsp[-9].blk), (yyvsp[-7].blk), (yyvsp[-5].blk), (yyvsp[-3].blk), (yyvsp[-1].blk));
}
#line 2532 "src/parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 359 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_foreach((yyvsp[-7].blk), (yyvsp[-5].blk), (yyvsp[-3].blk), (yyvsp[-1].blk), gen_noop());
}
#line 2540 "src/parser.c" /* yacc.c:1646  */
    break;

  case 16:
#line 363 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_cond((yyvsp[-3].blk), (yyvsp[-1].blk), (yyvsp[0].blk));
}
#line 2548 "src/parser.c" /* yacc.c:1646  */
    break;

  case 17:
#line 366 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "Possibly unterminated 'if' statement");
  (yyval.blk) = (yyvsp[-2].blk);
}
#line 2557 "src/parser.c" /* yacc.c:1646  */
    break;

  case 18:
#line 371 "src/parser.y" /* yacc.c:1646  */
    {
  //$$ = BLOCK(gen_op_target(FORK_OPT, $2), $2, $4);
  (yyval.blk) = gen_try((yyvsp[-2].blk), gen_try_handler((yyvsp[0].blk)));
}
#line 2566 "src/parser.c" /* yacc.c:1646  */
    break;

  case 19:
#line 375 "src/parser.y" /* yacc.c:1646  */
    {
  //$$ = BLOCK(gen_op_target(FORK_OPT, $2), $2, gen_op_simple(BACKTRACK));
  (yyval.blk) = gen_try((yyvsp[0].blk), gen_op_simple(BACKTRACK));
}
#line 2575 "src/parser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 379 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "Possibly unterminated 'try' statement");
  (yyval.blk) = (yyvsp[-2].blk);
}
#line 2584 "src/parser.c" /* yacc.c:1646  */
    break;

  case 21:
#line 384 "src/parser.y" /* yacc.c:1646  */
    {
  jv v = jv_string_fmt("*label-%s", jv_string_value((yyvsp[-2].literal)));
  (yyval.blk) = gen_location((yyloc), locations, gen_label(jv_string_value(v), (yyvsp[0].blk)));
  jv_free((yyvsp[-2].literal));
  jv_free(v);
}
#line 2595 "src/parser.c" /* yacc.c:1646  */
    break;

  case 22:
#line 391 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_try((yyvsp[-1].blk), gen_op_simple(BACKTRACK));
}
#line 2603 "src/parser.c" /* yacc.c:1646  */
    break;

  case 23:
#line 395 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_call("_assign", BLOCK(gen_lambda((yyvsp[-2].blk)), gen_lambda((yyvsp[0].blk))));
}
#line 2611 "src/parser.c" /* yacc.c:1646  */
    break;

  case 24:
#line 399 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_or((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2619 "src/parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 403 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_and((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2627 "src/parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 407 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_definedor((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2635 "src/parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 411 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_definedor_assign((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2643 "src/parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 415 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_call("_modify", BLOCK(gen_lambda((yyvsp[-2].blk)), gen_lambda((yyvsp[0].blk))));
}
#line 2651 "src/parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 419 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = block_join((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2659 "src/parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 423 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_both((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2667 "src/parser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 427 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '+');
}
#line 2675 "src/parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 431 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_update((yyvsp[-2].blk), (yyvsp[0].blk), '+');
}
#line 2683 "src/parser.c" /* yacc.c:1646  */
    break;

  case 33:
#line 435 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[0].blk), gen_call("_negate", gen_noop()));
}
#line 2691 "src/parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 439 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '-');
}
#line 2699 "src/parser.c" /* yacc.c:1646  */
    break;

  case 35:
#line 443 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_update((yyvsp[-2].blk), (yyvsp[0].blk), '-');
}
#line 2707 "src/parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 447 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '*');
}
#line 2715 "src/parser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 451 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_update((yyvsp[-2].blk), (yyvsp[0].blk), '*');
}
#line 2723 "src/parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 455 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '/');
  if (block_is_const_inf((yyval.blk)))
    FAIL((yyloc), "Division by zero?");
}
#line 2733 "src/parser.c" /* yacc.c:1646  */
    break;

  case 39:
#line 461 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '%');
  if (block_is_const_inf((yyval.blk)))
    FAIL((yyloc), "Remainder by zero?");
}
#line 2743 "src/parser.c" /* yacc.c:1646  */
    break;

  case 40:
#line 467 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_update((yyvsp[-2].blk), (yyvsp[0].blk), '/');
}
#line 2751 "src/parser.c" /* yacc.c:1646  */
    break;

  case 41:
#line 471 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_update((yyvsp[-2].blk), (yyvsp[0].blk), '%');
}
#line 2759 "src/parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 475 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), EQ);
}
#line 2767 "src/parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 479 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), NEQ);
}
#line 2775 "src/parser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 483 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '<');
}
#line 2783 "src/parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 487 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), '>');
}
#line 2791 "src/parser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 491 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), LESSEQ);
}
#line 2799 "src/parser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 495 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-2].blk), (yyvsp[0].blk), GREATEREQ);
}
#line 2807 "src/parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 499 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 2815 "src/parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 504 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[-1].blk);
}
#line 2823 "src/parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 507 "src/parser.y" /* yacc.c:1646  */
    {
  if (!block_is_const((yyvsp[-1].blk))) {
    FAIL((yyloc), "Module metadata must be constant");
    (yyval.blk) = gen_noop();
    block_free((yyvsp[-2].blk));
    block_free((yyvsp[-1].blk));
  } else if (block_const_kind((yyvsp[-1].blk)) != JV_KIND_OBJECT) {
    FAIL((yyloc), "Module metadata must be an object");
    (yyval.blk) = gen_noop();
    block_free((yyvsp[-2].blk));
    block_free((yyvsp[-1].blk));
  } else {
    (yyval.blk) = gen_import_meta((yyvsp[-2].blk), (yyvsp[-1].blk));
  }
}
#line 2843 "src/parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 524 "src/parser.y" /* yacc.c:1646  */
    {
  jv v = block_const((yyvsp[-3].blk));
  // XXX Make gen_import take only blocks and the int is_data so we
  // don't have to free so much stuff here
  (yyval.blk) = gen_import(jv_string_value(v), jv_string_value((yyvsp[0].literal)), 1);
  block_free((yyvsp[-3].blk));
  jv_free((yyvsp[0].literal));
  jv_free(v);
}
#line 2857 "src/parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 533 "src/parser.y" /* yacc.c:1646  */
    {
  jv v = block_const((yyvsp[-2].blk));
  (yyval.blk) = gen_import(jv_string_value(v), jv_string_value((yyvsp[0].literal)), 0);
  block_free((yyvsp[-2].blk));
  jv_free((yyvsp[0].literal));
  jv_free(v);
}
#line 2869 "src/parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 540 "src/parser.y" /* yacc.c:1646  */
    {
  jv v = block_const((yyvsp[0].blk));
  (yyval.blk) = gen_import(jv_string_value(v), NULL, 0);
  block_free((yyvsp[0].blk));
  jv_free(v);
}
#line 2880 "src/parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 548 "src/parser.y" /* yacc.c:1646  */
    {
  if (!block_is_const((yyvsp[0].blk))) {
    FAIL((yyloc), "Import path must be constant");
    (yyval.blk) = gen_const(jv_string(""));
    block_free((yyvsp[0].blk));
  } else {
    (yyval.blk) = (yyvsp[0].blk);
  }
}
#line 2894 "src/parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 559 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_function(jv_string_value((yyvsp[-3].literal)), gen_noop(), (yyvsp[-1].blk));
  jv_free((yyvsp[-3].literal));
}
#line 2903 "src/parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 564 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_function(jv_string_value((yyvsp[-6].literal)), (yyvsp[-4].blk), (yyvsp[-1].blk));
  jv_free((yyvsp[-6].literal));
}
#line 2912 "src/parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 570 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_function(jv_string_value((yyvsp[-3].literal)), gen_noop(), (yyvsp[-1].blk));
  jv_free((yyvsp[-3].literal));
}
#line 2921 "src/parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 575 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_function(jv_string_value((yyvsp[-6].literal)), (yyvsp[-4].blk), (yyvsp[-1].blk));
  jv_free((yyvsp[-6].literal));
}
#line 2930 "src/parser.c" /* yacc.c:1646  */
    break;

  case 59:
#line 582 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 2938 "src/parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 585 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 2946 "src/parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 588 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2954 "src/parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 591 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 2962 "src/parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 596 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_param_regular(jv_string_value((yyvsp[0].literal)));
  jv_free((yyvsp[0].literal));
}
#line 2971 "src/parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 601 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_param(jv_string_value((yyvsp[0].literal)));
  jv_free((yyvsp[0].literal));
}
#line 2980 "src/parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 607 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_param_vararg(jv_string_value((yyvsp[-4].literal)), jv_string_value((yyvsp[-1].literal)));
  jv_free((yyvsp[-1].literal));
  jv_free((yyvsp[-4].literal));
}
#line 2990 "src/parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 615 "src/parser.y" /* yacc.c:1646  */
    { (yyval.literal) = jv_string("text"); }
#line 2996 "src/parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 615 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[-1].blk);
  jv_free((yyvsp[-2].literal));
}
#line 3005 "src/parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 619 "src/parser.y" /* yacc.c:1646  */
    { (yyval.literal) = (yyvsp[-1].literal); }
#line 3011 "src/parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 619 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[-1].blk);
  jv_free((yyvsp[-2].literal));
}
#line 3020 "src/parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 626 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_const(jv_string(""));
}
#line 3028 "src/parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 629 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-1].blk), gen_const((yyvsp[0].literal)), '+');
}
#line 3036 "src/parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 632 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_binop((yyvsp[-3].blk), gen_format((yyvsp[-1].blk), jv_copy((yyvsp[-4].literal))), '+');
}
#line 3044 "src/parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 638 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_cond((yyvsp[-3].blk), (yyvsp[-1].blk), (yyvsp[0].blk));
}
#line 3052 "src/parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 641 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[-1].blk);
}
#line 3060 "src/parser.c" /* yacc.c:1646  */
    break;

  case 75:
#line 646 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = block_join((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3068 "src/parser.c" /* yacc.c:1646  */
    break;

  case 76:
#line 649 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[0].blk), gen_call("_negate", gen_noop()));
}
#line 3076 "src/parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 652 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3084 "src/parser.c" /* yacc.c:1646  */
    break;

  case 78:
#line 658 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_noop();
}
#line 3092 "src/parser.c" /* yacc.c:1646  */
    break;

  case 79:
#line 661 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_call("recurse", gen_noop());
}
#line 3100 "src/parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 664 "src/parser.y" /* yacc.c:1646  */
    {
  jv v = jv_string_fmt("*label-%s", jv_string_value((yyvsp[0].literal)));     // impossible symbol
  (yyval.blk) = gen_location((yyloc), locations,
                    BLOCK(gen_op_unbound(LOADV, jv_string_value(v)),
                    gen_call("error", gen_noop())));
  jv_free(v);
  jv_free((yyvsp[0].literal));
}
#line 3113 "src/parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 672 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "break requires a label to break to");
  (yyval.blk) = gen_noop();
}
#line 3122 "src/parser.c" /* yacc.c:1646  */
    break;

  case 82:
#line 676 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index_opt((yyvsp[-2].blk), gen_const((yyvsp[-1].literal)));
}
#line 3130 "src/parser.c" /* yacc.c:1646  */
    break;

  case 83:
#line 679 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index_opt(gen_noop(), gen_const((yyvsp[-1].literal)));
}
#line 3138 "src/parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 682 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index_opt((yyvsp[-3].blk), (yyvsp[-1].blk));
}
#line 3146 "src/parser.c" /* yacc.c:1646  */
    break;

  case 85:
#line 685 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index_opt(gen_noop(), (yyvsp[-1].blk));
}
#line 3154 "src/parser.c" /* yacc.c:1646  */
    break;

  case 86:
#line 688 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index((yyvsp[-1].blk), gen_const((yyvsp[0].literal)));
}
#line 3162 "src/parser.c" /* yacc.c:1646  */
    break;

  case 87:
#line 691 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index(gen_noop(), gen_const((yyvsp[0].literal)));
}
#line 3170 "src/parser.c" /* yacc.c:1646  */
    break;

  case 88:
#line 694 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3178 "src/parser.c" /* yacc.c:1646  */
    break;

  case 89:
#line 697 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index(gen_noop(), (yyvsp[0].blk));
}
#line 3186 "src/parser.c" /* yacc.c:1646  */
    break;

  case 90:
#line 700 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "try .[\"field\"] instead of .field for unusually named fields");
  (yyval.blk) = gen_noop();
}
#line 3195 "src/parser.c" /* yacc.c:1646  */
    break;

  case 91:
#line 704 "src/parser.y" /* yacc.c:1646  */
    {
  jv_free((yyvsp[-1].literal));
  FAIL((yyloc), "try .[\"field\"] instead of .field for unusually named fields");
  (yyval.blk) = gen_noop();
}
#line 3205 "src/parser.c" /* yacc.c:1646  */
    break;

  case 92:
#line 709 "src/parser.y" /* yacc.c:1646  */
    {
  /* varargs arg deref */
  (yyval.blk) = gen_location((yyloc), locations, gen_call_varargs(jv_string_value((yyvsp[-3].literal)), (yyvsp[-1].blk)));
  jv_free((yyvsp[-3].literal));
}
#line 3215 "src/parser.c" /* yacc.c:1646  */
    break;

  case 93:
#line 715 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index_opt((yyvsp[-4].blk), (yyvsp[-2].blk));
}
#line 3223 "src/parser.c" /* yacc.c:1646  */
    break;

  case 94:
#line 718 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_index((yyvsp[-3].blk), (yyvsp[-1].blk));
}
#line 3231 "src/parser.c" /* yacc.c:1646  */
    break;

  case 95:
#line 721 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = block_join((yyvsp[-3].blk), gen_op_simple(EACH_OPT));
}
#line 3239 "src/parser.c" /* yacc.c:1646  */
    break;

  case 96:
#line 724 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = block_join((yyvsp[-2].blk), gen_op_simple(EACH));
}
#line 3247 "src/parser.c" /* yacc.c:1646  */
    break;

  case 97:
#line 727 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-6].blk), (yyvsp[-4].blk), (yyvsp[-2].blk), INDEX_OPT);
}
#line 3255 "src/parser.c" /* yacc.c:1646  */
    break;

  case 98:
#line 730 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-5].blk), (yyvsp[-3].blk), gen_const(jv_null()), INDEX_OPT);
}
#line 3263 "src/parser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 733 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-5].blk), gen_const(jv_null()), (yyvsp[-2].blk), INDEX_OPT);
}
#line 3271 "src/parser.c" /* yacc.c:1646  */
    break;

  case 100:
#line 736 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-5].blk), (yyvsp[-3].blk), (yyvsp[-1].blk), INDEX);
}
#line 3279 "src/parser.c" /* yacc.c:1646  */
    break;

  case 101:
#line 739 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-4].blk), (yyvsp[-2].blk), gen_const(jv_null()), INDEX);
}
#line 3287 "src/parser.c" /* yacc.c:1646  */
    break;

  case 102:
#line 742 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_slice_index((yyvsp[-4].blk), gen_const(jv_null()), (yyvsp[-1].blk), INDEX);
}
#line 3295 "src/parser.c" /* yacc.c:1646  */
    break;

  case 103:
#line 745 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_const((yyvsp[0].literal));
}
#line 3303 "src/parser.c" /* yacc.c:1646  */
    break;

  case 104:
#line 748 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3311 "src/parser.c" /* yacc.c:1646  */
    break;

  case 105:
#line 751 "src/parser.y" /* yacc.c:1646  */
    {
#if 0
  (yyval.blk) = gen_location((yyloc), locations, gen_format(gen_noop(), (yyvsp[0].literal)));
#else
  (yyval.blk) = gen_format(gen_noop(), (yyvsp[0].literal));
#endif
}
#line 3323 "src/parser.c" /* yacc.c:1646  */
    break;

  case 106:
#line 758 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[-1].blk);
}
#line 3331 "src/parser.c" /* yacc.c:1646  */
    break;

  case 107:
#line 761 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_collect((yyvsp[-1].blk));
}
#line 3339 "src/parser.c" /* yacc.c:1646  */
    break;

  case 108:
#line 764 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_const(jv_array());
}
#line 3347 "src/parser.c" /* yacc.c:1646  */
    break;

  case 109:
#line 767 "src/parser.y" /* yacc.c:1646  */
    {
  block o = gen_const_object((yyvsp[-1].blk));
  if (o.first != NULL)
    (yyval.blk) = o;
  else
    (yyval.blk) = BLOCK(gen_subexp(gen_const(jv_object())), (yyvsp[-1].blk), gen_op_simple(POP));
}
#line 3359 "src/parser.c" /* yacc.c:1646  */
    break;

  case 110:
#line 774 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_const(JV_OBJECT(jv_string("file"), jv_copy(locations->fname),
                           jv_string("line"), jv_number(locfile_get_line(locations, (yyloc).start) + 1)));
}
#line 3368 "src/parser.c" /* yacc.c:1646  */
    break;

  case 111:
#line 778 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_location((yyloc), locations, gen_op_unbound(LOADV, jv_string_value((yyvsp[0].literal))));
  jv_free((yyvsp[0].literal));
}
#line 3377 "src/parser.c" /* yacc.c:1646  */
    break;

  case 112:
#line 782 "src/parser.y" /* yacc.c:1646  */
    {
  const char *s = jv_string_value((yyvsp[0].literal));
  if (strcmp(s, "false") == 0)
    (yyval.blk) = gen_const(jv_false());
  else if (strcmp(s, "true") == 0)
    (yyval.blk) = gen_const(jv_true());
  else if (strcmp(s, "null") == 0)
    (yyval.blk) = gen_const(jv_null());
  else
    (yyval.blk) = gen_location((yyloc), locations, gen_call(s, gen_noop()));
  jv_free((yyvsp[0].literal));
}
#line 3394 "src/parser.c" /* yacc.c:1646  */
    break;

  case 113:
#line 794 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_call(jv_string_value((yyvsp[-3].literal)), (yyvsp[-1].blk));
  (yyval.blk) = gen_location((yylsp[-3]), locations, (yyval.blk));
  jv_free((yyvsp[-3].literal));
}
#line 3404 "src/parser.c" /* yacc.c:1646  */
    break;

  case 114:
#line 799 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = gen_noop(); }
#line 3410 "src/parser.c" /* yacc.c:1646  */
    break;

  case 115:
#line 800 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = gen_noop(); }
#line 3416 "src/parser.c" /* yacc.c:1646  */
    break;

  case 116:
#line 801 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = (yyvsp[-3].blk); }
#line 3422 "src/parser.c" /* yacc.c:1646  */
    break;

  case 117:
#line 802 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = gen_noop(); }
#line 3428 "src/parser.c" /* yacc.c:1646  */
    break;

  case 118:
#line 805 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3436 "src/parser.c" /* yacc.c:1646  */
    break;

  case 119:
#line 808 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3444 "src/parser.c" /* yacc.c:1646  */
    break;

  case 120:
#line 813 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_lambda((yyvsp[0].blk));
}
#line 3452 "src/parser.c" /* yacc.c:1646  */
    break;

  case 121:
#line 818 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), gen_destructure_alt((yyvsp[0].blk)));
}
#line 3460 "src/parser.c" /* yacc.c:1646  */
    break;

  case 122:
#line 821 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_destructure_alt((yyvsp[0].blk));
}
#line 3468 "src/parser.c" /* yacc.c:1646  */
    break;

  case 123:
#line 826 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3476 "src/parser.c" /* yacc.c:1646  */
    break;

  case 124:
#line 829 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3484 "src/parser.c" /* yacc.c:1646  */
    break;

  case 125:
#line 834 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_op_unbound(STOREV, jv_string_value((yyvsp[0].literal)));
  jv_free((yyvsp[0].literal));
}
#line 3493 "src/parser.c" /* yacc.c:1646  */
    break;

  case 126:
#line 838 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-1].blk), gen_op_simple(POP));
}
#line 3501 "src/parser.c" /* yacc.c:1646  */
    break;

  case 127:
#line 841 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-1].blk), gen_op_simple(POP));
}
#line 3509 "src/parser.c" /* yacc.c:1646  */
    break;

  case 128:
#line 846 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_array_matcher(gen_noop(), (yyvsp[0].blk));
}
#line 3517 "src/parser.c" /* yacc.c:1646  */
    break;

  case 129:
#line 849 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_array_matcher((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3525 "src/parser.c" /* yacc.c:1646  */
    break;

  case 130:
#line 854 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3533 "src/parser.c" /* yacc.c:1646  */
    break;

  case 131:
#line 857 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = BLOCK((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3541 "src/parser.c" /* yacc.c:1646  */
    break;

  case 132:
#line 862 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_object_matcher(gen_const((yyvsp[0].literal)), gen_op_unbound(STOREV, jv_string_value((yyvsp[0].literal))));
}
#line 3549 "src/parser.c" /* yacc.c:1646  */
    break;

  case 133:
#line 865 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_object_matcher(gen_const((yyvsp[-2].literal)), BLOCK(gen_op_simple(DUP), gen_op_unbound(STOREV, jv_string_value((yyvsp[-2].literal))), (yyvsp[0].blk)));
}
#line 3557 "src/parser.c" /* yacc.c:1646  */
    break;

  case 134:
#line 868 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_object_matcher(gen_const((yyvsp[-2].literal)), (yyvsp[0].blk));
}
#line 3565 "src/parser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 871 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_object_matcher(gen_const((yyvsp[-2].literal)), (yyvsp[0].blk));
}
#line 3573 "src/parser.c" /* yacc.c:1646  */
    break;

  case 136:
#line 874 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_object_matcher((yyvsp[-2].blk), (yyvsp[0].blk));
}
#line 3581 "src/parser.c" /* yacc.c:1646  */
    break;

  case 137:
#line 877 "src/parser.y" /* yacc.c:1646  */
    {
  jv msg = check_object_key((yyvsp[-3].blk));
  if (jv_is_valid(msg)) {
    FAIL((yyloc), jv_string_value(msg));
  }
  jv_free(msg);
  (yyval.blk) = gen_object_matcher((yyvsp[-3].blk), (yyvsp[0].blk));
}
#line 3594 "src/parser.c" /* yacc.c:1646  */
    break;

  case 138:
#line 885 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "May need parentheses around object key expression");
  (yyval.blk) = (yyvsp[0].blk);
}
#line 3603 "src/parser.c" /* yacc.c:1646  */
    break;

  case 139:
#line 891 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("as");
}
#line 3611 "src/parser.c" /* yacc.c:1646  */
    break;

  case 140:
#line 894 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("def");
}
#line 3619 "src/parser.c" /* yacc.c:1646  */
    break;

  case 141:
#line 897 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("export");
}
#line 3627 "src/parser.c" /* yacc.c:1646  */
    break;

  case 142:
#line 900 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("module");
}
#line 3635 "src/parser.c" /* yacc.c:1646  */
    break;

  case 143:
#line 903 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("import");
}
#line 3643 "src/parser.c" /* yacc.c:1646  */
    break;

  case 144:
#line 906 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("include");
}
#line 3651 "src/parser.c" /* yacc.c:1646  */
    break;

  case 145:
#line 909 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("if");
}
#line 3659 "src/parser.c" /* yacc.c:1646  */
    break;

  case 146:
#line 912 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("then");
}
#line 3667 "src/parser.c" /* yacc.c:1646  */
    break;

  case 147:
#line 915 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("else");
}
#line 3675 "src/parser.c" /* yacc.c:1646  */
    break;

  case 148:
#line 918 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("elif");
}
#line 3683 "src/parser.c" /* yacc.c:1646  */
    break;

  case 149:
#line 921 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("reduce");
}
#line 3691 "src/parser.c" /* yacc.c:1646  */
    break;

  case 150:
#line 924 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("foreach");
}
#line 3699 "src/parser.c" /* yacc.c:1646  */
    break;

  case 151:
#line 927 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("end");
}
#line 3707 "src/parser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 930 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("and");
}
#line 3715 "src/parser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 933 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("or");
}
#line 3723 "src/parser.c" /* yacc.c:1646  */
    break;

  case 154:
#line 936 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("try");
}
#line 3731 "src/parser.c" /* yacc.c:1646  */
    break;

  case 155:
#line 939 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("catch");
}
#line 3739 "src/parser.c" /* yacc.c:1646  */
    break;

  case 156:
#line 942 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("label");
}
#line 3747 "src/parser.c" /* yacc.c:1646  */
    break;

  case 157:
#line 945 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("break");
}
#line 3755 "src/parser.c" /* yacc.c:1646  */
    break;

  case 158:
#line 948 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.literal) = jv_string("__loc__");
}
#line 3763 "src/parser.c" /* yacc.c:1646  */
    break;

  case 159:
#line 953 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk)=gen_noop();
}
#line 3771 "src/parser.c" /* yacc.c:1646  */
    break;

  case 160:
#line 956 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = (yyvsp[0].blk); }
#line 3777 "src/parser.c" /* yacc.c:1646  */
    break;

  case 161:
#line 957 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk)=block_join((yyvsp[-2].blk), (yyvsp[0].blk)); }
#line 3783 "src/parser.c" /* yacc.c:1646  */
    break;

  case 162:
#line 958 "src/parser.y" /* yacc.c:1646  */
    { (yyval.blk) = (yyvsp[0].blk); }
#line 3789 "src/parser.c" /* yacc.c:1646  */
    break;

  case 163:
#line 961 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair(gen_const((yyvsp[-2].literal)), (yyvsp[0].blk));
 }
#line 3797 "src/parser.c" /* yacc.c:1646  */
    break;

  case 164:
#line 964 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair(gen_const((yyvsp[-2].literal)), (yyvsp[0].blk));
  }
#line 3805 "src/parser.c" /* yacc.c:1646  */
    break;

  case 165:
#line 967 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair((yyvsp[-2].blk), (yyvsp[0].blk));
  }
#line 3813 "src/parser.c" /* yacc.c:1646  */
    break;

  case 166:
#line 970 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair((yyvsp[0].blk), BLOCK(gen_op_simple(POP), gen_op_simple(DUP2),
                              gen_op_simple(DUP2), gen_op_simple(INDEX)));
  }
#line 3822 "src/parser.c" /* yacc.c:1646  */
    break;

  case 167:
#line 974 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair(gen_const((yyvsp[0].literal)),
                    gen_location((yyloc), locations, gen_op_unbound(LOADV, jv_string_value((yyvsp[0].literal)))));
  }
#line 3831 "src/parser.c" /* yacc.c:1646  */
    break;

  case 168:
#line 978 "src/parser.y" /* yacc.c:1646  */
    {
  (yyval.blk) = gen_dictpair(gen_const(jv_copy((yyvsp[0].literal))),
                    gen_index(gen_noop(), gen_const((yyvsp[0].literal))));
  }
#line 3840 "src/parser.c" /* yacc.c:1646  */
    break;

  case 169:
#line 982 "src/parser.y" /* yacc.c:1646  */
    {
  jv msg = check_object_key((yyvsp[-3].blk));
  if (jv_is_valid(msg)) {
    FAIL((yyloc), jv_string_value(msg));
  }
  jv_free(msg);
  (yyval.blk) = gen_dictpair((yyvsp[-3].blk), (yyvsp[0].blk));
  }
#line 3853 "src/parser.c" /* yacc.c:1646  */
    break;

  case 170:
#line 990 "src/parser.y" /* yacc.c:1646  */
    {
  FAIL((yyloc), "May need parentheses around object key expression");
  (yyval.blk) = (yyvsp[0].blk);
  }
#line 3862 "src/parser.c" /* yacc.c:1646  */
    break;


#line 3866 "src/parser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, answer, errors, locations, lexer_param_ptr, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, answer, errors, locations, lexer_param_ptr, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, answer, errors, locations, lexer_param_ptr);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, answer, errors, locations, lexer_param_ptr);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, answer, errors, locations, lexer_param_ptr, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, answer, errors, locations, lexer_param_ptr);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, answer, errors, locations, lexer_param_ptr);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 994 "src/parser.y" /* yacc.c:1906  */


int jq_parse(struct locfile* locations, block* answer) {
  struct lexer_param scanner;
  YY_BUFFER_STATE buf;
  jq_yylex_init_extra(0, &scanner.lexer);
  buf = jq_yy_scan_bytes(locations->data, locations->length, scanner.lexer);
  int errors = 0;
  *answer = gen_noop();
  yyparse(answer, &errors, locations, &scanner);
  jq_yy_delete_buffer(buf, scanner.lexer);
  jq_yylex_destroy(scanner.lexer);
  if (errors > 0) {
    block_free(*answer);
    *answer = gen_noop();
  }
  return errors;
}

int jq_parse_library(struct locfile* locations, block* answer) {
  int errs = jq_parse(locations, answer);
  if (errs) return errs;
  if (block_has_main(*answer)) {
    locfile_locate(locations, UNKNOWN_LOCATION, "jq: error: library should only have function definitions, not a main expression");
    return 1;
  }
  assert(block_has_only_binders_and_imports(*answer, OP_IS_CALL_PSEUDO));
  return 0;
}
