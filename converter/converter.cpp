/* Original: .\converter.cpp */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <direct.h>
#include <mmsystem.h>

#include "converter.h"
#include "q_import.h"
#include "assertive.h"
#include "utils.h"
#include "parser.h"
#include "server.h"

#include <ctype.h>
#include <tlhelp32.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>

static void ProcessChangeList( void );                  /* 0x0040d600 */
static void SaveCacheFilesAtExit( void );               /* 0x00408d30 */
static void LoadChangeListFiles( void );                /* 0x0040d7d0 */
static void ProcessChangeListGdf( std::string gdfName );  /* 0x0040b5b0 */
static void ProcessChangeListEntries( void );           /* 0x0040b900 */
static void ProcessGdfFiles( void );                    /* 0x0040b9e0 */
static void ConvertGdfPass( const std::string &gdfName, std::set< std::string > *assets,
                            ObjectInfo *info, unsigned int pass );   /* 0x00409aa0 */

static void CacheConvertedFiles( void );                /* 0x0040d1d0 */
static void WriteMyChanges( void );                     /* 0x0040e5d0 */

static void RemoveGdtEntries( const std::string &relFileName );  /* 0x0040c050 */

typedef struct
{
    std::string  m_gdfName;     /* +0x00 */
    std::string  m_assetName;   /* +0x1c */
    int          m_size;        /* +0x38 */
    unsigned     m_checksum;    /* +0x3c */
    int          m_version;     /* +0x40 */
} targetCache_t;

static void ConvertError( ConvertData *data, const char *message );              /* 0x004024a0 */
static void AddFilesToList( fileList_t **list, std::set< std::string > *files ); /* 0x00402660 */
static void DeleteAssetFiles( const char *relName );                             /* 0x00403720 */
static void AddReferencedEntries( fileList_t *list, std::string name );          /* 0x004049d0 */
static void ClearRegeneratedFiles( fileListMap_t *cache, const std::string &name,
                              std::set< std::string > *files, bool *dirty );     /* 0x00404b60 */
static void RemoveCachedFiles( fileListMap_t *cache, ConvertInfo *info, std::string name,
                               bool *dirty );                                    /* 0x00404cf0 */
static void CollectMissingEntries( fileListMap_t *cache, ValueScope *scope,
                                   std::set< std::string > *missing );  /* 0x00408e00 */
static void CacheFiles( fileListMap_t *cache, std::string name, bool *dirty,
                        std::set< std::string > *files, bool keepEmpty );        /* 0x00405140 */
static bool ReadTargetCacheFile( std::string relTrgFileName, targetCache_t *cache ); /* 0x00405250 */
static void WriteTargetCacheFile( std::string relTrgFileName, targetCache_t *cache ); /* 0x00405670 */
static void DeleteTargetCacheFile( std::string relTrgFileName );                 /* 0x004058d0 */
static void UpdateGdtTargetFile( const std::string &fileName, bool missing );    /* 0x00405c90 */
static void MarkTargetFile( ConvertData *data, const std::string &fileName, bool unchanged ); /* 0x00405cf0 */
static void MarkTargetFiles( ConvertData *data );                                /* 0x00406c20 */
static void MarkConvertedEntries( ConvertInfo *info );                           /* 0x00408160 */
static std::string GetPathFromFileName( const char *fileName );                  /* 0x00403210 */
static bool IsWildcardFile( const char *fileName );                              /* 0x004032c0 */
static void TouchSourceCacheEntry( const std::string &name );                    /* 0x004029f0 */
static void CreateDirs( std::string dir, std::string relPath );  /* 0x00401f70 */
static void AddSingleConvertAssets( std::set< std::string > *singleConvertAssetList );   /* 0x004095a0 */
static bool IsAssetUpToDate( ConvertInfo *info, bool quiet );    /* 0x00406d10 */
static bool ConvertOneAsset( ConvertInfo *info );                /* 0x004083e0 */
static void ReportMissingAsset( const std::string &name );       /* 0x00409860 */
static void SetupTargetDirectories( ConvertInfo *info, ObjectInfo *objectInfo );  /* 0x00404ff0 */
static HMODULE LoadConvertDll( const std::string &name );        /* 0x00409110 */
static int  ReadVersionFile( const std::string &fileName );      /* 0x00405b30 */
static void MarkConvertedFiles( ConvertData *data, const std::string &name );
static void WriteVersionFile( const std::string &fileName, int version );  /* 0x00405bb0 */
static int  GetAssetDirType( const std::string &name );          /* 0x00401470 */
static bool SetAssetDir( int assetDirType );                     /* 0x004014c0 */
static bool IsAssetForThisPlatform( const std::string &name );   /* 0x00409380 */


int          g_platform;                                /* 0x004f515c */

std::string  g_assetChangeListDir;                      /* 0x004b48ec */


std::string  g_cacheSqlServer;                          /* 0x0048617c */
std::string  g_cacheSqlDb;                              /* 0x00486198 */
std::string  g_cacheFilePath;                           /* 0x004861b4 */
std::string  g_cacheAcceptablePrefix;                   /* 0x004861d0 */

std::string  g_platformNames[CONVERTER_PLATFORM_COUNT] = /* 0x00486238 */
{
    "NOT SET",
    "pc",
    "xenon",
    "ps3"
};

std::string  g_srcCacheDir[ASSET_DIR_COUNT];            /* 0x20505e78 */
std::string  g_tgtCacheDir[ASSET_DIR_COUNT];            /* 0x20505f40 */
std::string  g_assetDir[ASSET_DIR_COUNT];               /* 0x20505fd0 */

std::string  g_zoneSoundVars;                           /* 0x004b495c */

std::set< std::string >                             g_skipTypes;            /* 0x20505ee8 */
std::map< std::string, std::set< std::string > * > g_singleConvertAssets;   /* 0x20505fc0 */
static std::set< std::string >                     g_changeListFiles;       /* 0x20505e50 */
static std::set< std::string >                     g_changeListFileNames;   /* 0x20506070 */
static std::set< std::string >                     g_changeListTargets;     /* 0x20506094 */
static bool                                        g_addedAllGdtFiles;      /* 0x204f59f0 */

static const char *g_gdfFilesFileName = "converter_gdf_files_0.txt";    /* 0x004b284c */

bool         g_spam = true;                             /* 0x004b2854 */
int          g_verbose;                                 /* 0x20505de8 */
static int   g_cleanConvert;                            /* 0x20505dec */
static bool  g_aborting;                                /* 0x20505df0 */
static bool  g_answerYes;                               /* 0x20505df5 */
static bool  g_answerNo;                                /* 0x20505df6 */
static bool  g_cacheEnabled;                            /* 0x20505df7 */
static bool  g_cacheUpload;                             /* 0x20505df8 */
bool         g_preprocess;                              /* 0x20505df4 */
bool         g_cacheEntry;                              /* 0x20505e00 */
static int   g_errorCount;                              /* 0x20505e04 */
bool         g_pause;                                   /* 0x204f59f4 */
static bool  g_processChangeList;                       /* 0x204f59ee */

static HANDLE g_consoleHandle;                          /* 0x20505e08 */
static WORD   g_consoleAttributes;                      /* 0x204f59ec */

static char   sys_processSemaphoreFile[MAX_PATH];       /* 0x204f59a0 */

static std::map< std::string, bool > g_createdDirs;     /* 0x20505e5c */

static fileListMap_t g_sourceFilesCache;                /* 0x20506088 */
static fileListMap_t g_targetFilesCache;                /* 0x20505f24 */
static fileListMap_t g_debugFilesCache;                 /* 0x2050607c */

static std::string   g_sourceFilesCacheName;            /* 0x004b4908 */
static std::string   g_targetFilesCacheName;            /* 0x004b4924 */
static std::string   g_debugFilesCacheName;             /* 0x004b4940 */

static bool          g_saveSourceFilesCache;            /* 0x20505df1 */
static bool          g_saveTargetFilesCache;            /* 0x20505df2 */
static bool          g_saveDebugFilesCache;             /* 0x20505df3 */


static std::map< std::string, char > g_convertEntries;         /* 0x20506040 */
static std::map< std::string, int >  g_timeEntries;            /* 0x20505e68 */
static std::map< std::string, std::set< std::string > > g_referencedEntries;  /* 0x20505f30 */

static std::vector< std::string >    g_xenonDeleteDirs;        /* 0x20505fb0 */

static const char *g_xenonDeleteDirsFileName = "converter_xenon_delete_dirs.txt";  /* 0x004b2850 */

static std::set< std::string >       g_newTargetFiles;         /* 0x20505f0c */
static std::set< std::string >       g_newDebugFiles;          /* 0x20505f18 */
static std::set< std::string >       g_changedTargetFiles;     /* 0x2050604c */

static fileList_t                   *g_newSourceFiles;         /* 0x20505dfc */

static int   g_convertReasonCounts[CONVERT_REASON_COUNT];      /* 0x204f59c4 */

static const char *g_convertReasonNames[CONVERT_REASON_COUNT] =    /* 0x004b2828 */
{
    "missing file",
    "missing cache entry",
    "asset out of date",
    "clean convert",
    "new version",
    "entry changed",
    "cache dirty",
    "new source file",
    "changed source file"
};


/* ClearCacheList  0x00402850 */
static void ClearCacheList( fileListMap_t *list )
{
    fileListMap_t::iterator it;

    for ( it = list->begin(); it != list->end(); ++it )
    {
        if ( it->second )
        {
            free( it->second );
        }
    }

    list->clear();
}


/* MarkSourceCacheDirty  0x00402910 */
static void MarkSourceCacheDirty( std::string name )
{
    fileListMap_t::iterator it;

    it = g_sourceFilesCache.find( name );

    if ( it == g_sourceFilesCache.end() )
    {
        return;
    }

    if ( it->second )
    {
        free( it->second );
    }

    g_saveSourceFilesCache = true;

    g_sourceFilesCache.erase( it );
}


/* TouchSourceCacheEntry  0x004029f0 */
static void TouchSourceCacheEntry( const std::string &name )
{
    fileListMap_t::iterator it;
    fileList_t             *entry;

    it = g_sourceFilesCache.find( name );

    if ( it == g_sourceFilesCache.end() )
    {
        return;
    }

    entry = it->second;

    if ( entry && entry->num )
    {
        if ( entry->names[0][0] == '?' )
        {
            return;
        }

        if ( entry->names[0][0] == ':' )
        {
            g_saveSourceFilesCache = true;
            entry->names[0][0] = '?';
            return;
        }
    }

    MarkSourceCacheDirty( name );
}


/* GetPathFromFileName  0x00403210 */
static std::string GetPathFromFileName( const char *fileName )
{
    char buf[BUFFER_LEN];
    int  len;

    len = (int)strlen( fileName ) - 1;

    while ( len >= 0 && fileName[len] != '\\' && fileName[len] != '/' )
    {
        len--;
    }

    len++;

    memcpy( buf, fileName, len );
    buf[len] = 0;

    return std::string( buf );
}


/* IsWildcardFile  0x004032c0 */
static bool IsWildcardFile( const char *fileName )
{
    return fileName[strlen( fileName ) - 1] == '*';
}


/* IsGdtFile  0x004032f0 */
static bool IsGdtFile( const std::string &fileName )
{
    if ( (int)fileName.size() <= 4 )
    {
        return false;
    }

    return !_stricmp( fileName.c_str() + fileName.size() - 4, ".gdt" );
}


/* InitXenonDeleteDirs  0x00403330 */
static void InitXenonDeleteDirs( void )
{
    FILE *f;
    char  line[1024];
    char *name;

    assert( g_platform == CONVERTER_PLATFORM_XENON );

    f = fopen( g_xenonDeleteDirsFileName, "rb" );

    if ( !f )
    {
        return;
    }

    while ( ( name = ReadLine( f, line, sizeof( line ) ) ) != NULL )
    {
        g_xenonDeleteDirs.push_back( std::string( line ) );
    }

    fclose( f );
}


/* IsXenonDeleteDir  0x004034a0 */
static bool IsXenonDeleteDir( const char *relPath )
{
    unsigned int i;
    size_t       len;

    assert( g_platform == CONVERTER_PLATFORM_XENON );

    for ( i = 0; i < g_xenonDeleteDirs.size(); i++ )
    {
        len = g_xenonDeleteDirs[i].size();

        if ( !_strnicmp( relPath, g_xenonDeleteDirs[i].c_str(), len ) )
        {
            if ( relPath[len] == '/' || relPath[len] == '\\' )
            {
                return true;
            }
        }
    }

    return false;
}


/* DeleteAssetFile  0x00403540 */
static void DeleteAssetFile( const std::string &relFileName )
{
    std::string fullName;
    char        buf[BUFFER_LEN];
    char       *out;
    const char *in;

    if ( IsGdtFile( relFileName ) )
    {
        RemoveGdtEntries( relFileName );
    }

    fullName = g_baseDir + relFileName;

    DeleteFileA( fullName.c_str() );

    assert( g_platform != CONVERTER_PLATFORM_NOT_SET );

    if ( g_platform != CONVERTER_PLATFORM_XENON )
    {
        return;
    }

    if ( _strnicmp( relFileName.c_str(), "xenon", 5 ) )
    {
        return;
    }

    if ( relFileName.c_str()[5] != '/' && relFileName.c_str()[5] != '\\' )
    {
        return;
    }

    in  = relFileName.c_str() + 6;
    out = buf;

    while ( *in )
    {
        *out = ( *in == '/' ) ? '\\' : *in;
        in++;
        out++;
    }

    *out = 0;

    if ( IsXenonDeleteDir( buf ) )
    {
        ShellExecuteA( NULL, "open", "xenon_deletefile.bat", buf, NULL, 0 );
    }
}


