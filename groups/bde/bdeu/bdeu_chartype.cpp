// bdeu_chartype.cpp           -*-C++-*-
#include <bdeu_chartype.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeu_chartype_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

// ============================================================================
// ***                      STATIC CONSTANT STRING VARIABLES                ***
// ----------------------------------------------------------------------------

static const char UPPER_STRING[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char LOWER_STRING[]  = "abcdefghijklmnopqrstuvwxyz";

static const char ALPHA_STRING[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

static const char DIGIT_STRING[]  = "0123456789";

static const char XDIGIT_STRING[] = "0123456789ABCDEFabcdef";

static const char ALNUM_STRING[]  = "0123456789"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "abcdefghijklmnopqrstuvwxyz";

static const char SPACE_STRING[]  = "\011\012\013\014\015 ";

static const char PRINT_STRING[]  = " !\"#$%&'()*+,-./0123456789:;<=>?@"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                                    "abcdefghijklmnopqrstuvwxyz{|}~";

static const char GRAPH_STRING[]  = "!\"#$%&'()*+,-./0123456789:;<=>?@"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                                    "abcdefghijklmnopqrstuvwxyz{|}~";

static const char PUNCT_STRING[]  = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

static const char CNTRL_STRING[]  = "\000\001\002\003\004\005\006\007\010\011"
                                    "\012\013\014\015\016\017\020\021\022\023"
                                    "\024\025\026\027\030\031\032\033\034\035"
                                    "\036\037\177";

static const char ASCII_STRING[]  = "\000\001\002\003\004\005\006\007\010\011"
                                    "\012\013\014\015\016\017\020\021\022\023"
                                    "\024\025\026\027\030\031\032\033\034\035"
                                    "\036\037"
                                    " !\"#$%&'()*+,-./0123456789:;<=>?@"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                                    "abcdefghijklmnopqrstuvwxyz{|}~\177";

static const char IDENT_STRING[]  = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
                                    "abcdefghijklmnopqrstuvwxyz";

static const char ALUND_STRING[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
                                    "abcdefghijklmnopqrstuvwxyz";

static const char ALL_STRING[]    = "\000\001\002\003\004\005\006\007\010\011"
                                    "\012\013\014\015\016\017\020\021\022\023"
                                    "\024\025\026\027\030\031\032\033\034\035"
                                    "\036\037"
                                    " !\"#$%&'()*+,-./0123456789:;<=>?@"
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                                    "abcdefghijklmnopqrstuvwxyz{|}~\177"
                                    "\200\201\202\203\204\205\206\207\210\211"
                                    "\212\213\214\215\216\217\220\221\222\223"
                                    "\224\225\226\227\230\231\232\233\234\235"
                                    "\236\237\240\241\242\243\244\245\246\247"
                                    "\250\251\252\253\254\255\256\257\260\261"
                                    "\262\263\264\265\266\267\270\271\272\273"
                                    "\274\275\276\277\300\301\302\303\304\305"
                                    "\306\307\310\311\312\313\314\315\316\317"
                                    "\320\321\322\323\324\325\326\327\330\331"
                                    "\332\333\334\335\336\337\340\341\342\343"
                                    "\344\345\346\347\350\351\352\353\354\355"
                                    "\356\357\360\361\362\363\364\365\366\367"
                                    "\370\371\372\373\374\375\376\377";

static const char NONE_STRING[]   = "";

static const char *CATEGORY_STRING[bdeu_CharType::BDEU_NUM_CATEGORIES] = {
    UPPER_STRING,
    LOWER_STRING,
    ALPHA_STRING,
    DIGIT_STRING,
    XDIGIT_STRING,
    ALNUM_STRING,
    SPACE_STRING,
    PRINT_STRING,
    GRAPH_STRING,
    PUNCT_STRING,
    CNTRL_STRING,
    ASCII_STRING,
    IDENT_STRING,
    ALUND_STRING,
    ALL_STRING,
    NONE_STRING,
};

static const char *CATEGORY_NAME[bdeu_CharType::BDEU_NUM_CATEGORIES] = {
    "UPPER",
    "LOWER",
    "ALPHA",
    "DIGIT",
    "XDIGIT",
    "ALNUM",
    "SPACE",
    "PRINT",
    "GRAPH",
    "PUNCT",
    "CNTRL",
    "ASCII",
    "IDENT",
    "ALUND",
    "ALL",
    "NONE",
};

// ============================================================================
// ***                   STATIC CONSTANT TABLES                             ***
// ----------------------------------------------------------------------------

static const bool UPPER_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
};

static const bool LOWER_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 70
};

static const bool ALPHA_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 50
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 70
};

