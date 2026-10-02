/* Original: flex and bison output, 0x0041d810-0x0041fd40 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

#include "parser.h"
#include "utils.h"


#define YYLAST              348
#define YYFINAL             186
#define YYFLAG              -32768
#define YYNTBASE            54
#define YYMAXUTOK           307
#define YYUNDEFTOK          77
#define YY_JAMBASE          312
#define YY_METASTATE        276
#define YY_END_OF_BUFFER    65

#define TOKEN_FIRST         257

static const unsigned char yytranslate[308] =  /* 0x004b2bb0 */
{
    0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53
};


static const short yyr1[76] =  /* 0x004b2ce8 */
{
    0, 54, 54, 54, 55, 55, 56, 57, 57, 57, 57, 57,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 58,
    58, 59, 60, 60, 61, 61, 62, 62, 63, 64, 65, 65,
    66, 66, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71,
    71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 72, 73,
    73, 73, 73, 73, 73, 73, 73, 73, 73, 74, 74, 74,
    75, 75, 76, 76
};


static const short yyr2[76] =  /* 0x004b2d80 */
{
    0, 2, 3, 2, 2, 0, 3, 2, 2, 4, 4, 4,
    4, 8, 8, 4, 4, 6, 6, 4, 4, 4, 4, 2,
    0, 3, 3, 1, 3, 1, 1, 0, 1, 1, 3, 1,
    1, 0, 4, 2, 0, 5, 4, 6, 2, 0, 4, 4,
    4, 5, 4, 2, 1, 2, 4, 1, 1, 4, 1, 1,
    3, 3, 1, 1, 1, 5, 3, 3, 3, 2, 5, 4,
    2, 0, 1, 1
};


static const short yydefact[188] =  /* 0x004b2e18 */
{
    5, 73, 0, 4, 24, 1, 3, 0, 0, 0, 0, 0,
    0, 52, 0, 55, 56, 0, 6, 0, 72, 2, 32, 31,
    0, 74, 75, 0, 0, 73, 51, 53, 0, 37, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 23, 58, 69, 45, 27, 29, 30, 0, 0, 0, 0,
    0, 0, 62, 0, 63, 64, 36, 0, 59, 35, 7, 24,
    8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 45, 37, 0, 0, 0, 33, 46, 47, 48, 0,
    50, 54, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 71, 0, 0, 0, 44, 26, 28, 49, 66, 34,
    68, 67, 60, 61, 0, 25, 9, 10, 11, 12, 0, 0,
    15, 16, 0, 0, 19, 20, 21, 22, 70, 0, 40, 0,
    0, 0, 0, 0, 0, 40, 0, 0, 65, 0, 0, 17,
    18, 0, 0, 42, 39, 0, 0, 0, 41, 37, 0, 13,
    14, 0, 43, 38, 0, 0, 0, 0
};


static const short yydefgoto[23] =  /* 0x004b2f90 */
{
    3, 1, 184, 49, 18, 70, 54, 55, 56, 71, 92, 66,
    67, 172, 162, 126, 88, 19, 68, 69, 20, 4, 27
};


static const short yypact[188] =  /* 0x004b2fc0 */
{
    -32768, -47, 4, -32768, 295, 5, -32768, 10, 42, 6, 6, 10,
    295, 41, 65, -32768, -32768, 68, 285, 78, 73, -32768, -32768, 81,
    71, -32768, -32768, 6, 6, -32768, -32768, -32768, 295, 3, 10, 10,
    80, 82, 83, 84, 86, 89, 91, 94, 96, 97, 103, 104,
    106, 79, -32768, 107, -32768, -32768, 95, 76, 110, 114, 6, 6,
    31, 109, -32768, 3, -32768, -32768, 92, 132, -32768, 32, -32768, -32768,
    -32768, 3, 3, 3, 3, 3, 3, 3, 3, 3, 140, 3,
    3, 3, -32768, 3, 1, 142, 81, -32768, -32768, -32768, -32768, 6,
    -32768, -32768, 58, 3, -32768, 3, 3, 3, 3, 3, 102, 122,
    131, 137, 145, 213, 219, 150, 157, 231, 111, 172, 185, 192,
    30, 133, -32768, 148, 10, 152, -32768, -32768, 95, -32768, -32768, 32,
    53, 0, 118, -32768, 206, -32768, -32768, -32768, -32768, -32768, 155, 6,
    -32768, -32768, 3, 163, -32768, -32768, -32768, -32768, -32768, 10, -32768, 3,
    3, 135, 138, 197, 168, -32768, 17, 253, 257, 156, 6, -32768,
    -32768, 17, 175, -32768, -32768, 78, 177, 180, -32768, 3, 182, -32768,
    -32768, 187, -32768, -32768, 203, 207, -32768, 0
};