/* AddToFileList  0x004024d0 */
static void AddToFileList( fileList_t **list, const char *name, bool checkDuplicate )
{
    fileList_t *entry;
    fileList_t *newEntry;
    int         size;
    int         headerSize;
    int         stringSize;
    int         delta;
    char       *strings;
    int         i;

    Prof_Begin( PROF_CACHE_FILE );

    entry = *list;

    if ( !entry )
    {
        size = (int)strlen( name ) + 13;

        entry = (fileList_t *)malloc( size );
        entry->len = size;
        entry->num = 1;

        strings = (char *)&entry->names[1];
        strcpy( strings, name );
        entry->names[0] = strings;

        *list = entry;

        Prof_End();
        return;
    }

    if ( checkDuplicate )
    {
        for ( i = entry->num - 1; i >= 0; i-- )
        {
            if ( !strcmp( entry->names[i], name ) )
            {
                Prof_End();
                return;
            }
        }
    }

    size = entry->len + (int)strlen( name ) + 5;

    newEntry = (fileList_t *)malloc( size );
    newEntry->len = size;
    newEntry->num = entry->num + 1;

    headerSize = entry->num * 4 + 8;
    stringSize = entry->len - headerSize;

    memcpy( &newEntry->names[newEntry->num], (char *)entry + headerSize, stringSize );

    strings = (char *)&newEntry->names[newEntry->num] + stringSize;

    delta = (int)( (char *)newEntry - (char *)entry ) + 4;

    for ( i = entry->num - 1; i >= 0; i-- )
    {
        newEntry->names[i] = entry->names[i] + delta;
    }

    strcpy( strings, name );
    newEntry->names[newEntry->num - 1] = strings;

    free( entry );

    *list = newEntry;

    Prof_End();
}


/* GetFileTime  0x00403fb0 */
static int GetFileTime( const std::string &fileName )
{
    std::map< std::string, int >::iterator it;
    struct _stat                           st;
    int                                    time;

    it = g_timeEntries.find( fileName );

    if ( it != g_timeEntries.end() )
    {
        return it->second;
    }

    Prof_Begin( PROF_FILE_TIME_CHECK );

    time = _stat( fileName.c_str(), &st ) ? 0 : (int)st.st_mtime;

    Prof_End();

    g_timeEntries[fileName] = time;

    return time;
}


/* DeleteAssetFiles  0x00403720 */
static void DeleteAssetFiles( const char *relName )
{
    std::string        fullName;
    std::string        dir;
    WIN32_FIND_DATAA   findData;
    HANDLE             find;
    BOOL               more;

    fullName = g_baseDir + relName;

    if ( relName[strlen( relName ) - 1] == '*' )
    {
        Prof_Begin( PROF_WILDCARD_ASSET );
        find = FindFirstFileA( fullName.c_str(), &findData );
        Prof_End();

        if ( find == INVALID_HANDLE_VALUE )
        {
            return;
        }

        dir = GetPathFromFileName( relName );

        do
        {
            std::string name = dir + findData.cFileName;

            if ( g_referencedEntries.find( name ) == g_referencedEntries.end() )
            {
                Prof_Begin( PROF_FILE_DELETE );
                DeleteAssetFile( name );
                Prof_End();
            }
            else
            {
                assert( g_referencedEntries.find( name )->second.size() );
            }

            Prof_Begin( PROF_WILDCARD_ASSET );
            more = FindNextFileA( find, &findData );
            Prof_End();
        }
        while ( more );

        FindClose( find );
        return;
    }

    assert( g_referencedEntries.find( std::string( relName ) ) == g_referencedEntries.end() );

    Prof_Begin( PROF_FILE_DELETE );
    DeleteAssetFile( std::string( relName ) );
    Prof_End();
}


/* AddFile  0x00403bc0 */
void AddFile( const char *fileName, int type )
{
    std::string relName;
    char        buf[BUFFER_LEN];

    Prof_Begin( PROF_REL_FILENAME );

    relName = MakeRelativePath( g_baseDir, std::string( fileName ) );

    Prof_End();

    if ( relName.empty() )
    {
        Error( va( "file '%s' is not under '%s'\n", fileName, g_baseDir.c_str() ) );
        return;
    }

    if ( type == 0 || type == 1 )
    {
        CreateDirs( g_tgtCacheDir[ASSET_DIR_PLATFORM], relName + "." );
    }

    CreateDirs( g_baseDir, relName + "." );

    ConvertPath( buf, relName.c_str(), sizeof( buf ) );

    if ( type == 0 || type == 1 )
    {
        g_newTargetFiles.insert( std::string( buf ) );
    }

    if ( type == 0 )
    {
        g_changedTargetFiles.insert( std::string( buf ) );
    }
    else if ( type == 2 )
    {
        g_newDebugFiles.insert( std::string( buf ) );
    }
}


/* ConvertData::AddTargetFile  0x00403f50 */
void ConvertData::AddTargetFile( const char *fileName )
{
    AddFile( fileName, 0 );
}


/* ConvertData::AddUnchangedTargetFile  0x00403f60 */
void ConvertData::AddUnchangedTargetFile( const char *fileName )
{
    AddFile( fileName, 1 );
}


/* ConvertData::AddDebugFile  0x00403f70 */
void ConvertData::AddDebugFile( const char *fileName )
{
    AddFile( fileName, 2 );
}


/* ConvertData::AddGeneratedFile  0x00404110 */
void ConvertData::AddGeneratedFile( const char *fileName )
{
    std::string        relName;
    std::string        dir;
    char               buf[BUFFER_LEN];
    WIN32_FIND_DATAA   findData;
    HANDLE             find;
    struct _stat       st;

    Prof_Begin( PROF_REL_FILENAME );

    relName = MakeRelativePath( g_baseDir, std::string( fileName ) );

    Prof_End();

    if ( relName.empty() )
    {
        printf( "\n************ ERROR ************\n" );
        printf( "%s", va( "file '%s' is not under '%s'\n", fileName, g_baseDir.c_str() ) );
        m_info->m_error = true;
        return;
    }

    ConvertPath( buf, relName.c_str(), sizeof( buf ) );

    AddToFileList( &g_newSourceFiles, buf, true );

    if ( relName.c_str()[relName.size() - 1] == '*' )
    {
        Prof_Begin( PROF_WILDCARD_ASSET );
        find = FindFirstFileA( fileName, &findData );
        Prof_End();

        if ( find == INVALID_HANDLE_VALUE )
        {
            printf( "\n************ ERROR ************\n" );
            printf( "%s", va( "Could not find source file '%s'\n", fileName ) );
            m_info->m_error = true;
            return;
        }

        dir = GetPathFromFileName( relName.c_str() );

        for ( ;; )
        {
            ConvertPath( buf, ( dir + findData.cFileName ).c_str(), sizeof( buf ) );

            AddToFileList( &g_newSourceFiles, buf, true );

            Prof_Begin( PROF_WILDCARD_ASSET );

            if ( !FindNextFileA( find, &findData ) )
            {
                FindClose( find );
                Prof_End();
                return;
            }

            Prof_End();
        }
    }

    if ( _stat( fileName, &st ) )
    {
        printf( "\n************ ERROR ************\n" );
        printf( "%s", va( "Could not find source file '%s'\n", fileName ) );
        m_info->m_error = true;
    }
}


/* ConvertData::AddWildcardFiles  0x004045b0 */
void ConvertData::AddWildcardFiles( const char *dir, const char *name, const char *extension )
{
    std::string paths[ASSET_DIR_COUNT];
    bool        exists[ASSET_DIR_COUNT];

    assert( dir );
    assert( name );
    assert( extension );

    paths[0] = g_baseDir + g_assetDir[ASSET_DIR_SHARED] + dir + name + extension;
    paths[1] = g_baseDir + g_assetDir[ASSET_DIR_DEFAULT] + dir + name + extension;

    exists[0] = !_access( paths[0].c_str(), 0 );
    exists[1] = !_access( paths[1].c_str(), 0 );

    if ( exists[0] || !exists[1] )
    {
        AddGeneratedFile( paths[0].c_str() );
    }

    if ( exists[1] || !exists[0] )
    {
        AddGeneratedFile( paths[1].c_str() );
    }
}


/* LoadCacheFiles  0x00402ab0 */
static void LoadCacheFiles( fileListMap_t *list, std::string fileName )
{
    FILE       *f;
    int         len;
    char       *buf;
    char       *text;
    char       *token;
    fileList_t *entry;
    char       *names;
    int         num;
    int         size;
    int         i;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "rb" );
    Prof_End();

    if ( !f )
    {
        return;
    }

    Prof_Begin( PROF_FILE_SEEK );
    fseek( f, 0, SEEK_END );
    len = ftell( f );

    if ( len <= 0 )
    {
        Prof_End();
        Prof_Begin( PROF_FILE_OPEN );
        fclose( f );
        Prof_End();
        return;
    }

    fseek( f, 0, SEEK_SET );
    Prof_End();

    buf = (char *)malloc( len + 1 );

    Prof_Begin( PROF_FILE_READ );

    if ( (int)fread( buf, 1, len, f ) != len )
    {
        Prof_End();
        free( buf );
        Error( va( "Short read in LoadCacheFiles\n" ) );

        Prof_Begin( PROF_FILE_OPEN );
        fclose( f );
        Prof_End();
        return;
    }

    Prof_End();

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();

    buf[len] = 0;

    text  = buf;
    token = COM_ParseExt( &text, qtrue );

    while ( text )
    {
        std::string name( token );

        num = Legacy_atol( COM_ParseExt( &text, qtrue ) );

        if ( !num )
        {
            ( *list )[name] = NULL;
        }
        else
        {
            size = Legacy_atol( COM_ParseExt( &text, qtrue ) );

            entry = (fileList_t *)malloc( size );
            entry->num = num;

            names = (char *)&entry->names[num];

            for ( i = 0; i < num; i++ )
            {
                strcpy( names, COM_ParseExt( &text, qtrue ) );
                entry->names[i] = names;
                names += strlen( names ) + 1;
            }

            entry->len = (int)( names - (char *)entry );

            assertx( entry->len <= size, "Bad cache file %s", fileName.c_str() );

            ( *list )[name] = entry;
        }

        token = COM_ParseExt( &text, qtrue );
    }

    free( buf );
}


/* SaveCacheFiles  0x00402e30 */
static void SaveCacheFiles( fileListMap_t *list, std::string fileName )
{
    FILE                    *f;
    fileListMap_t::iterator  it;
    fileList_t              *entry;
    int                      i;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "wb" );
    Prof_End();

    if ( !f )
    {
        return;
    }

    for ( it = list->begin(); it != list->end(); ++it )
    {
        entry = it->second;

        if ( !entry )
        {
            Prof_Begin( PROF_FILE_WRITE );
            fprintf( f, "\"%s\" 0\n", it->first.c_str() );
            Prof_End();
            continue;
        }

        assert( entry->num );

        std::string name = EscapeString( it->first );

        Prof_Begin( PROF_FILE_WRITE );
        fprintf( f, "\"%s\" %d %d", name.c_str(), entry->num, entry->len );
        Prof_End();

        for ( i = 0; i < entry->num; i++ )
        {
            std::string fileName2 = EscapeString( std::string( entry->names[i] ) );

            Prof_Begin( PROF_FILE_WRITE );
            fprintf( f, " \"%s\"", fileName2.c_str() );
            Prof_End();
        }

        Prof_Begin( PROF_FILE_WRITE );
        fprintf( f, "\n" );
        Prof_End();
    }

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();
}


/* CreateDirs  0x00401f70 */
static void CreateDirs( std::string dir, std::string relPath )
{
    unsigned int i;
    char         c;

    Prof_Begin( PROF_MKDIR );

    if ( g_createdDirs.find( dir ) == g_createdDirs.end() )
    {
        g_createdDirs[dir] = true;
        _mkdir( dir.c_str() );
    }

    for ( i = 0; i < relPath.size(); i++ )
    {
        c = relPath.c_str()[i];

        if ( c == '\\' || c == '/' )
        {
            if ( g_createdDirs.find( dir ) == g_createdDirs.end() )
            {
                g_createdDirs[dir] = true;
                _mkdir( dir.c_str() );
            }

            if ( i == relPath.size() - 1 )
            {
                Prof_End();
                return;
            }
        }
        else if ( c == '.' )
        {
            Prof_End();
            return;
        }

        dir += c;
    }

    if ( g_createdDirs.find( dir ) == g_createdDirs.end() )
    {
        g_createdDirs[dir] = true;
        _mkdir( dir.c_str() );
    }

    Prof_End();
}


/* IsProcessRunning  0x0040e930 */
static bool IsProcessRunning( DWORD processId )
{
    HANDLE        process;
    HANDLE        snapshot;
    MODULEENTRY32 moduleEntry;
    char          moduleFileName[MAX_PATH];
    char         *name;
    char         *p;
    bool          running;

    process = OpenProcess( PROCESS_ALL_ACCESS, FALSE, processId );

    if ( !process )
    {
        return false;
    }

    CloseHandle( process );

    snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, processId );

    if ( snapshot == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    running = false;

    moduleEntry.dwSize = sizeof( moduleEntry );

    if ( Module32First( snapshot, &moduleEntry ) )
    {
        GetModuleFileNameA( NULL, moduleFileName, sizeof( moduleFileName ) );

        moduleFileName[sizeof( moduleFileName ) - 1] = 0;

        name = moduleFileName;

        for ( p = moduleFileName; *p; p++ )
        {
            if ( *p == '\\' || *p == ':' )
            {
                name = p + 1;
            }
        }

        do
        {
            if ( !_stricmp( moduleEntry.szModule, name ) )
            {
                running = true;
                break;
            }
        }
        while ( Module32Next( snapshot, &moduleEntry ) );
    }

    CloseHandle( snapshot );

    return running;
}


