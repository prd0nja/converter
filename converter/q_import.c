/* Original: .\q_import.c */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <mmsystem.h>
#include <intrin.h>

#include "q_import.h"
#include "assertive.h"


static char  com_token[MAX_TOKEN_CHARS + 1];        /* 0x204f5a38 */
static int   com_lines;                             /* 0x204f5e40 */
static int   com_backupLines;                       /* 0x204fe0a8 */
static char *com_backupScript;                      /* 0x20505db0 */


/* COM_RestoreParseSession  0x0041c610 */
void COM_RestoreParseSession( char **data_p )
{
    com_lines = com_backupLines;
    *data_p   = com_backupScript;
}


/* SkipWhitespace  0x0041c630 */
static char *SkipWhitespace( char *data, qboolean *hasNewLines )
{
    int c;

    while ( ( c = *data ) <= ' ' )
    {
        if ( c == '\n' )
        {
            com_lines++;
            *hasNewLines = qtrue;
        }
        else if ( !c )
        {
            return NULL;
        }

        data++;
    }

    return data;
}


/* COM_ParseExt  0x0041c670 */
char *COM_ParseExt( char **data_p, qboolean allowLineBreaks )
{
    int      c;
    int      len;
    qboolean hasNewLines;
    char    *data;

    data         = *data_p;
    len          = 0;
    hasNewLines  = qfalse;
    com_token[0] = 0;

    if ( !data )
    {
        *data_p = NULL;
        return com_token;
    }

    com_backupLines  = com_lines;
    com_backupScript = *data_p;

    while ( 1 )
    {
        data = SkipWhitespace( data, &hasNewLines );

        if ( !data )
        {
            *data_p = NULL;
            return com_token;
        }

        if ( hasNewLines && !allowLineBreaks )
        {
            *data_p = data;
            return com_token;
        }

        c = *data;

        if ( c == '/' && data[1] == '/' )
        {
            data += 2;
            while ( *data && *data != '\n' )
            {
                data++;
            }
        }
        else if ( c == '/' && data[1] == '*' )
        {
            data += 2;
            while ( *data && ( *data != '*' || data[1] != '/' ) )
            {
                data++;
            }
            if ( *data )
            {
                data += 2;
            }
        }
        else
        {
            break;
        }
    }

    if ( c == '\"' )
    {
        data++;

        while ( 1 )
        {
            c = *data++;

            if ( c == '\"' || !c )
            {
                com_token[len] = 0;
                *data_p = data;
                return com_token;
            }

            if ( c == '\\' )
            {
                c = *data++;

                switch ( c )
                {
                case '\"':
                case '\\':
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                default:
                    com_token[0] = 0;
                    *data_p = NULL;
                    return com_token;
                }
            }

            if ( len < MAX_TOKEN_CHARS )
            {
                com_token[len] = (char)c;
                len++;
            }
        }
    }

    do
    {
        if ( len < MAX_TOKEN_CHARS )
        {
            com_token[len] = (char)c;
            len++;
        }

        data++;
        c = *data;

        if ( c == '\n' )
        {
            com_lines++;
        }
    }
    while ( c > ' ' );

    if ( len == MAX_TOKEN_CHARS )
    {
        len = 0;
    }

    com_token[len] = 0;

    *data_p = data;
    return com_token;
}


/* COM_Parse  0x0041c8a0 */
char *COM_Parse( char **data_p )
{
    return COM_ParseExt( data_p, qtrue );
}


/* Q_strncpyz  0x0041c8b0 */
void Q_strncpyz( char *dest, const char *src, int destsize )
{
    assert( src );
    assert( destsize >= 1 );

    strncpy( dest, src, destsize - 1 );
    dest[destsize - 1] = 0;
}


/* Q_islower  0x0041c950 */
static int Q_islower( int c )
{
    return c >= 'a' && c <= 'z';
}


/* Q_stricmpn  0x0041c970 */
int Q_stricmpn( const char *s1, const char *s2, int n )
{
    int c1;
    int c2;

    do
    {
        c1 = *s1++;
        c2 = *s2++;

        if ( !n-- )
        {
            return 0;
        }

        if ( c1 != c2 )
        {
            if ( Q_islower( c1 ) )
            {
                c1 -= 'a' - 'A';
            }
            if ( Q_islower( c2 ) )
            {
                c2 -= 'a' - 'A';
            }
            if ( c1 != c2 )
            {
                return c1 < c2 ? -1 : 1;
            }
        }
    }
    while ( c1 );

    return 0;
}