static const bool DIGIT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
};

static const bool XDIGIT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
};

static const bool ALNUM_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 50
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 70
};

static const bool SPACE_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
};

static const bool PRINT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 30
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 50
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  // 70
};

static const bool GRAPH_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 30
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 50
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  // 70
};

static const bool PUNCT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,  // 30
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,  // 50
       1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,  // 70
};

static const bool CNTRL_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 00
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // 70
};

static const bool ASCII_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 00
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 30
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 50
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 70
};

static const bool IDENT_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  // 50
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 70
};

static const bool ALUND_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  // 50
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  // 70
};

static const bool ALL_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 00
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 10
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 20
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 30
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 40
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 50
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 60
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 70
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 80
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 90
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // A0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // B0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // C0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // D0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // E0
       1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // F0
};

static const bool NONE_TABLE[256] = {
    // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
};

static const bool *CATEGORY_TABLE[bdeu_CharType::BDEU_NUM_CATEGORIES] = {
    UPPER_TABLE,
    LOWER_TABLE,
    ALPHA_TABLE,
    DIGIT_TABLE,
    XDIGIT_TABLE,
    ALNUM_TABLE,
    SPACE_TABLE,
    PRINT_TABLE,
    GRAPH_TABLE,
    PUNCT_TABLE,
    CNTRL_TABLE,
    ASCII_TABLE,
    IDENT_TABLE,
    ALUND_TABLE,
    ALL_TABLE,
    NONE_TABLE,
};

static const short int CATEGORY_COUNT[bdeu_CharType::BDEU_NUM_CATEGORIES] = {
    sizeof UPPER_STRING - 1,
    sizeof LOWER_STRING - 1,
    sizeof ALPHA_STRING - 1,
    sizeof DIGIT_STRING - 1,
    sizeof XDIGIT_STRING - 1,
    sizeof ALNUM_STRING - 1,
    sizeof SPACE_STRING - 1,
    sizeof PRINT_STRING - 1,
    sizeof GRAPH_STRING - 1,
    sizeof PUNCT_STRING - 1,
    sizeof CNTRL_STRING - 1,
    sizeof ASCII_STRING - 1,
    sizeof IDENT_STRING - 1,
    sizeof ALUND_STRING - 1,
    sizeof ALL_STRING - 1,
    sizeof NONE_STRING - 1,
};

static const unsigned char TOUPPER_TABLE[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  // 00
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  // 10
       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,  // 20
       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,  // 30
       64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,  // 40
       80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,  // 50
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
       96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,  // 60
       80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,  // 70
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
      128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,  // 80
      144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,  // 90
      160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,  // A0
      176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,  // B0
      192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,  // C0
      208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,  // D0
      224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,  // E0
      240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,  // F0
};

static const unsigned char TOLOWER_TABLE[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  // 00
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  // 10
       32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,  // 20
       48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,  // 30
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
       64, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,  // 40
      112,113,114,115,116,117,118,119,120,121,122, 91, 92, 93, 94, 95,  // 50
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
       96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,  // 60
      112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,  // 70
      128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,  // 80
      144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,  // 90
      160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,  // A0
      176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,  // B0
      192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,  // C0
      208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,  // D0
      224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,  // E0
      240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,  // F0
};

// ============================================================================
// ***                STATIC CLASS DATA MEMBER DEFINITIONS
// ----------------------------------------------------------------------------

const char *const bdeu_CharType::s_upperString_p  = UPPER_STRING;
const char *const bdeu_CharType::s_lowerString_p  = LOWER_STRING;
const char *const bdeu_CharType::s_alphaString_p  = ALPHA_STRING;
const char *const bdeu_CharType::s_digitString_p  = DIGIT_STRING;
const char *const bdeu_CharType::s_xdigitString_p = XDIGIT_STRING;
const char *const bdeu_CharType::s_alnumString_p  = ALNUM_STRING;
const char *const bdeu_CharType::s_spaceString_p  = SPACE_STRING;
const char *const bdeu_CharType::s_printString_p  = PRINT_STRING;
const char *const bdeu_CharType::s_graphString_p  = GRAPH_STRING;
const char *const bdeu_CharType::s_punctString_p  = PUNCT_STRING;
const char *const bdeu_CharType::s_cntrlString_p  = CNTRL_STRING;
const char *const bdeu_CharType::s_asciiString_p  = ASCII_STRING;
const char *const bdeu_CharType::s_identString_p  = IDENT_STRING;
const char *const bdeu_CharType::s_alundString_p  = ALUND_STRING;
const char *const bdeu_CharType::s_allString_p    = ALL_STRING;
const char *const bdeu_CharType::s_noneString_p   = NONE_STRING;