/* ConvertPath  0x00402250 */
void ConvertPath( char *dest, const char *src, int count )
{
    int c;

    assert( count );

    for ( ;; )
    {
        c = *src++;

        while ( c == '\\' || c == '/' )
        {
            c = *src++;
        }

        if ( (unsigned char)c < ' ' )
        {
            if ( !c )
            {
                break;
            }
            continue;
        }

        for ( ;; )
        {
            *dest++ = (char)tolower( c );
            count--;

            assert( count );

            if ( c == '/' )
            {
                break;
            }

            c = *src++;

            if ( c == '\\' )
            {
                c = '/';
                continue;
            }

            if ( (unsigned char)c < ' ' )
            {
                break;
            }
        }

        if ( !c )
        {
            break;
        }
    }

    *dest = 0;
}


/* PrintDone  0x0040d360 */
static void PrintDone( void )
{
    if ( !g_pause )
    {
        fflush( stdout );
        return;
    }

    if ( g_errorCount )
    {
        printf( "\nConversion failed with %d errors.", g_errorCount );
    }
    else
    {
        printf( "\nConversion done." );
    }

    printf( " Hit key to continue.\n" );
    fflush( stdout );

    Prof_Pause();
    _getch();
    Prof_Resume();

    g_pause = false;
}


/* ErrorPrompt  0x00402310 */
void ErrorPrompt( void )
{
    int c;

    g_errorCount++;

    if ( g_preprocess )
    {
        PrintDone();
        exit( 0 );
    }

    if ( g_answerYes || g_aborting )
    {
        return;
    }

    if ( g_answerNo )
    {
        printf( "aborting....\n" );
        fflush( stdout );
        g_aborting = true;
        return;
    }

    printf( "\nContinue? ( (y)es / (n)o / yes (a)ll )\n" );
    fflush( stdout );

    for ( ;; )
    {
        Prof_Pause();
        c = _getch();
        Prof_Resume();

        switch ( c )
        {
        case 'y':
        case 'Y':
            printf( "continuing...\n" );
            fflush( stdout );
            return;

        case 'n':
        case 'N':
            printf( "aborting...\n" );
            fflush( stdout );
            g_aborting = true;
            return;

        case 'a':
        case 'A':
            printf( "continuing all...\n" );
            fflush( stdout );
            g_answerYes = true;
            return;
        }
    }
}


/* Error  0x00402460 */
void Error( const char *message )
{
    if ( g_cacheEntry )
    {
        return;
    }

    printf( "\n************ ERROR ************\n" );
    printf( "%s", message );
    fflush( stdout );

    ErrorPrompt();
}


/* Usage  0x0040d3f0 */
static void Usage( void )
{
    fprintf( stderr,
             "\n"
             "\n"
             "Valid parameters are:\n"
             "  build all\n"
             "  clean all\n"
             "  -nospam\n"
             "  -gamedir <game dir path>\n"
             "  -nocachedownload\n"
             "  -cacheupload <password>\n"
             "  -cachesqlserver <server name>\n"
             "  -cachesqldb <db name>\n"
             "  -cachefilepath <filepath>\n"
             "  -cacheprefix <depot name prefix>\n"
             "  -skiptype <asset type>\n"
             "  -single <asset type> <asset name>\n"
             "  -verbose\n"
             "  -changelist\n"
             "  -zonesoundvars <loadspec,loadspec,...>\n"
             "  -y\n"
             "  -n\n" );
    fflush( stderr );

    printf( "Conversion failed due to invalid or missing parameter(s).\n Hit key to continue.\n" );
    fflush( stdout );

    if ( !g_answerYes && !g_answerNo )
    {
        Prof_Pause();
        _getch();
        Prof_Resume();
    }
}


/* InitGlobals  0x0040d450 */
static void InitGlobals( void )
{
    assert( !g_cacheSqlServer.length() );
    assert( !g_cacheSqlDb.length() );
    assert( !g_cacheFilePath.length() );
    assert( !g_cacheAcceptablePrefix.length() );

    g_assetDir[ASSET_DIR_DEFAULT].assign( "raw/", 4 );

    g_pause        = true;
    g_cacheEnabled = true;
    g_cacheUpload  = false;
}


/* ValidateCacheOptions  0x0040d570 */
static bool ValidateCacheOptions( void )
{
    bool ok;

    ok = true;

    if ( g_cacheEnabled )
    {
        if ( !g_cacheSqlServer.length() )
        {
            ok = false;
            fprintf( stderr, "ERROR! cache enabled, yet -cachesqlserver not specified!\n" );
        }

        if ( !g_cacheSqlDb.length() )
        {
            ok = false;
            fprintf( stderr, "ERROR! cache enabled, yet -cachesqldb not specified!\n" );
        }

        if ( !g_cacheFilePath.length() )
        {
            ok = false;
            fprintf( stderr, "ERROR! cache enabled, yet -cachefilepath not specified!\n" );
        }

        if ( !g_cacheAcceptablePrefix.length() )
        {
            ok = false;
            fprintf( stderr, "ERROR! cache enabled, yet -cacheprefix not specified!\n" );
        }
    }

    return ok;
}


/* FixupGameDir  0x0040d250 */
static std::string FixupGameDir( const char *dir )
{
    char  buf[BUFFER_LEN];
    char *end;

    ConvertPath( buf, dir, sizeof( buf ) );

    end = buf + strlen( buf ) - 1;

    while ( end >= buf && *end == '/' )
    {
        *end = 0;
        end--;
    }

    return std::string( buf ) + "/";
}


/* SetPlatform  0x0040d3d0 */
static void SetPlatform( void )
{
    g_assetDir[ASSET_DIR_SHARED] = g_assetDir[ASSET_DIR_DEFAULT];

    g_platform = CONVERTER_PLATFORM_PC;
}


/* InitCacheDirs  0x0040e730 */
static void InitCacheDirs( int assetDirType )
{
    g_srcCacheDir[assetDirType] = g_srcCacheBaseDir + g_assetDir[assetDirType];
    g_tgtCacheDir[assetDirType] = g_tgtCacheBaseDir + g_assetDir[assetDirType];

    CreateDirs( g_srcCacheBaseDir, g_assetDir[assetDirType] );
    CreateDirs( g_tgtCacheBaseDir, g_assetDir[assetDirType] );
}


/* InitConsole  0x0040b3f0 */
static void InitConsole( void )
{
    CONSOLE_SCREEN_BUFFER_INFO info;

    g_consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

    GetConsoleScreenBufferInfo( g_consoleHandle, &info );

    g_consoleAttributes = info.wAttributes;
}


/* BuildProcessSemaphoreFileName  0x0040e8b0 */
static void BuildProcessSemaphoreFileName( void )
{
    char  moduleFileName[MAX_PATH];
    char *name;
    char *p;

    GetModuleFileNameA( NULL, moduleFileName, sizeof( moduleFileName ) );

    moduleFileName[sizeof( moduleFileName ) - 1] = 0;

    name = moduleFileName;

    for ( p = moduleFileName; *p; p++ )
    {
        if ( *p == '\\' || *p == ':' )
        {
            name = p + 1;
        }
        else if ( *p == '.' )
        {
            *p = 0;
        }
    }

    sprintf( sys_processSemaphoreFile, "__%s", name );
}


/* CheckProcessSemaphore  0x0040ea50 */
static bool CheckProcessSemaphore( void )
{
    HANDLE file;
    DWORD  processId;
    DWORD  otherProcessId;
    DWORD  bytes;

    assert( sys_processSemaphoreFile[0] );

    processId = GetCurrentProcessId();

    file = CreateFileA( sys_processSemaphoreFile, GENERIC_READ, 0, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL );

    if ( file != INVALID_HANDLE_VALUE )
    {
        if ( ReadFile( file, &otherProcessId, sizeof( otherProcessId ), &bytes, NULL )
             && bytes == sizeof( otherProcessId ) )
        {
            CloseHandle( file );

            if ( otherProcessId != processId && IsProcessRunning( otherProcessId ) )
            {
                return true;
            }
        }
        else
        {
            CloseHandle( file );
        }
    }

    file = CreateFileA( sys_processSemaphoreFile, GENERIC_WRITE, 0, NULL,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL );

    if ( file == INVALID_HANDLE_VALUE )
    {
        printf( "File '%s' failed to open", sys_processSemaphoreFile );
        exit( 1 );
    }

    if ( WriteFile( file, &processId, sizeof( processId ), &bytes, NULL )
         && bytes == sizeof( processId ) )
    {
        CloseHandle( file );
        return false;
    }

    CloseHandle( file );

    printf( "File '%s' failed to write", sys_processSemaphoreFile );
    exit( 1 );

    return false;
}


/* ParseCommandLine  0x0040da30 */
static bool ParseCommandLine( int argc, char **argv )
{
    int         i;
    std::string gdfName;

    for ( i = 1; i < argc; i++ )
    {
        if ( !_stricmp( argv[i], "-gamedir" ) )
        {
            i++;
            if ( i >= argc )
            {
                return false;
            }
            g_assetDir[ASSET_DIR_DEFAULT] = FixupGameDir( argv[i] );
        }
        else if ( !_stricmp( argv[i], "-nospam" ) )
        {
            g_spam = false;
        }
        else if ( !_stricmp( argv[i], "-verbose" ) )
        {
            g_verbose = 1;
        }
        else if ( !_stricmp( argv[i], "build" ) )
        {
            i++;
            if ( i >= argc )
            {
                return false;
            }
            g_cleanConvert = 0;
            if ( _stricmp( argv[i], "all" ) )
            {
                printf( "WARNING: building a specific gdf file is deprecated\n" );
            }
        }
        else if ( !_stricmp( argv[i], "clean" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }
            g_cleanConvert = 1;
            if ( _stricmp( argv[i], "all" ) )
            {
                printf( "WARNING: cleaning a specific gdf file is deprecated\n" );
            }
        }
        else if ( !_stricmp( argv[i], "-cachefilepath" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }
            g_cacheFilePath = argv[i];
        }
        else if ( !_stricmp( argv[i], "-cachesqlserver" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }
            g_cacheSqlServer = argv[i];
        }
        else if ( !_stricmp( argv[i], "-cachesqldb" ) )
        {
            i++;
            if ( i >= argc )
            {
                fprintf( stderr, "No string argument after -cachesqldb!\n\n" );
                goto usage;
            }
            g_cacheSqlDb = argv[i];
        }
        else if ( !_stricmp( argv[i], "-nocachedownload" ) )
        {
            g_cacheEnabled = false;
        }
        else if ( !_stricmp( argv[i], "-cachedownload" ) )
        {
            fprintf( stderr,
                     "\n*** WARNING! -cachedownload parameter is deprecated\n"
                     "*** (cache is on by default.)\n"
                     "*** Please fix your shortcut/batchfile/cmd/whatever.\n\n" );
        }
        else if ( !_stricmp( argv[i], "-cacheupload" ) )
        {
            i++;
            if ( i >= argc || strcmp( argv[i], "dontdothis" ) )
            {
                fprintf( stderr, "Bad or no -cacheupload password!\n\n" );
                goto usage;
            }
            g_cacheEnabled = true;
            g_cacheUpload  = true;
        }
        else if ( !_stricmp( argv[i], "-cacheprefix" ) )
        {
            i++;
            if ( i >= argc )
            {
                fprintf( stderr, "No string argument after -cacheprefix!\n\n" );
                goto usage;
            }
            if ( argv[i][0] != '/' || argv[i][1] != '/' )
            {
                fprintf( stderr, "Unacceptable -cacheprefix (must begin with \"//\"!)\n\n" );
                goto usage;
            }
            g_cacheAcceptablePrefix = argv[i];
        }
        else if ( !_stricmp( argv[i], "-nopause" ) )
        {
            g_pause = false;
        }
        else if ( !_stricmp( argv[i], "-y" ) )
        {
            g_answerYes = true;
        }
        else if ( !_stricmp( argv[i], "-n" ) )
        {
            g_answerNo = true;
        }
        else if ( !_stricmp( argv[i], "-skiptype" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }
            g_skipTypes.insert( std::string( argv[i] ) + ".gdf" );
        }
        else if ( !_stricmp( argv[i], "-single" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }

            gdfName = std::string( argv[i] ) + ".gdf";

            i++;
            if ( i >= argc )
            {
                goto usage;
            }

            if ( g_singleConvertAssets.find( gdfName ) == g_singleConvertAssets.end() )
            {
                g_singleConvertAssets[gdfName] = new std::set< std::string >;
            }

            g_singleConvertAssets[gdfName]->insert( std::string( argv[i] ) );
        }
        else if ( !_stricmp( argv[i], "-changelist" ) )
        {
            ProcessChangeList();
        }
        else if ( !_stricmp( argv[i], "-zonesoundvars" ) )
        {
            i++;
            if ( i >= argc )
            {
                goto usage;
            }

            if ( argv[i][0] != ',' )
            {
                g_zoneSoundVars += ",";
            }

            g_zoneSoundVars += argv[i];

            i++;
        }
        else
        {
            fprintf( stderr, "\n\nERROR! Unknown switch '%s'\n", argv[i] );
            goto usage;
        }
    }

    return true;

usage:
    return false;
}