/* Q_stricmp  0x0041c9d0 */
int Q_stricmp( const char *s1, const char *s2 )
{
    if ( !s1 || !s2 )
    {
        return -1;
    }

    return Q_stricmpn( s1, s2, 99999 );
}


static char com_vaBigBuffer[BIG_INFO_STRING];       /* 0x204fe0b0 */
static char com_vaBuffer[BIG_INFO_STRING];          /* 0x204f5e48 */
static int  com_vaIndex;                            /* 0x20505e18 */


/* va  0x0041c9f0 */
char *va( const char *format, ... )
{
    va_list  argptr;
    char    *buf;
    int      len;

    va_start( argptr, format );
    vsprintf( com_vaBigBuffer, format, argptr );
    va_end( argptr );

    len = (int)strlen( com_vaBigBuffer );

    assert( len < BIG_INFO_STRING );

    if ( com_vaIndex + len >= BIG_INFO_STRING - 1 )
    {
        com_vaIndex = 0;
    }

    buf = &com_vaBuffer[com_vaIndex];
    memcpy( buf, com_vaBigBuffer, len + 1 );
    com_vaIndex += len + 1;

    return buf;
}


/* Com_sprintf  0x0041caa0 */
void Com_sprintf( char *dest, int size, const char *fmt, ... )
{
    int     len;
    va_list argptr;
    char    bigbuffer[BIG_INFO_STRING + 4];

    va_start( argptr, fmt );
    len = vsprintf( bigbuffer, fmt, argptr );
    va_end( argptr );

    assert( (unsigned int)len < BIG_INFO_STRING );
    assert( len < size );

    Q_strncpyz( dest, bigbuffer, size );
}


/* CopyString  0x0041cb80 */
char *CopyString( const char *in )
{
    char *out;

    out = (char *)malloc( strlen( in ) + 1 );
    strcpy( out, in );

    return out;
}


/* Com_StringContains  0x0041cbb0 */
static const char *Com_StringContains( const char *str1, const char *str2, int casesensitive )
{
    int len;
    int i;
    int j;

    len = (int)strlen( str1 ) - (int)strlen( str2 );

    for ( i = 0; i <= len; i++, str1++ )
    {
        for ( j = 0; str2[j]; j++ )
        {
            if ( casesensitive )
            {
                if ( str1[j] != str2[j] )
                {
                    break;
                }
            }
            else
            {
                if ( toupper( str1[j] ) != toupper( str2[j] ) )
                {
                    break;
                }
            }
        }

        if ( !str2[j] )
        {
            return str1;
        }
    }

    return NULL;
}


/* Com_Filter  0x0041cc70 */
qboolean Com_Filter( const char *filter, const char *name, int casesensitive )
{
    char        buf[MAX_TOKEN_CHARS];
    const char *ptr;
    int         i;
    int         found;

    while ( *filter )
    {
        if ( *filter == '*' )
        {
            filter++;

            if ( !*filter )
            {
                while ( *name )
                {
                    if ( *name == '.' )
                    {
                        break;
                    }
                    name++;
                }
                continue;
            }

            for ( i = 0; *filter; i++ )
            {
                if ( *filter == '*' || *filter == '?' )
                {
                    break;
                }
                buf[i] = *filter;
                filter++;
            }
            buf[i] = 0;

            if ( strlen( buf ) )
            {
                ptr = Com_StringContains( name, buf, casesensitive );
                if ( !ptr )
                {
                    return qfalse;
                }
                name = ptr + strlen( buf );
            }
        }
        else if ( *filter == '?' )
        {
            filter++;
            name++;
        }
        else if ( *filter == '[' && *( filter + 1 ) == '[' )
        {
            filter++;
        }
        else if ( *filter == '[' )
        {
            filter++;
            found = qfalse;

            while ( *filter && !found )
            {
                if ( *filter == ']' && *( filter + 1 ) != ']' )
                {
                    break;
                }

                if ( *( filter + 1 ) == '-' && *( filter + 2 )
                     && ( *( filter + 2 ) != ']' || *( filter + 3 ) == ']' ) )
                {
                    if ( casesensitive )
                    {
                        if ( *name >= *filter && *name <= *( filter + 2 ) )
                        {
                            found = qtrue;
                        }
                    }
                    else
                    {
                        if ( toupper( *name ) >= toupper( *filter )
                             && toupper( *name ) <= toupper( *( filter + 2 ) ) )
                        {
                            found = qtrue;
                        }
                    }
                    filter += 3;
                }
                else
                {
                    if ( casesensitive )
                    {
                        if ( *filter == *name )
                        {
                            found = qtrue;
                        }
                    }
                    else
                    {
                        if ( toupper( *filter ) == toupper( *name ) )
                        {
                            found = qtrue;
                        }
                    }
                    filter++;
                }
            }

            if ( !found )
            {
                return qfalse;
            }

            while ( *filter )
            {
                if ( *filter == ']' && *( filter + 1 ) != ']' )
                {
                    break;
                }
                filter++;
            }

            filter++;
            name++;
        }
        else
        {
            if ( casesensitive )
            {
                if ( *filter != *name )
                {
                    return qfalse;
                }
            }
            else
            {
                if ( toupper( *filter ) != toupper( *name ) )
                {
                    return qfalse;
                }
            }

            filter++;
            name++;
        }
    }

    return *name == 0;
}


