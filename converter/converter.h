/* Original: .\converter.h */

#ifndef CONVERTER_H
#define CONVERTER_H

#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

#include "library.h"

#define BUFFER_LEN          2048
#define COMPARE_BUFFER_LEN  20000
#define MAX_CONVERT_ARGS    256

typedef enum
{
    CONVERTER_PLATFORM_NOT_SET,
    CONVERTER_PLATFORM_PC,
    CONVERTER_PLATFORM_XENON,
    CONVERTER_PLATFORM_PS3,

    CONVERTER_PLATFORM_COUNT
} converterPlatform_t;

typedef enum
{
    ASSET_DIR_DEFAULT,
    ASSET_DIR_SHARED,
    ASSET_DIR_PLATFORM,
    ASSET_DIR_CURRENT,

    ASSET_DIR_COUNT
} assetDirType_t;

typedef enum
{
    CONVERT_REASON_MISSING_FILE,
    CONVERT_REASON_MISSING_CACHE_ENTRY,
    CONVERT_REASON_OUT_OF_DATE,
    CONVERT_REASON_CLEAN_CONVERT,
    CONVERT_REASON_NEW_VERSION,
    CONVERT_REASON_ENTRY_CHANGED,
    CONVERT_REASON_CACHE_DIRTY,
    CONVERT_REASON_NEW_SOURCE_FILE,
    CONVERT_REASON_CHANGED_SOURCE_FILE,

    CONVERT_REASON_COUNT
} convertReason_t;

typedef struct
{
    int   num;          /* +0x00 */
    int   len;          /* +0x04 */
    char *names[1];     /* +0x08 */
} fileList_t;

typedef std::map< std::string, fileList_t * > fileListMap_t;

typedef struct
{
    char *buf;          /* +0x00 */
    int   len;          /* +0x04 */
} gdtBuf_t;

class ValueScope;

typedef std::map< std::string, gdtBuf_t >     gdtFileMap_t;
typedef std::map< std::string, ValueScope * > gdtEntryMap_t;

typedef enum
{
    CONVERT_STATE_DONE,
    CONVERT_STATE_PENDING,
    CONVERT_STATE_FAILED
} convertState_t;

class ObjectInfo;
class Variable;
class ValueScope;

class ConvertInfo
{
public:
    ConvertInfo();                                          /* 0x0040b2a0 */
    ~ConvertInfo();                                         /* 0x0040b310 */

    ConvertFunc_t              Convert;                     /* +0x00 */
    std::string                m_SourceDirectory;           /* +0x04 */
    std::vector< std::string > m_TargetDirectory;           /* +0x20 */
    std::vector< std::string > m_RelativeTargetDirectory;   /* +0x30 */
    Variable                  *m_entry;                     /* +0x40 */
    bool                       m_versionChanged;            /* +0x44 */
    ValueScope                *m_cachedScope[2];            /* +0x48 */
    std::string                m_gdfName;                   /* +0x50 */
    int                        m_version;                   /* +0x6c */
    bool                       m_marked;                    /* +0x70 */
    bool                       m_error;                     /* +0x71 */
    std::string                m_typeName;                  /* +0x78 */
    int                        m_index;                     /* +0x94 */
    int                        m_count;                     /* +0x98 */
};

extern int          g_platform;                             /* 0x004f515c */

extern std::string  g_assetDir[ASSET_DIR_COUNT];            /* 0x20505fd0 */
extern std::string  g_srcCacheDir[ASSET_DIR_COUNT];         /* 0x20505e78 */
extern std::string  g_tgtCacheDir[ASSET_DIR_COUNT];         /* 0x20505f40 */

extern std::string  g_platformNames[CONVERTER_PLATFORM_COUNT];  /* 0x00486238 */
extern int          g_verbose;                              /* 0x20505de8 */

extern std::string    g_assetChangeListDir;                 /* 0x004b48ec */
extern bool           g_spam;                               /* 0x004b2854 */
extern bool           g_preprocess;                         /* 0x20505df4 */
extern bool           g_cacheEntry;                         /* 0x20505e00 */

extern gdtFileMap_t   g_convertFileNames;                   /* 0x20505ef4 */
extern gdtEntryMap_t  g_gdtEntries;                         /* 0x20505f00 */
extern gdtEntryMap_t  g_cachedEntries[ASSET_DIR_COUNT];
extern gdtEntryMap_t  g_changeListEntries;                  /* 0x205060a0 */

void  AddGdtFile( std::string relFileName, bool useChangeList );   /* 0x0040c9a0 */
void  AddGdtFileCallback( const std::string &fileName );           /* 0x0040ce60 */
void  ForEachGdtDir( void ( *callback )( const std::string &fileName ) );  /* 0x00424b60 */

void  Error( const char *message );                         /* 0x00402460 */
void  AddFile( const char *fileName, int type );            /* 0x00403bc0 */
void  ErrorPrompt( void );                                  /* 0x00402310 */
void  ConvertPath( char *dest, const char *src, int count ); /* 0x00402250 */

#endif