/* ReadConverterCfg  0x0040e0f0 */
static bool ReadConverterCfg( void )
{
    char  line[1024];
    char *argv[MAX_CONVERT_ARGS];
    int   argc;
    int   i;
    FILE *f;
    char *text;
    char *token;
    bool  ok;

    f = fopen( "converter.cfg", "r" );

    if ( !f )
    {
        fprintf( stderr,
                 "\n\nERROR! No \"converter.cfg\" file found!\n"
                 "Make sure you are running converter from the appropriate directory.\n\n" );
        return false;
    }

    argc    = 1;
    argv[0] = "<ignored>";

    while ( fgets( line, sizeof( line ), f ) )
    {
        text = line;

        for ( ;; )
        {
            token = COM_ParseExt( &text, qtrue );

            assert( token );

            if ( !*token )
            {
                break;
            }

            argv[argc] = _strdup( token );
            argc++;

            if ( argc == MAX_CONVERT_ARGS )
            {
                fprintf( stderr, "\n\nERROR! Too many arguments in \"converter.cfg\"!\n\n" );
                fclose( f );
                return false;
            }
        }
    }

    fclose( f );

    argv[argc] = NULL;

    ok = ParseCommandLine( argc, argv );

    for ( i = argc - 1; i > 0; i-- )
    {
        free( argv[i] );
    }

    if ( !ok )
    {
        fprintf( stderr, "\n\nERROR in \"converter.cfg\"\n\n" );
        return false;
    }

    return true;
}


/* ProcessChangeList  0x0040d600 */
static void ProcessChangeList( void )
{
    FILE        *f;
    std::string  fileName;
    char         line[1024];
    char        *name;

    g_processChangeList = true;

    fileName = g_assetChangeListDir + "changelist.txt";

    f = fopen( fileName.c_str(), "r" );

    if ( !f )
    {
        printf( "'%s' not found\n", fileName.c_str() );
        return;
    }

    while ( ( name = ReadLine( f, line, sizeof( line ) ) ) != NULL )
    {
        g_changeListFileNames.insert( std::string( name ) );
    }

    fclose( f );
}


/* SaveCacheFilesAtExit  0x00408d30 */
static void SaveCacheFilesAtExit( void )
{
    if ( g_saveSourceFilesCache )
    {
        SaveCacheFiles( &g_sourceFilesCache, g_sourceFilesCacheName );
        g_saveSourceFilesCache = false;
    }

    if ( g_saveTargetFilesCache )
    {
        SaveCacheFiles( &g_targetFilesCache, g_targetFilesCacheName );
        g_saveTargetFilesCache = false;
    }

    if ( g_saveDebugFilesCache )
    {
        SaveCacheFiles( &g_debugFilesCache, g_debugFilesCacheName );
        g_saveDebugFilesCache = false;
    }
}


/* RemoveEntriesFromFile  0x0040bf00 */
static void RemoveEntriesFromFile( const std::string &fileName, gdtEntryMap_t *entries )
{
    gdtEntryMap_t::iterator           it;
    std::list< Variable * >::iterator var;
    std::list< Variable * >::iterator next;

    for ( it = entries->begin(); it != entries->end(); ++it )
    {
        for ( var = it->second->m_variables.begin();
              var != it->second->m_variables.end();
              var = next )
        {
            next = var;
            ++next;

            if ( ( (ValueData *)( *var )->m_value )->m_sourceFileName == fileName )
            {
                delete *var;
                it->second->m_variables.erase( var );
                it->second->ResetIndex();
            }
        }
    }
}


/* RemoveGdtEntries  0x0040c050 */
static void RemoveGdtEntries( const std::string &relFileName )
{
    gdtFileMap_t::iterator it;
    char                   buf[BUFFER_LEN];

    ConvertPath( buf, relFileName.c_str(), sizeof( buf ) );

    it = g_convertFileNames.find( std::string( buf ) );

    if ( it == g_convertFileNames.end() )
    {
        return;
    }

    g_convertFileNames.erase( it );

    RemoveEntriesFromFile( std::string( buf ), &g_gdtEntries );
    RemoveEntriesFromFile( std::string( buf ), &g_cachedEntries[ASSET_DIR_DEFAULT] );
    RemoveEntriesFromFile( std::string( buf ), &g_cachedEntries[ASSET_DIR_SHARED] );
}


/* LoadChangeListFiles  0x0040d7d0 */
static void LoadChangeListFiles( void )
{
    std::set< std::string >::iterator it;
    std::string                       relName;

    for ( it = g_changeListFileNames.begin(); it != g_changeListFileNames.end(); ++it )
    {
        relName = MakeRelativePath( g_baseDir, *it );

        if ( relName.empty() )
        {
            printf( "file '%s' is not under '%s'\n", it->c_str(), g_baseDir.c_str() );
            continue;
        }

        if ( strstr( relName.c_str(), ".gdt" ) )
        {
            AddGdtFile( relName, true );
            continue;
        }

        g_changeListFiles.insert( relName );
        g_changeListTargets.insert( relName );
    }
}


/* ProcessChangeListGdf  0x0040b5b0 */
static void ProcessChangeListGdf( std::string gdfName )
{
    std::set< std::string >          *singleConvertAssetList;
    gdtEntryMap_t::iterator           scopeIt;
    gdtEntryMap_t::iterator           changeIt;
    ValueScope                       *scope;
    ValueScope                       *changeScope;
    std::list< Variable * >::iterator var;
    Variable                         *other;
    buf_t                             buf;
    buf_t                             otherBuf;
    char                              text[COMPARE_BUFFER_LEN];
    char                              otherText[COMPARE_BUFFER_LEN];

    assert( g_processChangeList );

    if ( g_singleConvertAssets.find( gdfName ) == g_singleConvertAssets.end() )
    {
        g_singleConvertAssets[gdfName] = new std::set< std::string >;
    }

    scopeIt = g_gdtEntries.find( gdfName );

    if ( scopeIt == g_gdtEntries.end() || !scopeIt->second )
    {
        return;
    }

    scope = scopeIt->second;

    singleConvertAssetList = g_singleConvertAssets[gdfName];

    assert( singleConvertAssetList );

    changeIt    = g_changeListEntries.find( gdfName );
    changeScope = changeIt != g_changeListEntries.end() ? changeIt->second : 0;

    for ( var = scope->m_variables.begin(); var != scope->m_variables.end(); ++var )
    {
        other = changeScope ? changeScope->Find( ( *var )->m_name ) : 0;

        if ( other )
        {
            Prof_Begin( PROF_COMPARE_GDT_ENTRY );

            InitBuf( &buf, text, sizeof( text ) );
            InitBuf( &otherBuf, otherText, sizeof( otherText ) );

            ( *var )->ToScript( 0, &buf );
            other->ToScript( 0, &otherBuf );

            Prof_End();

            if ( !strcmp( text, otherText ) )
            {
                continue;
            }
        }

        singleConvertAssetList->insert( ( *var )->m_name );
    }
}


/* ProcessChangeListEntries  0x0040b900 */
static void ProcessChangeListEntries( void )
{
    FILE *f;
    char  line[1024];
    char *name;

    f = fopen( g_gdfFilesFileName, "r" );

    if ( !f )
    {
        printf( "'%s' not found\n", g_gdfFilesFileName );
        return;
    }

    while ( ( name = ReadLine( f, line, sizeof( line ) ) ) != NULL )
    {
        ProcessChangeListGdf( std::string( name ) );
    }

    fclose( f );
}