const char *const *const bdeu_CharType::s_categoryString_p = CATEGORY_STRING;

const char *const *const bdeu_CharType::s_categoryName_p = CATEGORY_NAME;

const short int bdeu_CharType::s_upperCount  = sizeof UPPER_STRING - 1;
const short int bdeu_CharType::s_lowerCount  = sizeof LOWER_STRING - 1;
const short int bdeu_CharType::s_alphaCount  = sizeof ALPHA_STRING - 1;
const short int bdeu_CharType::s_digitCount  = sizeof DIGIT_STRING - 1;
const short int bdeu_CharType::s_xdigitCount = sizeof XDIGIT_STRING - 1;
const short int bdeu_CharType::s_alnumCount  = sizeof ALNUM_STRING - 1;
const short int bdeu_CharType::s_spaceCount  = sizeof SPACE_STRING - 1;
const short int bdeu_CharType::s_printCount  = sizeof PRINT_STRING - 1;
const short int bdeu_CharType::s_graphCount  = sizeof GRAPH_STRING - 1;
const short int bdeu_CharType::s_punctCount  = sizeof PUNCT_STRING - 1;
const short int bdeu_CharType::s_cntrlCount  = sizeof CNTRL_STRING - 1;
const short int bdeu_CharType::s_asciiCount  = sizeof ASCII_STRING - 1;
const short int bdeu_CharType::s_identCount  = sizeof IDENT_STRING - 1;
const short int bdeu_CharType::s_alundCount  = sizeof ALUND_STRING - 1;
const short int bdeu_CharType::s_allCount    = sizeof ALL_STRING - 1;
const short int bdeu_CharType::s_noneCount   = sizeof NONE_STRING - 1;

const short int *bdeu_CharType::s_categoryCount_p = CATEGORY_COUNT;

const bool *const bdeu_CharType::s_upperArray_p  = UPPER_TABLE;
const bool *const bdeu_CharType::s_lowerArray_p  = LOWER_TABLE;
const bool *const bdeu_CharType::s_alphaArray_p  = ALPHA_TABLE;
const bool *const bdeu_CharType::s_digitArray_p  = DIGIT_TABLE;
const bool *const bdeu_CharType::s_xdigitArray_p = XDIGIT_TABLE;
const bool *const bdeu_CharType::s_alnumArray_p  = ALNUM_TABLE;
const bool *const bdeu_CharType::s_spaceArray_p  = SPACE_TABLE;
const bool *const bdeu_CharType::s_printArray_p  = PRINT_TABLE;
const bool *const bdeu_CharType::s_graphArray_p  = GRAPH_TABLE;
const bool *const bdeu_CharType::s_punctArray_p  = PUNCT_TABLE;
const bool *const bdeu_CharType::s_cntrlArray_p  = CNTRL_TABLE;
const bool *const bdeu_CharType::s_asciiArray_p  = ASCII_TABLE;
const bool *const bdeu_CharType::s_identArray_p  = IDENT_TABLE;
const bool *const bdeu_CharType::s_alundArray_p  = ALUND_TABLE;
const bool *const bdeu_CharType::s_allArray_p    = ALL_TABLE;
const bool *const bdeu_CharType::s_noneArray_p   = NONE_TABLE;

const bool **bdeu_CharType::s_categoryArray_p = CATEGORY_TABLE;

const char *const bdeu_CharType::s_toUpper_p = (const char *)TOUPPER_TABLE;
const char *const bdeu_CharType::s_toLower_p = (const char *)TOLOWER_TABLE;

// ============================================================================
// ***                     NON-INLINE CLASS METHODS                         ***
// ----------------------------------------------------------------------------

bsl::ostream& operator<<(bsl::ostream& out, bdeu_CharType::Category category)
{
     return out << bdeu_CharType::toAscii(category) << bsl::flush;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
