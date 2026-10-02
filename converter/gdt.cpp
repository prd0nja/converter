/* Original: .\converter.cpp */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "converter.h"
#include "q_import.h"
#include "assertive.h"
#include "utils.h"
#include "parser.h"

gdtFileMap_t  g_convertFileNames;               /* 0x20505ef4 */

gdtEntryMap_t g_gdtEntries;                     /* 0x20505f00 */
gdtEntryMap_t g_cachedEntries[ASSET_DIR_COUNT]; /* 0x20506064, 0x20506058 */
gdtEntryMap_t g_changeListEntries;              /* 0x205060a0 */


/* AddGdtEntry  0x0040c2d0 */
static void AddGdtEntry( Variable *variable, const std::string &fileName,
                         gdtEntryMap_t *entries )
{
    gdtEntryMap_t::iterator it;
    ValueScope             *scope;
    Variable               *existing;
    Variable               *entry;
    std::string             name;

    assert( variable->m_type->CastTypeData() );

    name = ( (ValueData *)variable->m_value )->m_fileName;

    it = entries->find( name );

    if ( it == entries->end() )
    {
        scope = new ValueScope;
        ( *entries )[name] = scope;
    }
    else
    {
        scope = it->second;
    }

    existing = scope->Find( variable->m_name );

    if ( existing )
    {
        Error( va( "'%s' in '%s' and '%s' is a duplicate\n",
                   variable->m_name.c_str(),
                   fileName.c_str(),
                   ( (ValueData *)existing->m_value )->m_sourceFileName.c_str() ) );
        return;
    }

    entry = scope->AddVariable( variable->m_name, variable->m_type );

    delete entry->m_value;

    entry->m_value           = variable->m_value;
    entry->m_value->m_parent = entry;

    ( (ValueData *)entry->m_value )->m_sourceFileName = fileName;

    variable->m_value = 0;
}


/* AddGdtEntries  0x0040c520 */
static void AddGdtEntries( ValueScope *scope, const std::string &fileName,
                           gdtEntryMap_t *entries )
{
    std::list< Variable * >::iterator it;

    for ( it = scope->m_variables.begin(); it != scope->m_variables.end(); ++it )
    {
        AddGdtEntry( *it, fileName, entries );
    }
}


/* AddAssetsFromGdtToConvertList  0x0040c550 */
static void AddAssetsFromGdtToConvertList( gdtEntryMap_t *entries, std::string fileName,
                                           gdtBuf_t *pBuf )
{
    FILE        *f;
    int          len;
    char        *buf;
    char        *text;
    TypeData    *dataType;
    TypeMap     *mapType;
    Variable    *var;
    std::string  relName;
    char         cbuf[BUFFER_LEN];

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
        Error( va( "Short read in AddAssetsFromGdtToConvertList of '%s'\n", fileName.c_str() ) );

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

    dataType = new TypeData;
    mapType  = new TypeMap;

    mapType->m_type = dataType;

    var = mapType->CreateVariable();

    text = buf;

    if ( !var->FromScript( &text ) )
    {
        std::string lineText;
        std::string marker;
        int         line;

        COM_RestoreParseSession( &text );

        GetParseErrorLine( buf, text, &line, &lineText, &marker );

        Error( va( "Error in parsing '%s' on line %d:\n%s\n%s\n\n", fileName.c_str(), line,
                   lineText.c_str(), marker.c_str() ) );
        free( buf );
        delete var;
        return;
    }

    if ( !pBuf )
    {
        free( buf );
    }
    else
    {
        assert( !pBuf->buf );
        pBuf->buf = buf;
        pBuf->len = len;
    }

    relName = MakeRelativePath( g_baseDir, fileName );

    ConvertPath( cbuf, relName.c_str(), sizeof( cbuf ) );

    AddGdtEntries( &( (ValueMap *)var->m_value )->m_scope, std::string( cbuf ), entries );

    delete var;
}


/* AddGdtFile  0x0040c9a0 */
void AddGdtFile( std::string relFileName, bool useChangeList )
{
    char                  buf[BUFFER_LEN];
    gdtFileMap_t::iterator it;

    ConvertPath( buf, relFileName.c_str(), sizeof( buf ) );

    std::string name( buf );

    if ( g_convertFileNames.find( name ) != g_convertFileNames.end() )
    {
        return;
    }

    gdtBuf_t empty;
    empty.buf = 0;
    empty.len = 0;

    g_convertFileNames[name] = empty;

    if ( g_spam )
    {
        printf( "Adding '%s'...\n", name.c_str() );
    }

    assert( !g_preprocess );
    g_preprocess = true;

    it = g_convertFileNames.find( name );

    assert( it != g_convertFileNames.end() );

    AddAssetsFromGdtToConvertList( &g_gdtEntries, g_baseDir + name, &it->second );

    assert( !g_cacheEntry );
    g_cacheEntry = true;

    AddAssetsFromGdtToConvertList( &g_cachedEntries[ASSET_DIR_DEFAULT],
                                   g_srcCacheDir[ASSET_DIR_DEFAULT] + name, 0 );
    AddAssetsFromGdtToConvertList( &g_cachedEntries[ASSET_DIR_SHARED],
                                   g_srcCacheDir[ASSET_DIR_SHARED] + name, 0 );

    if ( useChangeList )
    {
        AddAssetsFromGdtToConvertList( &g_changeListEntries, g_assetChangeListDir + name, 0 );
    }

    assert( g_cacheEntry );
    g_cacheEntry = false;

    assert( g_preprocess );
    g_preprocess = false;
}


/* ScanGdtDir  0x004249d0 */
static void ScanGdtDir( std::string dir, void ( *callback )( const std::string &fileName ) )
{
    char *list[MAX_FOUND_FILES];
    int   numFiles;
    int   i;

    numFiles = 0;

    Sys_ListFilteredFiles_r( ( g_baseDir + dir ).c_str(), "", "*.gdt",
                             list, &numFiles, MAX_FOUND_FILES );

    for ( i = 0; i < numFiles; i++ )
    {
        callback( dir + "\\" + list[i] );
        free( list[i] );
    }
}


/* AddGdtFileCallback  0x0040ce60 */
void AddGdtFileCallback( const std::string &fileName )
{
    AddGdtFile( fileName, false );
}


/* ForEachGdtDir  0x00424b60 */
void ForEachGdtDir( void ( *callback )( const std::string &fileName ) )
{
    FILE *f;
    char  line[1024];
    char *dir;

    f = fopen( ( g_baseDir + "bin\\converter_gdt_dirs_0.txt" ).c_str(), "r" );

    if ( !f )
    {
        return;
    }

    while ( ( dir = ReadLine( f, line, sizeof( line ) ) ) != NULL )
    {
        ScanGdtDir( std::string( dir ), callback );
    }

    fclose( f );
}