/* ConvertGdfPass  0x00409aa0 */
static void ConvertGdfPass( const std::string &gdfName, std::set< std::string > *assets,
                            ObjectInfo *info, unsigned int pass )
{
    ConvertInfo                          convertInfo;
    std::string                          name;
    std::string                          otherSourceFile;
    std::string                          otherTargetFile;
    std::string                          versionFileName;
    int                                  assetDirType;
    int                                  otherDirType;
    int                                  version;
    bool                                 versionChanged;
    int                                  numToConvert;
    unsigned int                         i;
    HMODULE                              dll;
    char                                 suffix[32];
    std::map< std::string, char >::iterator entry;
    gdtEntryMap_t::iterator                 scopeIt;
    gdtEntryMap_t::iterator                 cachedIt;
    ValueScope                             *scope;
    std::list< Variable * >::iterator       var;

    name = gdfName;

    if ( pass )
    {
        sprintf( suffix, ".%i", pass );
        name.append( suffix, strlen( suffix ) );
    }

    assetDirType = info->m_DllIsAll[pass] ? ASSET_DIR_SHARED : ASSET_DIR_DEFAULT;
    otherDirType = info->m_DllIsAll[pass] ? ASSET_DIR_DEFAULT : ASSET_DIR_SHARED;

    g_assetDir[ASSET_DIR_PLATFORM]    = g_assetDir[assetDirType];
    g_srcCacheDir[ASSET_DIR_PLATFORM] = g_srcCacheDir[assetDirType];
    g_tgtCacheDir[ASSET_DIR_PLATFORM] = g_tgtCacheDir[assetDirType];

    g_sourceFilesCacheName = g_srcCacheDir[ASSET_DIR_PLATFORM] + name + ".src";
    g_targetFilesCacheName = g_tgtCacheDir[ASSET_DIR_PLATFORM] + name + ".trg";
    g_debugFilesCacheName  = g_tgtCacheDir[ASSET_DIR_PLATFORM] + name + ".dbg";

    otherSourceFile = g_srcCacheDir[otherDirType] + name + ".src";
    otherTargetFile = g_srcCacheDir[otherDirType] + name + ".src";

    if ( !assets && !_access( otherSourceFile.c_str(), 0 ) )
    {
        if ( g_spam )
        {
            printf( "\n'%s' target directory moved ('%s' -> '%s')\n",
                    gdfName.c_str(),
                    g_assetDir[otherDirType].c_str(),
                    g_assetDir[assetDirType].c_str() );
        }

        Prof_Begin( PROF_FILE_DELETE );
        DeleteFileA( otherSourceFile.c_str() );
        DeleteFileA( otherTargetFile.c_str() );
        Prof_End();
    }

    if ( g_cleanConvert )
    {
        Prof_Begin( PROF_FILE_DELETE );
        DeleteFileA( g_sourceFilesCacheName.c_str() );
        Prof_End();
    }

    assert( g_convertEntries.empty() );
    assert( g_timeEntries.empty() );
    assert( g_referencedEntries.empty() );
    assert( g_sourceFilesCache.empty() );
    assert( g_targetFilesCache.empty() );
    assert( g_debugFilesCache.empty() );
    assert( !g_saveSourceFilesCache );

    LoadCacheFiles( &g_sourceFilesCache, g_sourceFilesCacheName );

    if ( assets )
    {
        std::set< std::string >::iterator asset;

        AddSingleConvertAssets( assets );

        for ( asset = assets->begin(); asset != assets->end(); ++asset )
        {
            fileListMap_t::iterator cached;
            bool                    found;

            if ( !IsAssetForThisPlatform( *asset ) )
            {
                continue;
            }

            found  = false;
            cached = g_sourceFilesCache.find( *asset );

            if ( cached != g_sourceFilesCache.end() && cached->second
                 && cached->second->num
                 && ( cached->second->names[0][0] == ':'
                      || cached->second->names[0][0] == '?' ) )
            {
                gdtEntryMap_t::iterator entries;

                AddGdtFileCallback( std::string( cached->second->names[0] + 1 ) );

                entries = g_gdtEntries.find( gdfName );

                if ( entries != g_gdtEntries.end() && entries->second
                     && entries->second->Find( *asset ) )
                {
                    found = true;
                }
            }

            if ( !found )
            {
                if ( !g_addedAllGdtFiles )
                {
                    g_addedAllGdtFiles = true;
                    ForEachGdtDir( AddGdtFileCallback );
                }

                TouchSourceCacheEntry( *asset );
            }
        }
    }
    else if ( !g_addedAllGdtFiles )
    {
        g_addedAllGdtFiles = true;
        ForEachGdtDir( AddGdtFileCallback );
    }

    scopeIt = g_gdtEntries.find( gdfName );

    if ( scopeIt == g_gdtEntries.end() )
    {
        if ( !assets )
        {
            Prof_Begin( PROF_FILE_DELETE );
            DeleteFileA( g_sourceFilesCacheName.c_str() );
            Prof_End();
        }

        assert( !g_saveSourceFilesCache );

        ClearCacheList( &g_sourceFilesCache );
        g_timeEntries.clear();
        g_convertEntries.clear();
        g_referencedEntries.clear();
        return;
    }

    scope = scopeIt->second;

    convertInfo.m_cachedScope[0] = 0;
    convertInfo.m_cachedScope[1] = 0;

    cachedIt = g_cachedEntries[ASSET_DIR_DEFAULT].find( gdfName );

    if ( cachedIt != g_cachedEntries[ASSET_DIR_DEFAULT].end() )
    {
        convertInfo.m_cachedScope[0] = cachedIt->second;
    }

    if ( info->m_DllIsAll[pass] )
    {
        cachedIt = g_cachedEntries[ASSET_DIR_SHARED].find( gdfName );

        convertInfo.m_cachedScope[1] = cachedIt != g_cachedEntries[ASSET_DIR_SHARED].end()
                                       ? cachedIt->second : 0;
    }
    else
    {
        convertInfo.m_cachedScope[1] = convertInfo.m_cachedScope[0];
    }

    DeleteFileA( ( g_srcCacheDir[otherDirType] + name ).c_str() );

    versionFileName = g_srcCacheDir[ASSET_DIR_PLATFORM] + name;

    version = ReadVersionFile( versionFileName );

    if ( version != info->m_Version )
    {
        if ( g_spam )
        {
            printf( "\n'%s' version change (%d -> %d)\n",
                    gdfName.c_str(), version, info->m_Version );
        }
    }

    convertInfo.m_gdfName        = gdfName;
    convertInfo.m_version        = info->m_Version;
    convertInfo.m_versionChanged = version != info->m_Version;
    convertInfo.m_marked         = false;
    convertInfo.m_error          = false;

    convertInfo.m_typeName = info->m_Name;

    versionChanged = version != info->m_Version;

    assert( !g_saveTargetFilesCache );
    assert( !g_saveDebugFilesCache );

    LoadCacheFiles( &g_targetFilesCache, g_targetFilesCacheName );
    LoadCacheFiles( &g_debugFilesCache, g_debugFilesCacheName );

    if ( !assets )
    {
        std::set< std::string >           missing;
        std::set< std::string >::iterator it;

        CollectMissingEntries( &g_targetFilesCache, scope, &missing );

        for ( it = missing.begin(); it != missing.end(); ++it )
        {
            RemoveCachedFiles( &g_targetFilesCache, &convertInfo, *it,
                               &g_saveTargetFilesCache );
        }

        missing.clear();

        CollectMissingEntries( &g_debugFilesCache, scope, &missing );

        for ( it = missing.begin(); it != missing.end(); ++it )
        {
            RemoveCachedFiles( &g_debugFilesCache, &convertInfo, *it,
                               &g_saveDebugFilesCache );
        }

        missing.clear();

        CollectMissingEntries( &g_sourceFilesCache, scope, &missing );

        for ( it = missing.begin(); it != missing.end(); ++it )
        {
            MarkSourceCacheDirty( *it );
        }
    }

    numToConvert = 0;

    if ( !g_spam )
    {
        for ( i = 0; i < CONVERT_REASON_COUNT; i++ )
        {
            g_convertReasonCounts[i] = 0;
        }
    }

    for ( var = scope->m_variables.begin(); var != scope->m_variables.end(); ++var )
    {
        bool quiet;

        convertInfo.m_entry = *var;

        if ( !IsAssetForThisPlatform( ( *var )->m_name ) )
        {
            continue;
        }

        quiet = assets && assets->find( ( *var )->m_name ) == assets->end();

        if ( !IsAssetUpToDate( &convertInfo, quiet ) )
        {
            TouchSourceCacheEntry( ( *var )->m_name );

            if ( !quiet )
            {
                g_convertEntries[( *var )->m_name] = CONVERT_STATE_PENDING;
                numToConvert++;
                continue;
            }
        }

        g_convertEntries[( *var )->m_name] = CONVERT_STATE_DONE;
    }

    if ( !g_spam )
    {
        for ( i = 0; i < CONVERT_REASON_COUNT; i++ )
        {
            if ( g_convertReasonCounts[i] > 0 )
            {
                printf( "Converting %d assets, reason: '%s'\n",
                        g_convertReasonCounts[i], g_convertReasonNames[i] );
            }
        }
    }

    if ( !assets && versionChanged )
    {
        SaveCacheFilesAtExit();
        WriteVersionFile( versionFileName, info->m_Version );
    }

    if ( numToConvert )
    {
        Prof_Begin( PROF_DLL_LOAD );
        dll = LoadConvertDll( info->m_Dll[pass] );
        Prof_End();

        if ( !dll )
        {
            printf( "Convert DLL \"%s\" could not be loaded.\n", info->m_Dll[pass].c_str() );
        }
        else
        {
            Prof_Begin( PROF_DLL_LOAD );
            convertInfo.Convert = (ConvertFunc_t)GetProcAddress( dll, "Convert" );
            Prof_End();

            assert( convertInfo.Convert );

            convertInfo.m_SourceDirectory = g_baseDir + info->m_SourceDirectory;

            convertInfo.m_RelativeTargetDirectory.clear();

            for ( i = 0; i < info->m_TargetDirectory.size(); i++ )
            {
                convertInfo.m_RelativeTargetDirectory.push_back( info->m_TargetDirectory[i] );
            }

            g_assetDir[ASSET_DIR_CURRENT]    = g_assetDir[ASSET_DIR_PLATFORM];
            g_srcCacheDir[ASSET_DIR_CURRENT] = g_srcCacheDir[ASSET_DIR_PLATFORM];
            g_tgtCacheDir[ASSET_DIR_CURRENT] = g_tgtCacheDir[ASSET_DIR_PLATFORM];

            SetupTargetDirectories( &convertInfo, info );

            convertInfo.m_typeName = info->m_Name;

            g_aborting = false;

            convertInfo.m_index = 0;
            convertInfo.m_count = numToConvert;

            for ( var = scope->m_variables.begin();
                  var != scope->m_variables.end() && !g_aborting;
                  ++var )
            {
                if ( !IsAssetForThisPlatform( ( *var )->m_name ) )
                {
                    continue;
                }

                entry = g_convertEntries.find( ( *var )->m_name );

                assert( entry != g_convertEntries.end() );

                if ( entry->second != CONVERT_STATE_PENDING )
                {
                    continue;
                }

                convertInfo.m_index++;

                if ( info->m_DllIsAll[pass] )
                {
                    if ( SetAssetDir( GetAssetDirType( ( *var )->m_name ) ) )
                    {
                        SetupTargetDirectories( &convertInfo, info );
                    }
                }

                convertInfo.m_entry = *var;

                entry->second = ConvertOneAsset( &convertInfo )
                                ? CONVERT_STATE_DONE : CONVERT_STATE_FAILED;
            }

            Prof_Begin( PROF_DLL_LOAD );
            FreeLibrary( dll );
            Prof_End();
        }
    }

    if ( assets )
    {
        std::set< std::string >::iterator it;

        for ( it = assets->begin(); it != assets->end(); ++it )
        {
            if ( IsAssetForThisPlatform( *it ) )
            {
                ReportMissingAsset( *it );
            }
        }
    }

    SaveCacheFilesAtExit();

    g_timeEntries.clear();
    g_convertEntries.clear();
    g_referencedEntries.clear();

    ClearCacheList( &g_targetFilesCache );
    ClearCacheList( &g_debugFilesCache );
    ClearCacheList( &g_sourceFilesCache );
}


/* ConvertInfo::ConvertInfo  0x0040b2a0 */
ConvertInfo::ConvertInfo()
{
    Convert          = 0;
    m_entry          = 0;
    m_cachedScope[0] = 0;
    m_cachedScope[1] = 0;
    m_error          = false;
    m_index          = 0;
    m_count          = 0;
}


/* ConvertInfo::~ConvertInfo  0x0040b310 */
ConvertInfo::~ConvertInfo()
{
}


/* ReportDllError  0x00408ee0 */
static void ReportDllError( int result, const std::string &fileName )
{
    const char *message;

    switch ( result + 3 )
    {
    case 0:
        message = va( "ERROR: Could not find 'Convert' in '%s'\n", fileName.c_str() );
        break;
    case 1:
        message = va( "Failed to load '%s'\n", fileName.c_str() );
        break;
    case 2:
        message = va( "Could not find '%s'\n", fileName.c_str() );
        break;
    case 3:
        return;
    default:
        message = va( "Failed to load '%s'\n", fileName.c_str() );

        if ( !g_cacheEntry )
        {
            printf( "\n************ ERROR ************\n" );
            printf( "%s", message );
            fflush( stdout );
            ErrorPrompt();
        }
        return;
    }

    Error( message );
}


/* OpenConvertDll  0x00409010 */
static int OpenConvertDll( HMODULE *dll, const std::string &fileName )
{
    struct _stat st;

    assert( dll );

    if ( _stat( fileName.c_str(), &st ) )
    {
        return -1;
    }

    *dll = LoadLibraryA( fileName.c_str() );

    if ( !*dll )
    {
        return -2;
    }

    if ( !GetProcAddress( *dll, "Convert" ) )
    {
        return -3;
    }

    return 0;
}


/* LoadConvertDll  0x00409110 */
static HMODULE LoadConvertDll( const std::string &name )
{
    std::string fileName;
    HMODULE     dll;
    int         result;

    fileName = g_baseDir + "deffiles" + "/" + name + ".dll";

    dll = 0;

    result = OpenConvertDll( &dll, fileName );

    ReportDllError( result, fileName );

    return result ? 0 : dll;
}


/* SetupTargetDirectories  0x00404ff0 */
static void SetupTargetDirectories( ConvertInfo *info, ObjectInfo *objectInfo )
{
    unsigned int i;

    info->m_TargetDirectory.clear();

    for ( i = 0; i < objectInfo->m_TargetDirectory.size(); i++ )
    {
        info->m_TargetDirectory.push_back( g_baseDir + g_assetDir[ASSET_DIR_CURRENT]
                                           + objectInfo->m_TargetDirectory[i] );
    }
}


/* ReadVersionFile  0x00405b30 */
static int ReadVersionFile( const std::string &fileName )
{
    FILE *f;
    int   version;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "rb" );
    Prof_End();

    if ( !f )
    {
        return 0;
    }

    version = 0;

    Prof_Begin( PROF_FILE_READ );
    fscanf( f, "%d", &version );
    Prof_End();

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();

    return version;
}


/* WriteVersionFile  0x00405bb0 */
static void WriteVersionFile( const std::string &fileName, int version )
{
    FILE *f;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "wb" );
    Prof_End();

    if ( !f )
    {
        return;
    }

    Prof_Begin( PROF_FILE_WRITE );
    fprintf( f, "%d", version );
    Prof_End();

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();
}


/* GetAssetDirType  0x00401470 */
static int GetAssetDirType( const std::string &name )
{
    return name.find( ':' ) == std::string::npos ? ASSET_DIR_PLATFORM : ASSET_DIR_DEFAULT;
}


/* SetAssetDir  0x004014c0 */
static bool SetAssetDir( int assetDirType )
{
    if ( !g_assetDir[ASSET_DIR_CURRENT].compare( 0, g_assetDir[ASSET_DIR_CURRENT].size(),
                                                 g_assetDir[assetDirType].c_str(),
                                                 g_assetDir[assetDirType].size() ) )
    {
        return false;
    }

    g_assetDir[ASSET_DIR_CURRENT]    = g_assetDir[assetDirType];
    g_srcCacheDir[ASSET_DIR_CURRENT] = g_srcCacheDir[assetDirType];
    g_tgtCacheDir[ASSET_DIR_CURRENT] = g_tgtCacheDir[assetDirType];

    return true;
}


/* IsAssetForThisPlatform  0x00409380 */
static bool IsAssetForThisPlatform( const std::string &name )
{
    int i;

    assert( g_platform != CONVERTER_PLATFORM_NOT_SET );

    if ( name.find( ':' ) == std::string::npos )
    {
        return true;
    }

    for ( i = 0; i < CONVERTER_PLATFORM_COUNT; i++ )
    {
        std::string prefix = g_platformNames[i] + ':';

        if ( !_strnicmp( name.c_str(), prefix.c_str(), prefix.size() ) )
        {
            return i == g_platform;
        }
    }

    Error( va( "Unknown platform [%s]\n", name.c_str() ) );

    return false;
}


/* AddSingleConvertAssets  0x004095a0 */
static void AddSingleConvertAssets( std::set< std::string > *singleConvertAssetList )
{
    fileListMap_t::iterator it;
    fileList_t             *list;
    int                     i;

    assert( singleConvertAssetList );

    if ( !g_changeListFiles.size() )
    {
        return;
    }

    for ( it = g_sourceFilesCache.begin(); it != g_sourceFilesCache.end(); ++it )
    {
        list = it->second;

        if ( !list || list->num <= 0 )
        {
            continue;
        }

        for ( i = 0; i < list->num; i++ )
        {
            if ( g_changeListFiles.find( std::string( list->names[i] ) )
                 != g_changeListFiles.end() )
            {
                singleConvertAssetList->insert( it->first );
            }
        }
    }
}


/* ReportMissingAsset  0x00409860 */
static void ReportMissingAsset( const std::string &name )
{
    fileListMap_t::iterator it;
    fileList_t             *list;
    char                  **targetFiles;
    int                     numTargetFiles;
    int                     i;

    it = g_targetFilesCache.find( name );

    if ( it == g_targetFilesCache.end() )
    {
        return;
    }

    list = it->second;

    if ( !list )
    {
        return;
    }

    targetFiles = list->names;

    assert( targetFiles );

    numTargetFiles = list->num;

    assert( numTargetFiles );

    for ( i = 0; i < numTargetFiles; i++ )
    {
        g_changeListTargets.insert( std::string( targetFiles[i] ) );
    }
}


