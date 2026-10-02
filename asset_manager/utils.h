/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\utils.h */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include <string>

typedef struct
{
    int   maxsize;      /* +0x00 */
    char *string;       /* +0x04 */
    char *curpos;       /* +0x08 */
} buf_t;

void        InitBuf( buf_t *buf, char *string, int maxsize );                   /* 0x0042f060 */
void        WriteIndent( buf_t *buf, int indent );                              /* 0x0042f080 */
char       *ReadLine( FILE *fp, char *buf, int size );                          /* 0x0042f180 */
void        SetBaseDir( const char *currDir, int levelsUp );                    /* 0x00430e30 */
std::string EscapeString( std::string str );                                    /* 0x00430cc0 */
std::string MakeRelativePath( std::string base, std::string full );             /* 0x00431040 */
unsigned    CRC32( const char *data, int len, unsigned crc );                   /* 0x00420870 */
long        Legacy_atol( const char *nptr );
double      Legacy_atof( const char *nptr );
int         Legacy_vsprintf( char *buf, const char *fmt, va_list args );
int         Legacy_sprintf( char *buf, const char *fmt, ... );
int         Legacy_vsscanf( const char *text, const char *fmt, va_list args );
int         Legacy_sscanf( const char *text, const char *fmt, ... );

extern std::string g_baseDir;                                                   /* 0x00486160 */
extern std::string g_srcCacheBaseDir;                                           /* 0x00486200 */
extern std::string g_tgtCacheBaseDir;                                           /* 0x0048621c */

#endif