static const short yypgoto[23] =  /* 0x004b3138 */
{
    -32768, -32768, -32768, -32768, 141, 173, 121, -32768, -32768, -5, -56, -32768,
    -84, -32768, 52, -32768, 130, 2, -18, -48, -32768, 191, -10
};


static const short yytable[349] =  /* 0x004b3168 */
{
    28, 51, 23, 121, 96, 2, 29, 50, 62, 5, 21, 63,
    122, 102, 30, 98, 22, 58, 59, 64, 6, 170, 25, 26,
    91, 107, 108, 109, 110, 111, 112, 113, 114, 115, 61, 117,
    118, 119, 91, 123, 124, 151, 103, 104, 101, 102, 31, 65,
    94, 95, 137, 131, 125, 132, 133, 134, 135, 136, 24, 7,
    8, 9, 10, 11, 12, 101, 102, 130, 123, 124, 101, 102,
    13, 32, 103, 104, 33, 105, 14, 15, 16, 125, 50, 52,
    17, 129, 53, 57, 73, 86, 74, 75, 76, 181, 77, 103,
    104, 78, 159, 79, 103, 104, 80, 105, 81, 82, 171, 163,
    164, 91, 89, 83, 84, 176, 85, 87, 90, 91, 97, 154,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 93, 138,
    99, 158, 101, 102, 44, 45, 46, 47, 139, 100, 152, 101,
    102, 116, 140, 127, 161, 101, 102, 147, 48, 153, 141, 178,
    175, 101, 102, 144, 155, 104, 101, 102, 103, 104, 145, 105,
    160, 101, 102, 157, 174, 103, 104, 165, 105, 168, 166, 103,
    104, 148, 105, 177, 101, 102, 179, 103, 104, 180, 105, 182,
    103, 104, 149, 105, 183, 101, 102, 103, 104, 150, 105, 185,
    101, 102, 167, 186, 72, 101, 102, 128, 106, 169, 103, 104,
    120, 105, 101, 102, 60, 0, 0, 0, 0, 101, 102, 103,
    104, 0, 105, 101, 102, 0, 103, 104, 0, 105, 0, 103,
    104, 0, 105, 101, 102, 0, 0, 0, 103, 104, 0, 105,
    156, 142, 0, 103, 104, 0, 105, 143, 0, 103, 104, 0,
    105, 101, 102, 0, 0, 101, 102, 146, 0, 103, 104, 0,
    105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 173, 0, 103, 104, 0, 105, 103,
    104, 0, -32768, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 0, 0, 0, 0, 0, 0, 44, 45, 46, 47, 7,
    8, 9, 10, 11, 12, 0, 0, 0, 0, 0, 0, 48,
    13, 0, 0, 0, 0, 0, 14, 15, 16, 0, 0, 0,
    17
};


static const short yycheck[349] =  /* 0x004b3428 */
{
    10, 19, 7, 87, 60, 52, 11, 4, 5, 5, 5, 8,
    11, 13, 12, 63, 6, 27, 28, 16, 16, 4, 16, 17,
    7, 73, 74, 75, 76, 77, 78, 79, 80, 81, 32, 83,
    84, 85, 7, 38, 39, 11, 42, 43, 12, 13, 5, 44,
    58, 59, 106, 99, 51, 101, 102, 103, 104, 105, 16, 28,
    29, 30, 31, 32, 33, 12, 13, 9, 38, 39, 12, 13,
    41, 8, 42, 43, 8, 45, 47, 48, 49, 51, 4, 10,
    53, 95, 5, 16, 8, 10, 8, 8, 8, 177, 8, 42,
    43, 8, 146, 8, 42, 43, 8, 45, 8, 8, 162, 155,
    156, 7, 15, 8, 8, 169, 8, 8, 40, 7, 9, 124,
    18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 16, 9,
    40, 143, 12, 13, 34, 35, 36, 37, 9, 9, 9, 12,
    13, 5, 9, 5, 153, 12, 13, 40, 50, 5, 9, 173,
    166, 12, 13, 9, 8, 43, 12, 13, 42, 43, 9, 45,
    5, 12, 13, 16, 16, 42, 43, 40, 45, 9, 40, 42,
    43, 9, 45, 8, 12, 13, 9, 42, 43, 9, 45, 9,
    42, 43, 9, 45, 9, 12, 13, 42, 43, 9, 45, 0,
    12, 13, 9, 0, 35, 12, 13, 90, 71, 161, 42, 43,
    86, 45, 12, 13, 29, -1, -1, -1, -1, 12, 13, 42,
    43, -1, 45, 12, 13, -1, 42, 43, -1, 45, -1, 42,
    43, -1, 45, 12, 13, -1, -1, -1, 42, 43, -1, 45,
    46, 40, -1, 42, 43, -1, 45, 40, -1, 42, 43, -1,
    45, 12, 13, -1, -1, 12, 13, 40, -1, 42, 43, -1,
    45, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 40, -1, 42, 43, -1, 45, 42,
    43, -1, 45, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, -1, -1, -1, -1, -1, -1, 34, 35, 36, 37, 28,
    29, 30, 31, 32, 33, -1, -1, -1, -1, -1, -1, 50,
    41, -1, -1, -1, -1, -1, 47, 48, 49, -1, -1, -1,
    53
};