/* IsAssetUpToDate  0x00406d10 */
static bool IsAssetUpToDate( ConvertInfo *info, bool quiet )
{
    fileListMap_t::iterator srcIt;
    fileListMap_t::iterator trgIt;
    fileList_t             *src;
    fileList_t             *trg;
    ValueScope             *cached;
    Variable               *cachedVar;
    char                  **sourceFiles;
    char                  **targetFiles;
    int                     numSourceFiles;
    int                     numTargetFiles;
    int                     assetDirType;
    int                     sourceTime;
    int                     targetTime;
    int                     time;
    int                     i;
    int                     j;
    std::string             name;
    std::string             relFileName;
    std::string             fullName;
    std::string             foundName;
    std::string             dir;
    std::string             sourceFile;
    std::string             targetFile;
    WIN32_FIND_DATAA        findData;
    HANDLE                  find;
    BOOL                    more;

    name = info->m_entry->m_name;

    if ( g_cleanConvert )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nclean convert\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_CLEAN_CONVERT]++;
            }
        }

        return false;
    }

    srcIt = g_sourceFilesCache.find( name );

    if ( srcIt == g_sourceFilesCache.end() )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nno source cache entry\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_MISSING_CACHE_ENTRY]++;
            }
        }

        return false;
    }

    src = srcIt->second;

    if ( src && src->num && src->names[0][0] == '?' )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nout of date\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_OUT_OF_DATE]++;
            }
        }

        return false;
    }

    trgIt = g_targetFilesCache.find( name );

    if ( trgIt == g_targetFilesCache.end() )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nno target cache entry\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_MISSING_CACHE_ENTRY]++;
            }
        }

        return false;
    }

    assetDirType = GetAssetDirType( name );

    assert( assetDirType == ASSET_DIR_DEFAULT || assetDirType == ASSET_DIR_PLATFORM );

    cached = info->m_cachedScope[assetDirType == ASSET_DIR_PLATFORM ? 1 : 0];

    if ( !cached )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nno gdt cache file\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_MISSING_CACHE_ENTRY]++;
            }
        }

        return false;
    }

    cachedVar = cached->Find( name );

    if ( !cachedVar )
    {
        if ( !quiet )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nno gdt cache entry\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_MISSING_CACHE_ENTRY]++;
            }
        }

        return false;
    }

    Prof_Begin( PROF_COMPARE_GDT_ENTRY );

    {
        static char textA[20000];
        static char textB[20000];
        buf_t       bufA;
        buf_t       bufB;

        InitBuf( &bufA, textA, sizeof( textA ) );
        InitBuf( &bufB, textB, sizeof( textB ) );

        info->m_entry->ToScript( 0, &bufA );
        cachedVar->ToScript( 0, &bufB );

        if ( strcmp( textA, textB ) )
        {
            Prof_End();

            if ( !quiet )
            {
                if ( g_spam )
                {
                    printf( "\nwill convert '%s':\nentry changed\n", name.c_str() );
                }
                else
                {
                    g_convertReasonCounts[CONVERT_REASON_ENTRY_CHANGED]++;
                }
            }

            return false;
        }
    }

    Prof_End();

    if ( quiet )
    {
        return true;
    }

    sourceTime = 0;
    targetTime = 0;

    src = srcIt->second;

    if ( src )
    {
        sourceFiles = src->names;

        assert( sourceFiles );

        numSourceFiles = src->num;

        assert( numSourceFiles );

        for ( i = 0; i < numSourceFiles; i++ )
        {
            if ( sourceFiles[i][0] == ':' )
            {
                continue;
            }

            relFileName = sourceFiles[i];

            if ( relFileName.empty() )
            {
                if ( g_spam )
                {
                    printf( "\nwill convert '%s':\nsource cache marked dirty\n", name.c_str() );
                }
                else
                {
                    g_convertReasonCounts[CONVERT_REASON_CACHE_DIRTY]++;
                }

                return false;
            }

            fullName = g_baseDir + relFileName;

            if ( IsWildcardFile( relFileName.c_str() ) )
            {
                Prof_Begin( PROF_WILDCARD_ASSET );
                find = FindFirstFileA( fullName.c_str(), &findData );
                Prof_End();

                if ( find == INVALID_HANDLE_VALUE )
                {
                    printf( "\nwill convert '%s':\ncannot find '%s'\n",
                            name.c_str(), fullName.c_str() );

                    return false;
                }

                dir = GetPathFromFileName( relFileName.c_str() );

                for ( ;; )
                {
                    foundName = dir + findData.cFileName;

                    for ( j = numSourceFiles - 1; j >= 0; j-- )
                    {
                        if ( !_stricmp( sourceFiles[j], foundName.c_str() ) )
                        {
                            break;
                        }
                    }

                    fullName = g_baseDir + foundName;

                    if ( j < 0 )
                    {
                        FindClose( find );

                        if ( g_spam )
                        {
                            printf( "\nwill convert '%s':\n'%s' is new\n",
                                    name.c_str(), fullName.c_str() );
                        }
                        else
                        {
                            g_convertReasonCounts[CONVERT_REASON_NEW_SOURCE_FILE]++;
                        }

                        return false;
                    }

                    time = GetFileTime( fullName );

                    if ( !time )
                    {
                        FindClose( find );

                        if ( g_spam )
                        {
                            printf( "\nwill convert '%s':\ncannot find '%s'\n",
                                    name.c_str(), fullName.c_str() );
                        }
                        else
                        {
                            g_convertReasonCounts[CONVERT_REASON_MISSING_FILE]++;
                        }

                        return false;
                    }

                    if ( !sourceTime || time > sourceTime )
                    {
                        sourceTime = time;
                        sourceFile = fullName;
                    }

                    Prof_Begin( PROF_WILDCARD_ASSET );
                    more = FindNextFileA( find, &findData );
                    Prof_End();

                    if ( !more )
                    {
                        break;
                    }
                }

                FindClose( find );
            }
            else
            {
                time = GetFileTime( fullName );

                if ( !time )
                {
                    if ( g_spam )
                    {
                        printf( "\nwill convert '%s':\ncannot find '%s'\n",
                                name.c_str(), fullName.c_str() );
                    }
                    else
                    {
                        g_convertReasonCounts[CONVERT_REASON_MISSING_FILE]++;
                    }

                    return false;
                }

                if ( !sourceTime || time > sourceTime )
                {
                    sourceTime = time;
                    sourceFile = fullName;
                }
            }

            assert( sourceTime );
        }
    }

    trg = trgIt->second;

    if ( !trg )
    {
        return true;
    }

    targetFiles = trg->names;

    assert( targetFiles );

    numTargetFiles = trg->num;

    assert( numTargetFiles );

    for ( i = 0; i < numTargetFiles; i++ )
    {
        relFileName = targetFiles[i];
        fullName    = g_baseDir + relFileName;

        if ( relFileName.empty() )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\ntarget cache marked dirty\n", name.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_CACHE_DIRTY]++;
            }

            return false;
        }

        if ( IsWildcardFile( relFileName.c_str() ) )
        {
            Prof_Begin( PROF_WILDCARD_ASSET );
            find = FindFirstFileA( fullName.c_str(), &findData );
            Prof_End();

            if ( find == INVALID_HANDLE_VALUE )
            {
                if ( g_spam )
                {
                    printf( "\nwill convert '%s':\ncannot find '%s'\n",
                            name.c_str(), fullName.c_str() );
                }
                else
                {
                    g_convertReasonCounts[CONVERT_REASON_MISSING_FILE]++;
                }

                return false;
            }

            dir = GetPathFromFileName( relFileName.c_str() );

            for ( ;; )
            {
                foundName = dir + findData.cFileName;

                fullName = g_baseDir + foundName;

                time = GetFileTime( fullName );

                if ( !time )
                {
                    FindClose( find );

                    if ( g_spam )
                    {
                        printf( "\nwill convert '%s':\ncannot find '%s'\n",
                                name.c_str(), fullName.c_str() );
                    }
                    else
                    {
                        g_convertReasonCounts[CONVERT_REASON_MISSING_FILE]++;
                    }

                    return false;
                }

                if ( !targetTime || time < targetTime )
                {
                    targetTime = time;
                    targetFile = fullName;
                }

                Prof_Begin( PROF_WILDCARD_ASSET );
                more = FindNextFileA( find, &findData );
                Prof_End();

                if ( !more )
                {
                    break;
                }
            }

            FindClose( find );
        }
        else
        {
            time = GetFileTime( fullName );

            if ( !time )
            {
                if ( g_spam )
                {
                    printf( "\nwill convert '%s':\ncannot find '%s'\n",
                            name.c_str(), fullName.c_str() );
                }
                else
                {
                    g_convertReasonCounts[CONVERT_REASON_MISSING_FILE]++;
                }

                return false;
            }

            if ( !targetTime || time < targetTime )
            {
                targetTime = time;
                targetFile = fullName;
            }
        }
    }

    assert( targetTime );

    if ( sourceTime && sourceTime > targetTime )
    {
        if ( g_spam )
        {
            printf( "\nwill convert '%s':\n'%s' > '%s'\n",
                    name.c_str(), sourceFile.c_str(), targetFile.c_str() );
        }
        else
        {
            g_convertReasonCounts[CONVERT_REASON_CHANGED_SOURCE_FILE]++;
        }

        return false;
    }

    if ( !info->m_versionChanged )
    {
        return true;
    }

    SetAssetDir( assetDirType );

    for ( i = 0; i < numTargetFiles; i++ )
    {
        targetCache_t cache;

        relFileName = targetFiles[i];

        assert( !relFileName.empty() );

        if ( !ReadTargetCacheFile( relFileName, &cache ) )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\ncannot find cache file for '%s'\n",
                        name.c_str(), relFileName.c_str() );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_MISSING_CACHE_ENTRY]++;
            }

            return false;
        }

        if ( cache.m_version != info->m_version )
        {
            if ( g_spam )
            {
                printf( "\nwill convert '%s':\nversion changed (%d -> %d)\n",
                        name.c_str(), cache.m_version, info->m_version );
            }
            else
            {
                g_convertReasonCounts[CONVERT_REASON_NEW_VERSION]++;
            }

            return false;
        }
    }

    return true;
}


/* AddFilesToList  0x00402660 */
static void AddFilesToList( fileList_t **list, std::set< std::string > *files )
{
    std::set< std::string >::iterator it;
    fileList_t *entry;
    fileList_t *newEntry;
    int         size;
    int         num;
    int         oldNum;
    int         headerSize;
    int         stringSize;
    int         offset;
    int         delta;
    int         i;

    Prof_Begin( PROF_CACHE_FILE );

    size = 0;
    num  = 0;

    for ( it = files->begin(); it != files->end(); ++it )
    {
        size += (int)it->size() + 5;
        num++;
    }

    if ( !num )
    {
        Prof_End();
        return;
    }

    entry = *list;

    if ( entry )
    {
        size += entry->len;
        num  += entry->num;
    }
    else
    {
        size += 8;
    }

    newEntry = (fileList_t *)malloc( size );
    newEntry->num = num;
    newEntry->len = size;

    if ( entry )
    {
        oldNum     = entry->num;
        headerSize = oldNum * 4 + 8;
        stringSize = entry->len - headerSize;

        memcpy( &newEntry->names[num], (char *)entry + headerSize, stringSize );

        delta = ( num - oldNum ) * 4 + (int)( (char *)newEntry - (char *)entry );

        for ( i = 0; i < oldNum; i++ )
        {
            newEntry->names[i] = entry->names[i] + delta;
        }

        offset = entry->len + ( num - oldNum ) * 4;

        free( entry );
    }
    else
    {
        oldNum = 0;
        offset = num * 4 + 8;
    }

    i = oldNum;

    for ( it = files->begin(); it != files->end(); ++it )
    {
        newEntry->names[i] = (char *)newEntry + offset;

        memcpy( newEntry->names[i], it->c_str(), it->size() + 1 );

        offset += (int)it->size() + 1;
        i++;
    }

    *list = newEntry;

    Prof_End();
}


/* ConvertError  0x004024a0 */
static void ConvertError( ConvertData *data, const char *message )
{
    printf( "\n************ ERROR ************\n" );
    printf( "%s", message );

    data->m_info->m_error = true;
}


/* AddReferencedEntries  0x004049d0 */
static void AddReferencedEntries( fileList_t *list, std::string name )
{
    int i;

    if ( !list )
    {
        return;
    }

    assert( list->num );

    for ( i = list->num - 1; i >= 0; i-- )
    {
        if ( !list->names[i][0] )
        {
            continue;
        }

        g_referencedEntries[ std::string( list->names[i] ) ].insert( name );
    }
}


/* ClearRegeneratedFiles  0x00404b60 */
static void ClearRegeneratedFiles( fileListMap_t *cache, const std::string &name,
                              std::set< std::string > *files, bool *dirty )
{
    fileListMap_t::iterator it;
    fileList_t             *list;
    int                     i;

    it = cache->find( name );

    if ( it == cache->end() )
    {
        return;
    }

    list = it->second;

    if ( !list )
    {
        return;
    }

    assert( list->num );

    for ( i = 0; i < list->num; i++ )
    {
        if ( !list->names[i][0] )
        {
            continue;
        }

        if ( files->find( std::string( list->names[i] ) ) != files->end() )
        {
            *dirty = true;
            list->names[i] = "";
        }
    }
}


/* RemoveCachedFiles  0x00404cf0 */
static void RemoveCachedFiles( fileListMap_t *cache, ConvertInfo *info, std::string name,
                               bool *dirty )
{
    fileListMap_t::iterator                                    it;
    std::map< std::string, std::set< std::string > >::iterator ref;
    fileList_t                                                *list;
    const char                                                *fileName;
    int                                                        i;

    it = cache->find( name );

    if ( it == cache->end() )
    {
        return;
    }

    list = it->second;

    *dirty = true;

    cache->erase( name );

    if ( !list )
    {
        return;
    }

    assert( list->num );

    for ( i = 0; i < list->num; i++ )
    {
        fileName = list->names[i];

        if ( !fileName[0] )
        {
            continue;
        }

        MarkConvertedEntries( info );

        ref = g_referencedEntries.find( std::string( fileName ) );

        if ( ref != g_referencedEntries.end() )
        {
            ref->second.erase( name );

            if ( ref->second.size() )
            {
                continue;
            }

            g_referencedEntries.erase( std::string( fileName ) );
        }

        DeleteAssetFiles( fileName );
        DeleteTargetCacheFile( std::string( fileName ) );
    }

    free( list );
}


