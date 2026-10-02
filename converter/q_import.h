/* Original: .\q_import.h */

#ifndef Q_IMPORT_H
#define Q_IMPORT_H

#include <stdarg.h>

#define qtrue   1
#define qfalse  0

typedef int qboolean;

#define MAX_QPATH           64
#define MAX_OS_PATH         256
#define MAX_TOKEN_CHARS     1024
#define BIG_INFO_STRING     32000

#define MAX_FOUND_FILES     0x1000

#ifdef __cplusplus
extern "C" {
#endif

void        COM_RestoreParseSession( char **data_p );
char       *COM_Parse( char **data_p );
char       *COM_ParseExt( char **data_p, qboolean allowLineBreaks );

void        Q_strncpyz( char *dest, const char *src, int destsize );
int         Q_stricmpn( const char *s1, const char *s2, int n );
int         Q_stricmp( const char *s1, const char *s2 );

char       *va( const char *format, ... );
void        Com_sprintf( char *dest, int size, const char *fmt, ... );
char       *CopyString( const char *in );

qboolean    Com_Filter( const char *filter, const char *name, int casesensitive );
qboolean    Com_FilterPath( const char *filter, const char *name, int casesensitive );

void        Sys_ListFilteredFiles_r( const char *basedir, const char *subdirs, const char *filter,
                                     char **list, int *numfiles, int maxfiles );

typedef enum
{
    PROF_UNKNOWN,
    PROF_CONVERT_MISC,
    PROF_UP_TO_DATE_MISC,
    PROF_MARK_FILES_MISC,
    PROF_MKDIR,
    PROF_ERRORS_MISC,
    PROF_ADD_KEY_LIST_MISC,
    PROF_ADD_LIST_TOTAL,
    PROF_FILE_OPEN,
    PROF_FILE_SEEK,
    PROF_FILE_READ,
    PROF_FILE_WRITE,
    PROF_FILE_TIME_CHECK,
    PROF_WILDCARD_ASSET,
    PROF_FILE_LIST,
    PROF_FILE_DELETE,
    PROF_FILE_FILTER,
    PROF_DLL_CONVERT,
    PROF_DLL_LOAD,
    PROF_COMPARE_GDT_ENTRY,
    PROF_REL_FILENAME,
    PROF_REF_COUNT,
    PROF_CACHE_FILE,
    PROF_SERVER_PERFORCE,
    PROF_SERVER_SQL,
    PROF_SERVER_FILE_COPY,
    PROF_SERVER_CHECKSUM,
    PROF_CACHE_CONVERT,
    PROF_INITIALIZATION,
    PROF_ADD_LIST_PROCESS,
    PROF_ADD_LIST_CONSTRUCT,
    PROF_FROM_SCRIPT,
    PROF_DATA_FROM_SCRIPT,
    PROF_DATA_SET_FILENAME,
    PROF_DATA_TO_SCRIPT,
    PROF_EMIT_DEF,
    PROF_EMIT_VARIABLES,
    PROF_EMIT_INFO_LIST,
    PROF_EMIT_PARAMETER_LIST,
    PROF_GET_PARSE_DATA,
    PROF_EMIT_ENUM_VARIABLE,
    PROF_EMIT_ENUM_PARAM_VAR,
    PROF_EMIT_OBJECT_INFO,

    PROF_COUNT
} profType_t;

#define PROF_DEPTH  16

void        Prof_Init( void );
void        Prof_Finish( void );
void        Prof_Print( void );
void        Prof_Pause( void );
void        Prof_Resume( void );
void        Prof_Begin( profType_t type );
void        Prof_End( void );

#ifdef __cplusplus
}
#endif

#endif