static const short yy_accept[276] =  /* 0x004b36e8 */
{
    0, 0, 0, 0, 0, 0, 0, 65, 63, 1, 4, 16,
    63, 63, 63, 10, 11, 50, 63, 63, 63, 18, 55, 63,
    54, 62, 62, 12, 13, 62, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 8, 17, 9, 3, 3, 4,
    52, 0, 7, 0, 0, 0, 0, 0, 0, 0, 15, 0,
    18, 19, 6, 5, 0, 51, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 14, 2, 0, 0, 0, 0,
    0, 0, 5, 0, 19, 62, 62, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 31, 62, 62, 35, 62, 56, 62,
    62, 62, 62, 62, 44, 0, 0, 0, 0, 0, 53, 62,
    62, 62, 57, 24, 30, 62, 41, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 0, 43, 0, 0,
    0, 62, 62, 33, 62, 62, 40, 62, 62, 32, 62, 62,
    62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0,
    37, 62, 62, 62, 62, 62, 60, 62, 62, 62, 62, 62,
    62, 62, 62, 62, 42, 34, 62, 0, 0, 0, 0, 62,
    62, 62, 62, 62, 62, 62, 61, 36, 62, 62, 62, 62,
    62, 62, 0, 0, 0, 49, 38, 62, 58, 59, 28, 62,
    62, 62, 62, 62, 62, 26, 62, 48, 0, 0, 0, 62,
    27, 62, 25, 62, 22, 62, 62, 0, 0, 0, 62, 20,
    62, 62, 21, 45, 46, 0, 39, 62, 62, 0, 62, 62,
    0, 29, 23, 0, 0, 0, 0, 0, 0, 0, 47, 0
};