/* CollectMissingEntries  0x00408e00 */
static void CollectMissingEntries( fileListMap_t *cache, ValueScope *scope,
                                   std::set< std::string > *missing )
{
    fileListMap_t::iterator it;

    for ( it = cache->begin(); it != cache->end(); ++it )
    {
        if ( !scope->Find( it->first ) )
        {
            missing->insert( it->first );
        }
    }
}


/* CacheFiles  0x00405140 */
static void CacheFiles( fileListMap_t *cache, std::string name, bool *dirty,
                        std::set< std::string > *files, bool keepEmpty )
{
    fileListMap_t::iterator it;

    *dirty = true;

    ( *cache )[name] = 0;

    it = cache->find( name );

    Prof_Begin( PROF_REF_COUNT );

    AddFilesToList( &it->second, files );
    AddReferencedEntries( it->second, name );

    Prof_End();

    if ( !keepEmpty && !it->second )
    {
        cache->erase( name );
    }
}


/* ReadTargetCacheFile  0x00405250 */
static bool ReadTargetCacheFile( std::string relTrgFileName, targetCache_t *cache )
{
    std::string fileName;
    FILE       *f;
    char        fileBuf[BUFFER_LEN];
    char        keyBuf[BUFFER_LEN];

    if ( IsWildcardFile( relTrgFileName.c_str() ) )
    {
        relTrgFileName = relTrgFileName.substr( 0, relTrgFileName.size() - 1 );
        assert( !IsWildcardFile( relTrgFileName.c_str() ) );
    }

    fileName = g_tgtCacheDir[ASSET_DIR_CURRENT] + relTrgFileName;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "rb" );
    Prof_End();

    if ( !f )
    {
        return false;
    }

    cache->m_size     = 0;
    cache->m_checksum = 0;
    cache->m_version  = 0;

    fileBuf[0] = 0;
    keyBuf[0]  = 0;

    Prof_Begin( PROF_FILE_READ );

    if ( fscanf( f, "%s%s%d%u%d", fileBuf, keyBuf, &cache->m_size, &cache->m_checksum,
                 &cache->m_version ) != 5 )
    {
        Prof_End();
        return false;
    }

    Prof_End();

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();

    assert( strlen( fileBuf ) < BUFFER_LEN );
    assert( strlen( keyBuf ) < BUFFER_LEN );

    cache->m_gdfName.assign( fileBuf, strlen( fileBuf ) );
    cache->m_assetName.assign( keyBuf, strlen( keyBuf ) );

    return true;
}


/* WriteTargetCacheFile  0x00405670 */
static void WriteTargetCacheFile( std::string relTrgFileName, targetCache_t *cache )
{
    std::string fileName;
    FILE       *f;

    if ( IsWildcardFile( relTrgFileName.c_str() ) )
    {
        relTrgFileName = relTrgFileName.substr( 0, relTrgFileName.size() - 1 );
        assert( !IsWildcardFile( relTrgFileName.c_str() ) );
    }

    fileName = g_tgtCacheDir[ASSET_DIR_CURRENT] + relTrgFileName;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( fileName.c_str(), "wb" );
    Prof_End();

    if ( !f )
    {
        return;
    }

    Prof_Begin( PROF_FILE_WRITE );
    fprintf( f, "%s\n%s\n%d\n%u\n%d\n", cache->m_gdfName.c_str(), cache->m_assetName.c_str(),
             cache->m_size, cache->m_checksum, cache->m_version );
    Prof_End();

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();
}


/* DeleteTargetCacheFile  0x004058d0 */
static void DeleteTargetCacheFile( std::string relTrgFileName )
{
    std::string fileName;

    if ( IsWildcardFile( relTrgFileName.c_str() ) )
    {
        relTrgFileName = relTrgFileName.substr( 0, relTrgFileName.size() - 1 );
        assert( !IsWildcardFile( relTrgFileName.c_str() ) );
    }

    fileName = g_tgtCacheDir[ASSET_DIR_PLATFORM] + relTrgFileName;

    Prof_Begin( PROF_FILE_DELETE );
    DeleteFileA( fileName.c_str() );
    Prof_End();

    if ( g_tgtCacheDir[ASSET_DIR_PLATFORM] != g_tgtCacheDir[ASSET_DIR_DEFAULT] )
    {
        fileName = g_tgtCacheDir[ASSET_DIR_DEFAULT] + relTrgFileName;

        Prof_Begin( PROF_FILE_DELETE );
        DeleteFileA( fileName.c_str() );
        Prof_End();
    }
}


/* UpdateGdtTargetFile  0x00405c90 */
static void UpdateGdtTargetFile( const std::string &fileName, bool missing )
{
    if ( fileName.size() <= 4 )
    {
        return;
    }

    if ( Q_stricmp( fileName.c_str() + fileName.size() - 4, ".gdt" ) )
    {
        return;
    }

    RemoveGdtEntries( fileName );

    if ( !missing )
    {
        AddGdtFileCallback( fileName );
    }
}


/* MarkTargetFile  0x00405cf0 */
static void MarkTargetFile( ConvertData *data, const std::string &fileName, bool unchanged )
{
    std::set< std::string >                                    entries;
    std::set< std::string >::iterator                          it;
    std::map< std::string, char >::iterator                    entry;
    std::map< std::string, std::set< std::string > >::iterator ref;
    targetCache_t     cache;
    std::string       fullName;
    std::string       dir;
    std::string       name;
    std::string       entryName;
    WIN32_FIND_DATAA  findData;
    HANDLE            find;
    FILE             *f;
    char             *buf;
    int               len;
    int               size;
    unsigned          checksum;
    bool              missing;
    bool              hadError;
    bool              conflict;
    BOOL              more;

    missing  = false;
    hadError = data->m_info->m_error;

    fullName = g_baseDir + fileName;

    size     = 0;
    checksum = 0;

    if ( IsWildcardFile( fileName.c_str() ) )
    {
        Prof_Begin( PROF_WILDCARD_ASSET );
        find = FindFirstFileA( fullName.c_str(), &findData );
        Prof_End();

        if ( find == INVALID_HANDLE_VALUE )
        {
            if ( !data->m_info->m_error )
            {
                ConvertError( data, va( "file '%s' was not generated\n", fullName.c_str() ) );
            }

            return;
        }

        dir = GetPathFromFileName( fullName.c_str() );

        for ( ;; )
        {
            name = dir + findData.cFileName;

            Prof_Begin( PROF_FILE_OPEN );
            f = fopen( name.c_str(), "rb" );
            Prof_End();

            if ( !f )
            {
                if ( !hadError )
                {
                    ConvertError( data, va( "file '%s' was not generated\n", name.c_str() ) );
                }

                missing = true;
                break;
            }

            Prof_Begin( PROF_FILE_SEEK );
            fseek( f, 0, SEEK_END );
            len = ftell( f );
            fseek( f, 0, SEEK_SET );
            Prof_End();

            if ( !len )
            {
                Prof_Begin( PROF_FILE_OPEN );
                fclose( f );
                Prof_End();

                if ( !hadError )
                {
                    ConvertError( data, va( "file '%s' was generated, but is 0 size (deleting).\n",
                                            name.c_str() ) );
                }

                DeleteFileA( name.c_str() );

                missing = true;
                break;
            }

            buf = (char *)malloc( len );

            Prof_Begin( PROF_FILE_READ );
            fread( buf, 1, len, f );
            Prof_End();

            Prof_Begin( PROF_FILE_OPEN );
            fclose( f );
            Prof_End();

            checksum += CRC32( buf, len, 0 );

            free( buf );

            size += len;

            Prof_Begin( PROF_WILDCARD_ASSET );
            more = FindNextFileA( find, &findData );
            Prof_End();

            if ( !more )
            {
                break;
            }
        }

        Prof_Begin( PROF_WILDCARD_ASSET );
        FindClose( find );
        Prof_End();
    }
    else if ( unchanged && ReadTargetCacheFile( fileName, &cache ) )
    {
        size     = cache.m_size;
        checksum = cache.m_checksum;
    }
    else
    {
        Prof_Begin( PROF_FILE_OPEN );
        f = fopen( fullName.c_str(), "rb" );
        Prof_End();

        if ( !f )
        {
            if ( !hadError )
            {
                ConvertError( data, va( "file '%s' was not generated\n", fullName.c_str() ) );
            }

            missing = true;
        }
        else
        {
            Prof_Begin( PROF_FILE_SEEK );
            fseek( f, 0, SEEK_END );
            len = ftell( f );
            fseek( f, 0, SEEK_SET );
            Prof_End();

            if ( !len )
            {
                Prof_Begin( PROF_FILE_OPEN );
                fclose( f );
                Prof_End();

                if ( !hadError )
                {
                    ConvertError( data, va( "file '%s' was generated, but is 0 size (deleting).\n",
                                            fullName.c_str() ) );
                }

                DeleteFileA( fullName.c_str() );

                missing = true;
            }
            else
            {
                buf = (char *)malloc( len );

                Prof_Begin( PROF_FILE_READ );
                fread( buf, 1, len, f );
                Prof_End();

                Prof_Begin( PROF_FILE_OPEN );
                fclose( f );
                Prof_End();

                checksum = CRC32( buf, len, 0 );

                free( buf );

                size = len;
            }
        }
    }

    UpdateGdtTargetFile( fileName, missing );

    if ( missing )
    {
        DeleteTargetCacheFile( fileName );
    }
    else
    {
        if ( ReadTargetCacheFile( fileName, &cache )
             && cache.m_size == size
             && cache.m_checksum == checksum
             && cache.m_version == data->m_info->m_version )
        {
            return;
        }

        cache.m_gdfName   = data->m_info->m_gdfName;
        cache.m_assetName = data->GetAssetName();
        cache.m_size      = size;
        cache.m_checksum  = checksum;
        cache.m_version   = data->m_info->m_version;

        WriteTargetCacheFile( fileName, &cache );
    }

    MarkConvertedEntries( data->m_info );

    ref = g_referencedEntries.find( fileName );

    if ( ref == g_referencedEntries.end() )
    {
        return;
    }

    entries = ref->second;

    conflict = false;

    for ( it = entries.begin(); it != entries.end(); ++it )
    {
        entryName = *it;

        entry = g_convertEntries.find( entryName );

        if ( entry != g_convertEntries.end() )
        {
            if ( entry->second != CONVERT_STATE_DONE )
            {
                continue;
            }

            entry->second = CONVERT_STATE_FAILED;
        }

        conflict = true;

        RemoveCachedFiles( &g_targetFilesCache, data->m_info, entryName,
                           &g_saveTargetFilesCache );

        TouchSourceCacheEntry( entryName );

        printf( "\n************ ERROR ************\n" );
        printf( "%s", va( "file '%s' has a conflict between entry '%s' and '%s' of '%s'\n",
                          fullName.c_str(), data->GetAssetName(), entryName.c_str(),
                          data->m_info->m_gdfName.c_str() ) );

        data->m_info->m_error = true;
    }

    if ( !conflict )
    {
        return;
    }

    for ( it = entries.begin(); it != entries.end(); ++it )
    {
        entryName = *it;

        entry = g_convertEntries.find( entryName );

        if ( entry == g_convertEntries.end() || entry->second != CONVERT_STATE_PENDING )
        {
            continue;
        }

        entry->second = CONVERT_STATE_FAILED;

        RemoveCachedFiles( &g_targetFilesCache, data->m_info, entryName,
                           &g_saveTargetFilesCache );

        TouchSourceCacheEntry( entryName );
    }
}


/* MarkTargetFiles  0x00406c20 */
static void MarkTargetFiles( ConvertData *data )
{
    std::set< std::string >::iterator it;

    for ( it = g_newTargetFiles.begin(); it != g_newTargetFiles.end(); ++it )
    {
        MarkTargetFile( data, *it, g_changedTargetFiles.find( *it ) == g_changedTargetFiles.end() );
    }
}


/* MarkConvertedEntries  0x00408160 */
static void MarkConvertedEntries( ConvertInfo *info )
{
    fileListMap_t::iterator it;

    if ( info->m_marked )
    {
        return;
    }

    info->m_marked = true;

    Prof_Begin( PROF_REF_COUNT );

    for ( it = g_targetFilesCache.begin(); it != g_targetFilesCache.end(); ++it )
    {
        AddReferencedEntries( it->second, it->first );
    }

    for ( it = g_debugFilesCache.begin(); it != g_debugFilesCache.end(); ++it )
    {
        AddReferencedEntries( it->second, it->first );
    }

    Prof_End();
}


#define CONVERT_STACK_RESIDUE   0x0045e68b
#define CONVERT_STACK_DEPTH     0x34

static int CallConvertDll( ConvertFunc_t convert, ConvertData *data )
{
    int result;

    __asm
    {
        mov     eax, data
        mov     ecx, convert
        sub     esp, CONVERT_STACK_DEPTH
        push    CONVERT_STACK_RESIDUE
        add     esp, CONVERT_STACK_DEPTH + 4
        push    eax
        call    ecx
        add     esp, 4
        mov     result, eax
    }

    return result;
}