/* Com_FilterPath  0x0041ceb0 */
qboolean Com_FilterPath( const char *filter, const char *name, int casesensitive )
{
    int  i;
    char new_filter[MAX_QPATH];
    char new_name[MAX_QPATH];

    for ( i = 0; i < MAX_QPATH - 1 && filter[i]; i++ )
    {
        if ( filter[i] == '\\' || filter[i] == ':' )
        {
            new_filter[i] = '/';
        }
        else
        {
            new_filter[i] = filter[i];
        }
    }
    new_filter[i] = 0;

    for ( i = 0; i < MAX_QPATH - 1 && name[i]; i++ )
    {
        if ( name[i] == '\\' || name[i] == ':' )
        {
            new_name[i] = '/';
        }
        else
        {
            new_name[i] = name[i];
        }
    }
    new_name[i] = 0;

    return Com_Filter( new_filter, new_name, casesensitive );
}


/* Sys_ListFilteredFiles_r  0x0041cf60 */
void Sys_ListFilteredFiles_r( const char *basedir, const char *subdirs, const char *filter,
                                     char **list, int *numfiles, int maxfiles )
{
    char            search[MAX_OS_PATH];
    char            filename[MAX_OS_PATH];
    struct _finddata_t findinfo;
    intptr_t        findhandle;
    int             more;

    if ( *numfiles >= maxfiles )
    {
        return;
    }

    if ( strlen( subdirs ) )
    {
        Com_sprintf( search, sizeof( search ), "%s\\%s\\*", basedir, subdirs );
    }
    else
    {
        Com_sprintf( search, sizeof( search ), "%s\\*", basedir );
    }

    Prof_Begin( PROF_FILE_LIST );
    findhandle = _findfirst( search, &findinfo );
    Prof_End();

    if ( findhandle == -1 )
    {
        return;
    }

    do
    {
        if ( !( findinfo.attrib & _A_SUBDIR ) )
        {
            if ( *numfiles >= maxfiles )
            {
                break;
            }

            Prof_Begin( PROF_FILE_FILTER );

            if ( strlen( subdirs ) )
            {
                Com_sprintf( filename, sizeof( filename ), "%s\\%s", subdirs, findinfo.name );
            }
            else
            {
                strcpy( filename, findinfo.name );
            }

            if ( Com_FilterPath( filter, filename, qfalse ) )
            {
                list[*numfiles] = CopyString( filename );
                ( *numfiles )++;
            }

            Prof_End();
        }

        Prof_Begin( PROF_FILE_LIST );
        more = _findnext( findhandle, &findinfo );
        Prof_End();
    }
    while ( more != -1 );

    Prof_Begin( PROF_FILE_LIST );
    _findclose( findhandle );
    Prof_End();
}


typedef struct
{
    __int64 start;      /* +0x00 */
    __int64 total;      /* +0x08 */
    __int64 self;       /* +0x10 */
    int     type;       /* +0x18 */
} profEntry_t;

static const char *g_profNames[PROF_COUNT] =        /* 0x004b2b00 */
{
    "unknown          ",
    "convert misc     ",
    "up to date misc  ",
    "mark files misc  ",
    "mkdir            ",
    "errors misc      ",
    "add key list misc",
    "add list total   ",
    "file open        ",
    "file seek        ",
    "file read        ",
    "file write       ",
    "file time check  ",
    "wildcard asset   ",
    "file list        ",
    "file delete      ",
    "file filter      ",
    "dll convert      ",
    "dll load         ",
    "compare gdt entry",
    "rel filename     ",
    "ref count        ",
    "cache file       ",
    "server perforce  ",
    "server sql       ",
    "server file copy ",
    "server checksum  ",
    "cache convert    ",
    "initialization   ",
    "add list process ",
    "add list cnstruct",
    "fromScript()     ",
    "data:fromScript()",
    "data:setFilename(",
    "data::toScript() ",
    "EmitDef()        ",
    "EmitVariables()  ",
    "EmitInfoList()   ",
    "EmitParameterList",
    "GetParseData()   ",
    "EmitEnumVariable ",
    "EmitEnumParamVar ",
    "EmitObjectInfo   "
};