static const int yy_ec[256] =  /* 0x004b3910 */
{
    0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 4, 5, 6, 1, 1, 7, 1, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 1, 1, 18, 1, 19,
    1, 20, 20, 20, 20, 21, 20, 20, 20, 20, 20, 20, 22, 20, 23, 20,
    20, 20, 20, 20, 20, 24, 20, 20, 20, 20, 20, 25, 26, 27, 1, 28,
    1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 20, 38, 39, 40, 41, 42,
    43, 20, 44, 45, 46, 47, 48, 49, 50, 51, 20, 52, 53, 54, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


static const int yy_meta[56] =  /* 0x004b3d10 */
{
    0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 3, 3, 3, 3, 3, 1, 1, 1, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 1, 1, 1, 0
};


static const short yy_base[284] =  /* 0x004b3df0 */
{
    0, 308, 307, 0, 0, 52, 53, 309, 312, 312, 306, 289,
    52, 26, 299, 312, 312, 312, 45, 289, 54, 52, 312, 286,
    312, 0, 279, 312, 312, 255, 34, 272, 43, 40, 269, 258,
    265, 260, 36, 263, 49, 264, 312, 241, 312, 312, 278, 290,
    312, 76, 312, 0, 252, 253, 260, 246, 258, 253, 312, 269,
    73, 69, 312, 0, 85, 312, 0, 262, 237, 249, 242, 234,
    242, 231, 244, 243, 236, 232, 231, 226, 220, 225, 226, 229,
    233, 222, 225, 227, 219, 220, 312, 312, 222, 221, 219, 211,
    213, 212, 0, 239, 238, 231, 206, 220, 208, 220, 202, 207,
    205, 214, 64, 215, 202, 0, 209, 195, 0, 194, 0, 197,
    195, 196, 62, 195, 312, 202, 201, 189, 197, 186, 0, 188,
    191, 184, 59, 0, 0, 181, 0, 193, 192, 178, 177, 181,
    191, 183, 180, 179, 184, 175, 184, 168, 185, 312, 181, 178,
    173, 168, 178, 164, 177, 168, 0, 172, 159, 169, 172, 153,
    157, 70, 159, 164, 164, 160, 148, 164, 159, 158, 144, 147,
    0, 146, 150, 146, 152, 147, 0, 151, 145, 141, 130, 147,
    141, 147, 137, 138, 0, 0, 137, 123, 144, 143, 129, 119,
    137, 134, 117, 119, 127, 122, 0, 0, 125, 122, 118, 122,
    112, 116, 110, 123, 73, 312, 0, 116, 0, 0, 0, 107,
    119, 105, 117, 99, 103, 0, 114, 312, 109, 108, 97, 110,
    0, 98, 0, 108, 0, 94, 95, 94, 93, 106, 91, 0,
    102, 103, 0, 312, 312, 104, 0, 94, 87, 97, 71, 66,
    70, 0, 0, 69, 80, 66, 83, 76, 69, 44, 312, 312,
    118, 121, 124, 79, 127, 0, 0, 0
};


static const short yy_def[284] =  /* 0x004b4028 */
{
    0, 276, 276, 275, 3, 277, 277, 275, 275, 275, 275, 275,
    278, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 279, 279, 275, 275, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 275, 275, 275, 275, 275, 275,
    275, 278, 275, 278, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 280, 275, 275, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 275, 275, 275, 275, 275, 275,
    275, 275, 280, 275, 275, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 275, 275, 275, 275, 275, 275, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 275, 275, 275, 275,
    275, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 275, 275, 275, 275,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 275, 275, 275, 275, 279,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
    279, 279, 275, 275, 275, 275, 279, 279, 279, 279, 279, 279,
    279, 279, 279, 279, 279, 279, 279, 275, 275, 275, 275, 279,
    279, 279, 279, 279, 279, 279, 279, 275, 275, 275, 279, 279,
    279, 279, 279, 275, 275, 275, 279, 279, 279, 275, 279, 279,
    275, 279, 279, 275, 275, 275, 275, 275, 275, 275, 275, 0,
    275, 275, 275, 275, 275, 0, 0, 0
};


static const short yy_nxt[368] =  /* 0x004b4260 */
{
    0, 8, 10, 9, 11, 12, 13, 14, 15, 16, 8, 8,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, 26,
    25, 27, 8, 28, 25, 25, 29, 30, 31, 32, 33, 25,
    34, 35, 36, 37, 38, 25, 25, 25, 39, 40, 25, 25,
    41, 25, 25, 25, 42, 43, 44, 9, 9, 50, 52, 59,
    53, 60, 46, 46, 62, 82, 59, 54, 60, 63, 69, 55,
    56, 64, 57, 72, 70, 76, 51, 77, 85, 50, 66, 83,
    73, 61, 86, 59, 274, 60, 64, 74, 87, 75, 64, 88,
    99, 139, 99, 149, 160, 100, 51, 191, 192, 237, 273, 161,
    272, 150, 140, 271, 270, 269, 268, 267, 266, 265, 238, 8,
    8, 8, 45, 45, 45, 49, 264, 49, 98, 263, 98, 262,
    261, 260, 259, 258, 257, 256, 255, 254, 253, 252, 251, 250,
    249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 236,
    235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224,
    223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212,
    211, 210, 209, 208, 207, 206, 205, 204, 203, 202, 201, 200,
    199, 198, 197, 196, 195, 194, 193, 190, 189, 188, 187, 186,
    185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174,
    173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162,
    159, 158, 157, 156, 155, 154, 153, 152, 151, 148, 147, 146,
    145, 144, 143, 142, 141, 138, 137, 136, 135, 134, 133, 132,
    131, 130, 100, 100, 129, 128, 127, 126, 125, 124, 123, 122,
    121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110,
    109, 108, 107, 106, 105, 104, 103, 102, 101, 61, 97, 96,
    95, 94, 93, 92, 47, 91, 90, 89, 84, 81, 80, 79,
    78, 71, 68, 67, 65, 61, 58, 48, 47, 275, 9, 9,
    7, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 0
};


static const short yy_chk[368] =  /* 0x004b4540 */
{
    0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 5, 6, 12, 13, 18,
    13, 18, 5, 6, 20, 38, 21, 13, 21, 20, 30, 13,
    13, 21, 13, 32, 30, 33, 12, 33, 40, 49, 279, 38,
    32, 61, 40, 60, 273, 60, 61, 32, 40, 32, 60, 40,
    64, 110, 64, 122, 134, 64, 49, 169, 169, 220, 272, 134,
    271, 122, 110, 270, 269, 268, 267, 264, 263, 262, 220, 276,
    276, 276, 277, 277, 277, 278, 261, 278, 280, 260, 280, 259,
    257, 253, 252, 250, 249, 248, 247, 246, 245, 243, 241, 239,
    238, 237, 236, 234, 232, 231, 230, 229, 228, 227, 223, 219,
    218, 217, 216, 215, 214, 213, 212, 209, 208, 207, 206, 205,
    204, 203, 202, 201, 200, 199, 198, 195, 194, 193, 192, 191,
    190, 189, 188, 187, 185, 184, 183, 182, 181, 179, 178, 177,
    176, 175, 174, 173, 172, 171, 170, 168, 167, 166, 165, 164,
    163, 161, 160, 159, 158, 157, 156, 155, 154, 152, 151, 150,
    149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 137,
    133, 132, 131, 129, 128, 127, 126, 125, 123, 121, 120, 119,
    117, 115, 114, 112, 111, 109, 108, 107, 106, 105, 104, 103,
    102, 101, 100, 99, 97, 96, 95, 94, 93, 92, 89, 88,
    87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76,
    75, 74, 73, 72, 71, 70, 69, 68, 67, 59, 57, 56,
    55, 54, 53, 52, 47, 46, 43, 41, 39, 37, 36, 35,
    34, 31, 29, 26, 23, 19, 14, 11, 10, 7, 2, 1,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275, 275,
    275, 275, 275, 275, 275, 275, 275, 0
};


static const char *s_tokenNames[51] =  /* 0x004b4820 */
{
    "TOKEN_EOL",
    "TOKEN_IDENTIFIER",
    "TOKEN_STRING",
    "TOKEN_LEFT_BRACES",
    "TOKEN_RIGHT_BRACES",
    "TOKEN_LEFT_BRACKET",
    "TOKEN_RIGHT_BRACKET",
    "TOKEN_LEFT_SQUARE_BRACKET",
    "TOKEN_RIGHT_SQUARE_BRACKET",
    "TOKEN_LOGICAL_OR",
    "TOKEN_LOGICAL_AND",
    "TOKEN_NOT",
    "TOKEN_ARITHMETIC_OR",
    "TOKEN_INTEGER",
    "TOKEN_FLOAT",
    "TOKEN_HCONTAINER",
    "TOKEN_VCONTAINER",
    "TOKEN_SCROLLBOX",
    "TOKEN_SIMPLELISTBOX",
    "TOKEN_EDIT",
    "TOKEN_MULTIEDIT",
    "TOKEN_SPINEDIT",
    "TOKEN_FLOATEDIT",
    "TOKEN_FILEEDIT",
    "TOKEN_MULTIFILEEDIT",
    "TOKEN_ENUM",
    "TOKEN_INT_TYPE",
    "TOKEN_FLOAT_TYPE",
    "TOKEN_COLOR_TYPE",
    "TOKEN_STRUCT",
    "TOKEN_MAP",
    "TOKEN_LISTBOX",
    "TOKEN_BUTTON",
    "TOKEN_CHECKBOX",
    "TOKEN_COLORPICKER",
    "TOKEN_EVENT",
    "TOKEN_EXEC",
    "TOKEN_COMMA",
    "TOKEN_STRING_TYPE",
    "TOKEN_EQUALITY",
    "TOKEN_INEQUALITY",
    "TOKEN_NULL",
    "TOKEN_CONDITIONAL",
    "TOKEN_COLON",
    "TOKEN_REF",
    "TOKEN_DATA",
    "TOKEN_DATANAME",
    "TOKEN_DATAVIEW",
    "TOKEN_FILTER",
    "TOKEN__DEFINE",
    "TOKEN_KEYENUM"
};


static const short s_ruleToken[69] =  /* 0x0041e506 */
{
    0, 0, 0, 0, 0, 0, 0, 259, 260, 261, 262, 263,
    264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275,
    276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287,
    288, 289, 290, 291, 292, 293, 295, 306, 306, 306, 306, 306,
    306, 306, 294, 296, 297, 298, 299, 300, 301, 302, 303, 304,
    305, 307, 258, 0, 0, 0, 0, 0, 0
};

#define YYMAXDEPTH          10000

typedef struct
{
    parseVal_t value;
    int        pos;
} parseStackVal_t;

static const char *s_text;                              /* 0x20505dd8 */
static int         s_pos;                               /* 0x20505dd4 */
static int         s_start;                             /* 0x20505e44 */
static int         s_lastToken;                         /* 0x20505dbc */
static int         s_braceDepth;                        /* 0x20505dc4 */
static int         s_lineNumber;                        /* 0x205074d4 */
static int         s_errorPos;                          /* 0x20505db8 */
static parseVal_t  s_root;                              /* 0x20505dc8 */
static parseVal_t  s_lval;                              /* 0x205074c8 */
static int         s_lpos;                              /* 0x205074cc */
static std::string s_yytext;                            /* 0x205074c4 */


/* Grammar_Unescape  0x0041e2c0 */
static char *Grammar_Unescape( const char *text, int len )
{
    char *result;
    char *out;

    result = (char *)malloc( len + 1 );
    out    = result;

    while ( len )
    {
        if ( *text == '\\' )
        {
            len--;

            if ( !len )
            {
                break;
            }

            text++;

            switch ( *text )
            {
            case 'n':
                *out = '\n';
                break;

            case 'r':
                *out = '\r';
                break;

            case 't':
                *out = '\t';
                break;

            default:
                *out = *text;
                break;
            }

            out++;
            len--;
        }
        else
        {
            len--;
            *out++ = *text;
        }

        text++;
    }

    *out = 0;

    return result;
}


/* yylex  0x0041e370 */
static int Grammar_Lex( void )
{
    int state;
    int cp;
    int lastState;
    int lastCp;
    int c;
    int act;
    int start;

    for ( ;; )
    {
        if ( !s_text[s_pos] )
        {
            return 0;
        }

        state     = s_start;
        cp        = s_pos;
        lastState = 0;
        lastCp    = s_pos;

        do
        {
            c = yy_ec[(unsigned char)s_text[cp]];

            if ( yy_accept[state] )
            {
                lastState = state;
                lastCp    = cp;
            }

            while ( yy_chk[yy_base[state] + c] != state )
            {
                state = yy_def[state];

                if ( state >= YY_METASTATE )
                {
                    c = yy_meta[c];
                }
            }

            state = yy_nxt[yy_base[state] + c];
            cp++;
        }
        while ( yy_base[state] != YY_JAMBASE );

        act = yy_accept[state];

        if ( !act )
        {
            cp    = lastCp;
            state = lastState;
            act   = yy_accept[state];
        }

        if ( act >= YY_END_OF_BUFFER )
        {
            return 0;
        }

        start = s_pos;

        s_yytext.assign( s_text + start, cp - start );

        s_lpos     = start;
        s_errorPos = start;
        s_pos      = cp;

        switch ( act )
        {
        case 1:
            s_lineNumber++;
            break;

        case 2:
            s_start = 3;
            break;

        case 3:
        case 4:
        case 5:
            break;

        case 6:
            s_start = 5;
            break;

        case 7:
            s_lval.string = Grammar_Unescape( s_text + start + 1, cp - start - 2 );
            s_lastToken   = s_ruleToken[act];
            return s_lastToken;

        case 18:
            Legacy_sscanf( s_yytext.c_str(), "%d", &s_lval.integer );
            s_lastToken    = s_ruleToken[act];
            return s_lastToken;

        case 19:
            Legacy_sscanf( s_yytext.c_str(), "%f", &s_lval.number );
            s_lastToken = s_ruleToken[act];
            return s_lastToken;

        case 62:
            s_lval.string = (char *)malloc( s_yytext.size() + 1 );
            strcpy( s_lval.string, s_yytext.c_str() );
            s_lastToken = s_ruleToken[act];
            return s_lastToken;

        case 63:
            Grammar_Error( start, "bad token:\n%s:\n%s\n",
                           s_tokenNames[s_lastToken - TOKEN_FIRST], s_yytext.c_str() );
            break;

        case 64:
            fwrite( s_yytext.c_str(), s_yytext.size(), 1, stdout );
            break;

        default:
            s_lastToken = s_ruleToken[act];
            return s_lastToken;
        }
    }
}


/* yyerror  0x0041fc40 */
static void Grammar_ParseError( int token )
{
    if ( !token )
    {
        if ( s_braceDepth )
        {
            Grammar_Error( s_errorPos, "unmatching {} pair" );
        }

        Grammar_Error( s_errorPos, "unexpected end of file found" );
    }

    Grammar_Error( s_errorPos, "bad token:\n%s:\n%s\n",
                   s_tokenNames[s_lastToken - TOKEN_FIRST], s_yytext.c_str() );
}


#define VAL( n )    ( values[base + ( n ) - 1].value )
#define POS( n )    ( Grammar_Int( values[base + ( n ) - 1].pos ) )


/* yyparse  0x0041d810 */
static int Grammar_Parse( void )
{
    std::vector< int >             states;
    std::vector< parseStackVal_t > values;
    parseStackVal_t                result;
    parseVal_t                     none;
    int                            token;
    int                            sym;
    int                            n;
    int                            rule;
    int                            len;
    int                            base;
    int                            lhs;
    int                            g;

    none.integer = 0;
    result.value = none;
    result.pos   = 0;

    states.push_back( 0 );
    values.push_back( result );

    token = -2;

    for ( ;; )
    {
        if ( (int)states.size() >= YYMAXDEPTH )
        {
            Grammar_ParseError( token );
        }

        n = yypact[states.back()];

        if ( n != YYFLAG )
        {
            if ( token == -2 )
            {
                token = Grammar_Lex();
            }

            if ( token <= 0 )
            {
                token = 0;
                sym   = 0;
            }
            else
            {
                sym = (unsigned)token <= YYMAXUTOK ? yytranslate[token] : YYUNDEFTOK;
            }

            n += sym;

            if ( n >= 0 && n <= YYLAST && yycheck[n] == sym )
            {
                n = yytable[n];

                if ( n > 0 )
                {
                    if ( n == YYFINAL )
                    {
                        return 0;
                    }

                    states.push_back( n );

                    result.value = s_lval;
                    result.pos   = s_lpos;
                    values.push_back( result );

                    if ( token )
                    {
                        token = -2;
                    }

                    continue;
                }

                if ( !n || n == YYFLAG )
                {
                    Grammar_ParseError( token );
                }

                rule = -n;
            }
            else
            {
                rule = yydefact[states.back()];
            }
        }
        else
        {
            rule = yydefact[states.back()];
        }

        if ( !rule )
        {
            Grammar_ParseError( token );
        }

        len  = yyr2[rule];
        base = (int)values.size() - len;

        if ( len > 0 )
        {
            result = values[base];
        }

        switch ( rule )
        {
        case 6:
            s_root = Grammar_Node3( 1, VAL( 2 ), VAL( 3 ) );
            break;

        case 7:
            result.value = Grammar_Node3( 2, VAL( 2 ), POS( 1 ) );
            break;

        case 8:
            result.value = Grammar_Node3( 3, VAL( 2 ), POS( 1 ) );
            break;

        case 9:
            result.value = Grammar_Node3( 4, VAL( 3 ), POS( 1 ) );
            break;

        case 10:
            result.value = Grammar_Node3( 5, VAL( 3 ), POS( 1 ) );
            break;

        case 11:
            result.value = Grammar_Node3( 6, VAL( 3 ), POS( 1 ) );
            break;

        case 12:
            result.value = Grammar_Node3( 7, VAL( 3 ), POS( 1 ) );
            break;

        case 13:
            result.value = Grammar_Node5( 10, VAL( 3 ), VAL( 5 ), VAL( 7 ), POS( 1 ) );
            break;

        case 14:
            result.value = Grammar_Node5( 11, VAL( 3 ), VAL( 5 ), VAL( 7 ), POS( 1 ) );
            break;

        case 15:
            result.value = Grammar_Node3( 12, VAL( 3 ), POS( 1 ) );
            break;

        case 16:
            result.value = Grammar_Node3( 13, VAL( 3 ), POS( 1 ) );
            break;

        case 17:
            result.value = Grammar_Node4( 27, VAL( 3 ), VAL( 5 ), POS( 1 ) );
            break;

        case 18:
            result.value = Grammar_Node4( 28, VAL( 3 ), VAL( 5 ), POS( 1 ) );
            break;

        case 19:
            result.value = Grammar_Node3( 9, VAL( 3 ), POS( 1 ) );
            break;

        case 20:
            result.value = Grammar_Node3( 8, VAL( 3 ), POS( 1 ) );
            break;

        case 21:
            result.value = Grammar_Node3( 46, VAL( 3 ), POS( 1 ) );
            break;

        case 22:
        case 71:
            result.value = Grammar_Node3( 29, VAL( 1 ), VAL( 3 ) );
            break;

        case 23:
        case 26:
        case 28:
        case 34:
        case 39:
        case 44:
        case 72:
            result.value = Grammar_ListAppend( VAL( 1 ), VAL( len ) );
            break;

        case 24:
        case 31:
        case 37:
        case 40:
        case 45:
        case 73:
            result.value = Grammar_ListCreate( Grammar_Node1( 0 ) );
            break;

        case 25:
            result.value = VAL( 2 );
            break;

        case 27:
        case 29:
        case 35:
            result.value = Grammar_ListAppend( Grammar_ListCreate( Grammar_Node1( 0 ) ), VAL( 1 ) );
            break;

        case 32:
            s_braceDepth++;
            break;

        case 33:
            s_braceDepth--;
            break;

        case 38:
            result.value = Grammar_Node3( 33, VAL( 1 ), VAL( 3 ) );
            break;

        case 41:
            result.value = Grammar_Node3( 30, VAL( 2 ), VAL( 4 ) );
            break;

        case 42:
            result.value = Grammar_Node2( 31, VAL( 3 ) );
            break;

        case 43:
            result.value = Grammar_Node4( 47, VAL( 3 ), VAL( 5 ), POS( 1 ) );
            break;

        case 46:
            result.value = Grammar_Node3( 18, VAL( 3 ), POS( 1 ) );
            break;

        case 47:
            result.value = Grammar_Node5( 19, VAL( 2 ), VAL( 3 ), VAL( 4 ), POS( 1 ) );
            break;

        case 48:
            result.value = Grammar_Node5( 20, VAL( 2 ), VAL( 3 ), VAL( 4 ), POS( 1 ) );
            break;

        case 49:
            result.value = Grammar_Node6( 21, VAL( 2 ), VAL( 3 ), VAL( 4 ), VAL( 5 ), POS( 1 ) );
            break;

        case 50:
            result.value = Grammar_Node3( 24, VAL( 3 ), POS( 1 ) );
            break;

        case 51:
            result.value = Grammar_Node3( 25, VAL( 2 ), POS( 1 ) );
            break;

        case 52:
            result.value = Grammar_Node1( 34 );
            break;

        case 53:
            result.value = Grammar_Node2( 35, VAL( 2 ) );
            break;

        case 54:
            result.value = Grammar_Node3( 42, VAL( 3 ), POS( 1 ) );
            break;

        case 55:
            result.value = Grammar_Node2( 44, POS( 1 ) );
            break;

        case 56:
            result.value = Grammar_Node2( 45, POS( 1 ) );
            break;

        case 57:
            result.value = Grammar_Node3( 48, VAL( 3 ), POS( 1 ) );
            break;

        case 58:
            result.value = Grammar_Node3( 26, VAL( 1 ), POS( 1 ) );
            break;

        case 60:
            result.value = Grammar_Node4( 36, VAL( 1 ), VAL( 3 ), POS( 2 ) );
            break;

        case 61:
            result.value = Grammar_Node4( 37, VAL( 1 ), VAL( 3 ), POS( 2 ) );
            break;

        case 62:
            result.value = Grammar_Node3( 38, VAL( 1 ), POS( 1 ) );
            break;

        case 63:
            result.value = Grammar_Node3( 39, VAL( 1 ), POS( 1 ) );
            break;

        case 64:
            result.value = Grammar_Node2( 40, POS( 1 ) );
            break;

        case 65:
            result.value = Grammar_Node5( 41, VAL( 1 ), VAL( 3 ), VAL( 5 ), POS( 2 ) );
            break;

        case 66:
            result = values[base + 1];
            break;

        case 67:
            result.value = Grammar_Node4( 14, VAL( 1 ), VAL( 3 ), POS( 2 ) );
            break;

        case 68:
            result.value = Grammar_Node4( 15, VAL( 1 ), VAL( 3 ), POS( 2 ) );
            break;

        case 69:
            result.value = Grammar_Node7( 32, VAL( 1 ), VAL( 2 ), POS( 1 ), none, none, none );
            break;

        case 70:
            result.value = Grammar_Node8( 43, VAL( 1 ), VAL( 2 ), VAL( 4 ), POS( 1 ), none, none, none );
            break;

        case 74:
            result.value = Grammar_Node3( 23, VAL( 1 ), POS( 1 ) );
            break;

        case 75:
            result.value = Grammar_Node3( 22, VAL( 1 ), POS( 1 ) );
            break;
        }

        states.resize( states.size() - len );
        values.resize( values.size() - len );

        lhs = yyr1[rule] - YYNTBASE;
        g   = yypgoto[lhs] + states.back();

        if ( g >= 0 && g <= YYLAST && yycheck[g] == states.back() )
        {
            states.push_back( yytable[g] );
        }
        else
        {
            states.push_back( yydefgoto[lhs] );
        }

        values.push_back( result );
    }
}


/* Grammar_ParseText  0x0041fcc0 */
bool Grammar_ParseText( const char *text, parseVal_t *root )
{
    s_text       = text;
    s_pos        = 0;
    s_start      = 3;
    s_lastToken  = TOKEN_FIRST;
    s_braceDepth = 0;
    s_lineNumber = 0;
    s_errorPos   = 0;

    if ( Grammar_Parse() )
    {
        return false;
    }

    *root = s_root;

    return true;
}