/* ConvertOneAsset  0x004083e0 */
static bool ConvertOneAsset( ConvertInfo *info )
{
    ConvertData  data;
    const char  *relFileName;
    unsigned int i;
    char         newFilename[BUFFER_LEN];

    assert( !g_newTargetFiles.size() );
    assert( !g_changedTargetFiles.size() );
    assert( !g_newDebugFiles.size() );
    assert( !g_newSourceFiles );

    if ( g_spam )
    {
        printf( "\n" );
    }

    for ( i = 0; i < info->m_RelativeTargetDirectory.size(); i++ )
    {
        CreateDirs( g_srcCacheDir[ASSET_DIR_PLATFORM],
                    g_assetDir[ASSET_DIR_CURRENT] + info->m_RelativeTargetDirectory[i] );
        CreateDirs( g_tgtCacheDir[ASSET_DIR_PLATFORM],
                    g_assetDir[ASSET_DIR_CURRENT] + info->m_RelativeTargetDirectory[i] );
        CreateDirs( g_baseDir,
                    g_assetDir[ASSET_DIR_CURRENT] + info->m_RelativeTargetDirectory[i] );
    }

    data.SetVariable( info->m_entry );
    data.m_info    = info;
    data.m_version = CONVERTER_VERSION;

    info->m_error = false;

    relFileName = ( (ValueData *)info->m_entry->m_value )->m_sourceFileName.c_str();

    if ( relFileName[0] )
    {
        newFilename[0] = ':';

        assert( strlen( relFileName ) < sizeof( newFilename ) - 1 );

        strcpy( newFilename + 1, relFileName );

        AddToFileList( &g_newSourceFiles, newFilename, false );
    }

    if ( g_cacheEnabled )
    {
        Server_BeginAsset( &data, info->m_version );
    }

    printf( "%s '", "Converting" );
    SetConsoleTextAttribute( g_consoleHandle, 11 );
    printf( "%s", info->m_entry->m_name.c_str() );
    SetConsoleTextAttribute( g_consoleHandle, g_consoleAttributes );
    printf( "' (%s %i of %i)...\n", info->m_typeName.c_str(), info->m_index, info->m_count );
    fflush( stdout );

    g_cacheEnabled = false;

    Prof_Begin( PROF_DLL_CONVERT );

    if ( CallConvertDll( info->Convert, &data ) )
    {
        Prof_End();
        info->m_error = true;
    }
    else
    {
        Prof_End();
    }

    MarkConvertedFiles( &data, info->m_entry->m_name );

    return !info->m_error;
}


static void MarkConvertedFiles( ConvertData *data, const std::string &name )
{
    ConvertInfo *info;

    info = data->m_info;

    ClearRegeneratedFiles( &g_targetFilesCache, name, &g_newTargetFiles, &g_saveTargetFilesCache );
    ClearRegeneratedFiles( &g_debugFilesCache, name, &g_newDebugFiles, &g_saveDebugFilesCache );

    RemoveCachedFiles( &g_targetFilesCache, info, name, &g_saveTargetFilesCache );
    RemoveCachedFiles( &g_debugFilesCache, info, name, &g_saveDebugFilesCache );

    CacheFiles( &g_targetFilesCache, name, &g_saveTargetFilesCache, &g_newTargetFiles, true );
    CacheFiles( &g_debugFilesCache, name, &g_saveDebugFilesCache, &g_newDebugFiles, false );

    Prof_Begin( PROF_MARK_FILES_MISC );
    MarkTargetFiles( data );
    Prof_End();

    g_newTargetFiles.clear();
    g_changedTargetFiles.clear();
    g_newDebugFiles.clear();

    if ( info->m_error )
    {
        RemoveCachedFiles( &g_targetFilesCache, info, name, &g_saveTargetFilesCache );

        if ( g_newSourceFiles )
        {
            assert( g_newSourceFiles->num );

            if ( g_newSourceFiles->names[0][0] == ':' )
            {
                g_newSourceFiles->names[0][0] = '?';
                g_saveSourceFilesCache = true;
                g_sourceFilesCache[name] = g_newSourceFiles;
            }
            else
            {
                free( g_newSourceFiles );
            }
        }

        ErrorPrompt();
    }
    else
    {
        g_saveSourceFilesCache = true;
        g_sourceFilesCache[name] = g_newSourceFiles;
    }

    g_newSourceFiles = 0;
}


/* ConvertGdf  0x0040b420 */
static void ConvertGdf( const std::string &gdfName, std::set< std::string > *assets )
{
    DefFileMap_t::iterator it;
    ObjectInfo            *info;
    unsigned int           numPasses;
    unsigned int           pass;

    if ( g_spam )
    {
        printf( "Converting '%s'...\n", gdfName.c_str() );
    }

    GetDefFile( gdfName );

    it = g_defFiles.find( gdfName );

    if ( it == g_defFiles.end() )
    {
        return;
    }

    info = it->second;

    assert( info );

    numPasses = info->m_Dll.size();

    if ( !numPasses )
    {
        printf( "No convert DLL Specified.\n" );
        return;
    }

    for ( pass = 0; pass < info->m_Dll.size(); pass++ )
    {
        if ( g_spam && info->m_Dll.size() > 1 )
        {
            printf( "  pass %i of %i on '%s'...\n", pass + 1, info->m_Dll.size(), gdfName.c_str() );
        }

        ConvertGdfPass( gdfName, assets, info, pass );
    }
}


/* ProcessGdfFiles  0x0040b9e0 */
static void ProcessGdfFiles( void )
{
    FILE *f;
    char  line[1024];
    char *name;

    f = fopen( g_gdfFilesFileName, "r" );

    if ( !f )
    {
        printf( "'%s' not found\n", g_gdfFilesFileName );
        return;
    }

    if ( g_singleConvertAssets.size() )
    {
        while ( ( name = ReadLine( f, line, sizeof( line ) ) ) != NULL )
        {
            if ( g_skipTypes.find( std::string( name ) ) != g_skipTypes.end() )
            {
                break;
            }

            std::map< std::string, std::set< std::string > * >::iterator it =
                g_singleConvertAssets.find( std::string( name ) );

            if ( it == g_singleConvertAssets.end() )
            {
                assert( !g_processChangeList );
                continue;
            }

            ConvertGdf( std::string( name ), it->second );
        }
    }
    else
    {
        assert( !g_processChangeList );

        while ( ( name = ReadLine( f, line, sizeof( line ) ) ) != NULL )
        {
            if ( g_skipTypes.find( std::string( name ) ) != g_skipTypes.end() )
            {
                break;
            }

            ConvertGdf( std::string( name ), NULL );
        }
    }

    fclose( f );
}


/* CacheGdtFile  0x0040ce90 */
static void CacheGdtFile( int assetDirType, std::string fileName, const char *data, int len )
{
    std::string  name;
    FILE        *f;

    CreateDirs( g_srcCacheDir[assetDirType], fileName );

    name = g_srcCacheDir[assetDirType] + fileName;

    Prof_Begin( PROF_FILE_OPEN );
    f = fopen( name.c_str(), "wb" );
    Prof_End();

    if ( !f )
    {
        return;
    }

    Prof_Begin( PROF_FILE_WRITE );

    if ( (int)fwrite( data, 1, len, f ) != len )
    {
        Prof_End();
        Error( va( "Short write in CacheConvertList of '%s'\n", fileName.c_str() ) );
    }
    else
    {
        Prof_End();
    }

    Prof_Begin( PROF_FILE_OPEN );
    fclose( f );
    Prof_End();
}


/* CacheGdtBuf  0x0040d0d0 */
static void CacheGdtBuf( std::string fileName, gdtBuf_t *buf )
{
    if ( !buf->buf )
    {
        return;
    }

    CacheGdtFile( ASSET_DIR_DEFAULT, fileName, buf->buf, buf->len );
    CacheGdtFile( ASSET_DIR_SHARED, fileName, buf->buf, buf->len );

    free( buf->buf );

    buf->buf = 0;
    buf->len = 0;
}


/* CacheConvertedFiles  0x0040d1d0 */
static void CacheConvertedFiles( void )
{
    gdtFileMap_t::iterator it;

    if ( !g_convertFileNames.size() )
    {
        return;
    }

    if ( g_spam )
    {
        printf( "Caching converted files...\n" );
    }

    for ( it = g_convertFileNames.begin(); it != g_convertFileNames.end(); ++it )
    {
        CacheGdtBuf( it->first, &it->second );
    }
}


/* WriteMyChanges  0x0040e5d0 */
static void WriteMyChanges( void )
{
}


/* main  0x0040eb90 */
int main( int argc, char **argv )
{
    char *currDir;

    timeGetTime();

    currDir = _getcwd( NULL, 0 );
    SetBaseDir( currDir, 0 );
    free( currDir );

    g_assetChangeListDir = g_baseDir + "assetchangelist/";

    InitGlobals();

    if ( !ReadConverterCfg() || !ParseCommandLine( argc, argv ) || !ValidateCacheOptions() )
    {
        Usage();
        return 1;
    }

    BuildProcessSemaphoreFileName();

    if ( CheckProcessSemaphore() )
    {
        printf( "\nERROR: You can't run multiple copies of converter.exe.  Exiting.\n" );
        return 1;
    }

    InitConsole();

    Prof_Init();

    atexit( SaveCacheFilesAtExit );

    SetPlatform();

    assert( g_platform != CONVERTER_PLATFORM_NOT_SET );

    if ( g_platform == CONVERTER_PLATFORM_XENON )
    {
        InitXenonDeleteDirs();
    }

    InitCacheDirs( ASSET_DIR_DEFAULT );
    InitCacheDirs( ASSET_DIR_SHARED );

    if ( g_processChangeList )
    {
        LoadChangeListFiles();
        ProcessChangeListEntries();
    }

    Prof_Begin( PROF_CONVERT_MISC );
    ProcessGdfFiles();
    Prof_End();

    Prof_Begin( PROF_CACHE_CONVERT );
    CacheConvertedFiles();
    Prof_End();

    WriteMyChanges();

    Prof_Finish();

    printf( "\n" );

    if ( g_spam )
    {
        Prof_Print();
    }

    Server_Shutdown();

    PrintDone();

    DeleteFileA( sys_processSemaphoreFile );

    return 0;
}


#define ALLOC_ARENA_SIZE    0x20000000
#define ALLOC_RECENT_COUNT  16

typedef struct allocBlock_s
{
    struct allocBlock_s *m_next;                        /* +0x00 */
    int                  m_size;                        /* +0x04 */
} allocBlock_t;

typedef struct
{
    void *m_ptr;                                        /* +0x00 */
    int   m_size;                                       /* +0x04 */
} allocRecent_t;

static char           g_allocArena[ALLOC_ARENA_SIZE];   /* 0x004f5918 */
static allocRecent_t  g_allocRecent[ALLOC_RECENT_COUNT];/* 0x204f5918 */
static int            g_allocRecentIndex;               /* 0x204f5998 */
static allocBlock_t  *g_allocFreeList;                  /* 0x204f59c0 */
static int            g_allocUsed;                      /* 0x204f59e8 */


/* operator new  0x0040ee40 */
void *operator new( size_t size )
{
    allocBlock_t *block;
    allocBlock_t *next;
    char         *mem;
    int           remaining;

    size = ( size + 3 ) & ~3;

    mem   = 0;
    block = g_allocFreeList;

    if ( block )
    {
        next = block->m_next;

        g_allocFreeList = next;

        remaining = block->m_size - (int)size;

        if ( remaining >= 0 )
        {
            mem = (char *)block;

            if ( remaining >= 8 )
            {
                block = (allocBlock_t *)( (char *)block + size );

                block->m_next = next;
                block->m_size = remaining;

                g_allocFreeList = block;
            }
        }
    }

    if ( !mem )
    {
        if ( (unsigned)( g_allocUsed + size ) >= ALLOC_ARENA_SIZE )
        {
            return malloc( size );
        }

        mem = g_allocArena + g_allocUsed;

        g_allocUsed += (int)size;
    }

    if ( size >= 8 )
    {
        g_allocRecentIndex = ( g_allocRecentIndex + 1 ) & ( ALLOC_RECENT_COUNT - 1 );

        g_allocRecent[g_allocRecentIndex].m_ptr  = mem;
        g_allocRecent[g_allocRecentIndex].m_size = (int)size;
    }

    return mem;
}


/* operator delete  0x0040ee50 */
void operator delete( void *ptr )
{
    allocBlock_t *block;
    int           size;
    unsigned      offset;
    int           i;

    offset = (unsigned)( (char *)ptr - g_allocArena );

    if ( offset >= ALLOC_ARENA_SIZE )
    {
        free( ptr );
        return;
    }

    for ( i = 0; i < ALLOC_RECENT_COUNT; i++ )
    {
        if ( g_allocRecent[i].m_ptr == ptr )
        {
            break;
        }
    }

    if ( i == ALLOC_RECENT_COUNT || !ptr )
    {
        return;
    }

    if ( i == g_allocRecentIndex )
    {
        g_allocRecentIndex = ( g_allocRecentIndex - 1 ) & ( ALLOC_RECENT_COUNT - 1 );
    }

    g_allocRecent[i].m_ptr = 0;

    size = g_allocRecent[i].m_size;

    if ( (char *)ptr + size == g_allocArena + g_allocUsed )
    {
        g_allocUsed = (int)offset;
        return;
    }

    block = (allocBlock_t *)ptr;

    if ( (char *)g_allocFreeList == (char *)ptr + size )
    {
        block->m_next = g_allocFreeList->m_next;
        block->m_size = g_allocFreeList->m_size + size;
    }
    else
    {
        block->m_next = g_allocFreeList;
        block->m_size = size;
    }

    g_allocFreeList = block;
}


void operator delete( void *ptr, size_t size )
{
    ( void )size;

    operator delete( ptr );
}
