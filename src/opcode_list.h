OP(LOADK, CONSTANT, 1, 1)
OP(DUP,   NONE,     1, 2)
OP(DUPN,  NONE,     1, 2)
OP(DUP2,  NONE,     2, 3)
OP(PUSHK_UNDER, CONSTANT, 1, 2)
OP(POP,   NONE,     1, 0)
OP(LOADV, VARIABLE, 1, 1)
OP(LOADVN, VARIABLE, 1, 1)
OP(STOREV, VARIABLE, 1, 0)
OP(STORE_GLOBAL, GLOBAL, 0, 0)
OP(INDEX, NONE,     2, 1)
OP(INDEX_OPT, NONE,     2, 1)
OP(EACH,  NONE,     1, 1)
OP(EACH_OPT,  NONE, 1, 1)
OP(FORK,  BRANCH,   0, 0)
OP(TRY_BEGIN,  BRANCH,   0, 0)
OP(TRY_END,  NONE,   0, 0)
OP(JUMP,  BRANCH,   0, 0)
OP(JUMP_F,BRANCH,   1, 0)
OP(BACKTRACK, BACKTRACKING, 0, 0)
OP(APPEND, VARIABLE,1, 0)
OP(INSERT, NONE,    4, 2)
OP(RANGE, VARIABLE, 1, 1)

OP(SUBEXP_BEGIN,  NONE,     1, 2)
OP(SUBEXP_END,    NONE,     2, 2)

OP(PATH_BEGIN, NONE, 1, 2)
OP(PATH_END,   NONE, 2, 1)

OP(CALL_BUILTIN, CFUNC, -1, 1)

OP(CALL_JQ, UFUNC, 0, 0)
OP(RET_JQ, NONE, 0, 0)
OP(TAIL_CALL_JQ, UFUNC, 0, 0)

OP(CLOSURE_PARAM, DEFINITION, 0, 0)
OP(CLOSURE_REF, CLOSURE_REF_IMM, 0, 0)
OP(CLOSURE_CREATE, DEFINITION, 0, 0)
OP(CLOSURE_CREATE_C, DEFINITION, 0, 0)

// Schroedinger's TOP is there and not there at the same time
OP(TOP, MARKER, 0, 0)

OP(COCREATE, BRANCH, 1, 1)
// the entry point of a coroutine and main
OP(START, NONE, 0, 1)
OP(CLOSURE_PARAM_REGULAR, DEFINITION, 0, 0)
OP(CLOSURE_PARAM_COEXPR, DEFINITION, 0, 0)
OP(DEPS, CONSTANT, 0, 0)
OP(MODULEMETA, CONSTANT, 0, 0)
OP(GENLABEL, NONE, 0, 1)

OP(DESTRUCTURE_ALT, BRANCH, 0, 0)
OP(STOREVN, VARIABLE, 1, 0)
OP(COEVAL, NONE, 1, 1)
OP(UNWINDING, NONE, 0, 0)
OP(OUT, NONE, 1, 0)
OP(TAIL_OUT, BACKTRACKING, 1, 0)