static profEntry_t g_profEntries[PROF_COUNT];       /* 0x204fdb48 */
static int         g_profStack[PROF_DEPTH];         /* 0x204f59f8 */
static int         g_prof_depth;                    /* 0x20505e0c */
static int         g_profStartMsec;                 /* 0x204f5e38 */
static int         g_profTotalMsec;                 /* 0x204f5e3c */


/* Prof_Init  0x0041d340 */
void Prof_Init( void )
{
    int i;

    memset( g_profEntries, 0, sizeof( g_profEntries ) );
    g_prof_depth = 0;

    for ( i = 0; i < PROF_COUNT; i++ )
    {
        g_profEntries[i].type = i;
    }

    g_profTotalMsec = 0;
    g_profStartMsec = timeGetTime();

    Prof_Begin( PROF_UNKNOWN );
}


/* Prof_Finish  0x0041d390 */
void Prof_Finish( void )
{
    Prof_End();

    assert( !g_prof_depth );

    g_profTotalMsec += timeGetTime() - g_profStartMsec;
}


/* Prof_Compare  0x0041d3f0 */
static int __cdecl Prof_Compare( const void *pa, const void *pb )
{
    const profEntry_t *a = (const profEntry_t *)pa;
    const profEntry_t *b = (const profEntry_t *)pb;

    if ( a->self < b->self )
    {
        return -1;
    }

    return 1;
}


/* Prof_Print  0x0041d420 */
void Prof_Print( void )
{
    float  ticks;
    double msec;
    int    i;

    printf( "profile:\n\n" );

    ticks = (float)g_profEntries[0].total;

    qsort( g_profEntries, PROF_COUNT, sizeof( g_profEntries[0] ), Prof_Compare );

    for ( i = 0; i < PROF_COUNT; i++ )
    {
        msec = (double)g_profEntries[i].self * g_profTotalMsec * 0.001f / ticks;

        if ( msec >= 0.5f )
        {
            printf( "%s: %6.1f\n", g_profNames[g_profEntries[i].type], msec );
        }
    }

    printf( "total            : %6.1f\n", (double)g_profTotalMsec * 0.001f );
}


/* Prof_Ticks  0x0041d4e0 */
static __int64 Prof_Ticks( void )
{
    return __rdtsc();
}


/* Prof_Pause  0x0041d6b0 */
void Prof_Pause( void )
{
    __int64      now;
    __int64      delta;
    profEntry_t *entry;
    int          i;

    now = Prof_Ticks();

    g_profTotalMsec += timeGetTime() - g_profStartMsec;

    for ( i = 0; i < g_prof_depth; i++ )
    {
        entry = &g_profEntries[g_profStack[i]];
        delta = now - entry->start;

        entry->total += delta;
        entry->self  += delta;

        if ( i )
        {
            g_profEntries[g_profStack[i - 1]].self -= delta;
        }
    }
}


/* Prof_Resume  0x0041d780 */
void Prof_Resume( void )
{
    __int64 now;
    int     i;

    g_profStartMsec = timeGetTime();

    now = Prof_Ticks();

    for ( i = 0; i < g_prof_depth; i++ )
    {
        g_profEntries[g_profStack[i]].start = now;
    }
}


/* Prof_Begin  0x0041d520 */
void Prof_Begin( profType_t type )
{
    assert( g_prof_depth < PROF_DEPTH );

    g_profStack[g_prof_depth] = type;
    g_prof_depth++;

    g_profEntries[type].start = Prof_Ticks();
}


/* Prof_End  0x0041d5c0 */
void Prof_End( void )
{
    __int64      now;
    __int64      delta;
    profEntry_t *entry;

    now = Prof_Ticks();

    assert( g_prof_depth > 0 );

    entry = &g_profEntries[g_profStack[g_prof_depth - 1]];
    delta = now - entry->start;

    entry->total += delta;
    entry->self  += delta;

    g_prof_depth--;

    if ( g_prof_depth )
    {
        g_profEntries[g_profStack[g_prof_depth - 1]].self -= delta;
    }
}
