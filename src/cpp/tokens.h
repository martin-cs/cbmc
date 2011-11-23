// tokens

enum {

TOK_IDENTIFIER   = 258,
TOK_INTEGER      = 260,
TOK_CHARACTER    = 261,
TOK_FLOATING     = 262,
TOK_STRING       = 263,
TOK_AssignOp     = 266,
TOK_EqualOp      = 267,
TOK_RelOp        = 268,
TOK_ShiftOp      = 269,
TOK_LogOrOp      = 270,
TOK_ANDAND       = 271,
TOK_INCR         = 272,
TOK_DECR         = 273,
TOK_SCOPE        = 274,
TOK_ELLIPSIS     = 275,
TOK_PmOp         = 276,
TOK_ArrowOp      = 277,
TOK_BadToken     = 278,
TOK_AUTO         = 281,
TOK_CHAR         = 282,
TOK_CLASS        = 283,
TOK_CONST        = 284,
TOK_DELETE       = 285,
TOK_DOUBLE       = 286,
TOK_ENUM         = 287,
TOK_EXTERN       = 288,
TOK_FLOAT        = 289,
TOK_FRIEND       = 290,
TOK_INLINE       = 291,
TOK_INT          = 292,
TOK_LONG         = 293,
TOK_NEW          = 294,
TOK_OPERATOR     = 295,
TOK_PRIVATE      = 296,
TOK_PROTECTED    = 297,
TOK_PUBLIC       = 298,
TOK_REGISTER     = 299,
TOK_SHORT        = 300,
TOK_SIGNED       = 301,
TOK_STATIC       = 302,
TOK_STRUCT       = 303,
TOK_TYPEDEF      = 304,
TOK_UNION        = 305,
TOK_UNSIGNED     = 306,
TOK_VIRTUAL      = 307,
TOK_VOID         = 308,
TOK_VOLATILE     = 309,
TOK_TEMPLATE     = 310,
TOK_MUTABLE      = 311,
TOK_BREAK        = 312,
TOK_CASE         = 313,
TOK_CONTINUE     = 314,
TOK_DEFAULT      = 315,
TOK_DO           = 316,
TOK_ELSE         = 317,
TOK_FOR          = 318,
TOK_GOTO         = 319,
TOK_IF           = 320,
TOK_RETURN       = 321,
TOK_SIZEOF       = 322,
TOK_SWITCH       = 323,
TOK_THIS         = 324,
TOK_WHILE        = 325,
TOK_ATTRIBUTE    = 326,
TOK_BOOL         = 332,
TOK_EXTENSION    = 333,
TOK_TRY          = 334,
TOK_CATCH        = 335,
TOK_THROW        = 336,
TOK_NAMESPACE    = 338,
TOK_USING        = 339,
TOK_TYPEID       = 340,
TOK_WideStringL  = 341,
TOK_WideCharConst= 342,
TOK_WCHAR        = 343,
TOK_EXPLICIT     = 344,
TOK_TYPENAME     = 345,
TOK_WCHAR_T      = 346,
TOK_INT8         = 347,
TOK_INT16        = 348,
TOK_INT32        = 349,
TOK_INT64        = 350,
TOK_PTR32        = 351,
TOK_PTR64        = 352,
TOK_COMPLEX      = 353,
TOK_REAL         = 354,
TOK_IMAG         = 355,

TOK_Ignore       = 500,
TOK_GCC_ASM      = 501,
TOK_DECLSPEC     = 502,
TOK_TYPEOF       = 504,
TOK_MSC_ASM      = 505,
TOK_THREAD_LOCAL = 506,
TOK_DECLTYPE     = 507,
TOK_CDECL        = 508,
TOK_STDCALL      = 509,
TOK_FASTCALL     = 510,
TOK_CLRCALL      = 511,
TOK_STATIC_ASSERT= 512,

// MSC-specific
TOK_INTERFACE    = 513,
TOK_MSC_UNARY_TYPE_PREDICATE = 514,
TOK_MSC_BINARY_TYPE_PREDICATE = 515,
TOK_MSC_TRY      = 516,
TOK_MSC_EXCEPT   = 517,
TOK_MSC_FINALLY  = 518,
TOK_MSC_LEAVE    = 519,
TOK_MSC_UUIDOF   = 520,
TOK_MSC_IF_EXISTS= 521,
TOK_MSC_IF_NOT_EXISTS=522
};

