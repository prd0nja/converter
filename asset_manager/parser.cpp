/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\parser.cpp */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>

#include "parser.h"
#include "utils.h"
#include "q_import.h"
#include "assertive.h"

DefFileMap_t g_defFiles;                    /* 0x004f5160 */

extern bool g_pause;

extern int g_platform;                      /* 0x004f515c */

extern std::string g_platformNames[];       /* 0x00486238 */


static Type *EmitType( parseVal_t val );


/* Grammar_Error  0x004227e0 */
void Grammar_Error( int pos, const char *fmt, ... )
{
    char    buf[1024];
    va_list args;

    ( void )pos;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    printf( "ERROR: %s\n", buf );

    if ( g_pause )
    {
        printf( "Conversion aborted.  Hit key to continue.\n" );
        fflush( stdout );
        _getch();
    }
    else
    {
        fflush( stdout );
    }

    exit( 0 );
}


/* GetName  0x00422860 */
static char *GetName( parseVal_t val )
{
    assert( val.node[0].type == ENUM_simple_variable );

    return val.node[1].string;
}


/* EmitEnum  0x00422b40 */
static Type *EmitEnum( parseVal_t val )
{
    parseListItem_t  *group;
    parseListItem_t  *item;
    const char      **names;
    int              *ids;
    int               count;
    int               id;

    count = 0;

    for ( group = val.list->head->next; group; group = group->next )
    {
        for ( item = group->value.list->head->next; item; item = item->next )
        {
            count++;
        }
    }

    names = 0;
    ids   = 0;

    if ( count )
    {
        names = (const char **)_alloca( count * sizeof( *names ) );
        ids   = (int *)_alloca( count * sizeof( *ids ) );
    }

    count = 0;
    id    = 0;

    for ( group = val.list->head->next; group; group = group->next )
    {
        for ( item = group->value.list->head->next; item; item = item->next )
        {
            names[count] = item->value.string;
            ids[count]   = id;
            count++;
        }

        id++;
    }

    return new TypeEnum( names, ids, count );
}


/* GetFloat  0x00422e70 */
static float GetFloat( parseVal_t val )
{
    if ( val.node[0].type == ENUM_float_value )
    {
        return val.node[1].number;
    }

    if ( val.node[0].type == ENUM_int_value )
    {
        return (float)val.node[1].integer;
    }

    assert( !"unknown type in GetFloat" );

    return 0.0f;
}


/* AddVariable  0x00424820 */
static Variable *AddVariable( parseVal_t nameVal, const char *name, Type *type, ValueScope *scope )
{
    ( void )nameVal;

    try
    {
        return scope->AddVariable( std::string( name ), type );
    }
    catch ( const char *error )
    {
        Grammar_Error( 0, "%s\nwhile processing '%s'\n", error, name );
    }

    return 0;
}


/* GetParseErrorLine  0x004248c0 */
void GetParseErrorLine( const char *text, const char *pos, int *line,
                        std::string *lineText, std::string *marker )
{
    const char *p;
    const char *lineStart;
    const char *end;
    char       *buf;
    int         column;
    int         len;

    *line     = 1;
    column    = 0;
    lineStart = text;

    for ( p = text; ( p < pos || ( *p && *p <= ' ' ) ) && *p; p++ )
    {
        if ( *p == '\n' )
        {
            lineStart = p + 1;
            ( *line )++;
            column = 0;
        }
        else
        {
            column++;
        }
    }

    for ( end = lineStart; *end && *end != '\n'; end++ )
    {
    }

    len = (int)( end - lineStart );
    buf = (char *)_alloca( len + 2 );

    memcpy( buf, lineStart, len );
    buf[len] = 0;

    *lineText = buf;

    assert( column <= len );

    buf[column]     = '^';
    buf[column + 1] = 0;

    *marker = buf;
}


/* EmitStructMembers  0x00426660 */
static void EmitStructMembers( TypeStruct *type, parseVal_t val )
{
    parseListItem_t *item;
    parseVal_t       member;

    for ( item = val.list->head->next; item; item = item->next )
    {
        member = item->value;

        type->m_members[std::string( GetName( member.node[2] ) )] = EmitType( member.node[1] );
    }
}


/* EmitStruct  0x00426760 */
static Type *EmitStruct( parseVal_t val )
{
    TypeStruct *type;

    type = new TypeStruct;

    EmitStructMembers( type, val );

    return type;
}


/* EmitType  0x004267c0 */
static Type *EmitType( parseVal_t val )
{
    switch ( val.node[0].type )
    {
    case ENUM_enum:
        return EmitEnum( val.node[1] );

    case ENUM_int:
        return new TypeInt( val.node[1].integer, val.node[2].integer, val.node[3].integer );

    case ENUM_float:
        return new TypeFloat( GetFloat( val.node[1] ), GetFloat( val.node[2] ), GetFloat( val.node[3] ) );

    case ENUM_color:
        return new TypeColor( GetFloat( val.node[1] ), GetFloat( val.node[2] ), GetFloat( val.node[3] ),
                              GetFloat( val.node[4] ) );

    case ENUM_struct:
        return EmitStruct( val.node[1] );

    case ENUM_map:
        return new TypeMap( EmitType( val.node[1] ) );

    case ENUM_string:
    case ENUM_keyenum:
        return new TypeString( std::string( "" ) );

    case ENUM_string_default:
        return new TypeString( std::string( val.node[1].string ) );

    case ENUM_data:
        return new TypeData;
    }

    assert( !"unknown type in EmitType" );

    return 0;
}


/* EmitVariable  0x00426930 */
static Variable *EmitVariable( parseVal_t val, ValueScope *scope )
{
    assert( scope );

    switch ( val.node[0].type )
    {
    case ENUM_variable_exec:
        return EmitVariable( val.node[1], scope );

    case ENUM_variable:
        if ( !val.node[5].typeDef )
        {
            val.node[4].string  = GetName( val.node[2] );
            val.node[5].typeDef = EmitType( val.node[1] );
        }

        return AddVariable( val.node[2], val.node[4].string, val.node[5].typeDef, scope );

    case ENUM_variable_param:
        if ( !val.node[6].typeDef )
        {
            val.node[5].string  = GetName( val.node[2] );
            val.node[6].typeDef = EmitType( val.node[1] );
        }

        return AddVariable( val.node[2], val.node[5].string, val.node[6].typeDef, scope );
    }

    return 0;
}


/* EmitVariables  0x00426a80 */
static void EmitVariables( parseVal_t val, ValueScope *scope, std::vector< Variable * > *vars )
{
    parseListItem_t *item;

    assert( vars );

    for ( item = val.list->head->next; item; item = item->next )
    {
        vars->push_back( EmitVariable( item->value, scope ) );
    }
}


/* GetParseData  0x00426b60 */
parseVal_t *GetParseData( const std::string &fileName )
{
    static std::map< std::string, bool >       s_parsed;      /* 0x004b4a1c */
    static std::map< std::string, parseVal_t > s_parseData;   /* 0x004b4a10 */
    std::map< std::string, bool >::iterator    it;
    std::string                                path;
    FILE                                      *f;
    int                                        len;
    char                                      *buf;
    parseVal_t                                 root;

    it = s_parsed.find( fileName );

    if ( it != s_parsed.end() )
    {
        return it->second ? &s_parseData[fileName] : 0;
    }

    path = g_baseDir + "deffiles" + "/" + fileName;

    f = fopen( path.c_str(), "rb" );

    if ( !f )
    {
        s_parsed[fileName] = false;
        return 0;
    }

    fseek( f, 0, SEEK_END );
    len = ftell( f );

    if ( len <= 0 )
    {
        s_parsed[fileName] = false;
        return 0;
    }

    fseek( f, 0, SEEK_SET );

    buf = (char *)malloc( len + 2 );

    buf[len]     = '\n';
    buf[len + 1] = 0;

    fread( buf, 1, len, f );
    fclose( f );

    if ( !Grammar_ParseText( buf, &root ) )
    {
        s_parsed[fileName] = false;
        return 0;
    }

    s_parsed[fileName]    = true;
    s_parseData[fileName] = root;

    return &s_parseData[fileName];
}


/* EmitDef  0x00426e20 */
bool EmitDef( ValueData *data )
{
    parseVal_t                *parseData;
    std::vector< Variable * >  vars;

    Prof_Begin( PROF_GET_PARSE_DATA );
    parseData = GetParseData( data->m_fileName );
    Prof_End();

    if ( !parseData )
    {
        return false;
    }

    assert( parseData->node[0].type == ENUM_main );

    Prof_Begin( PROF_EMIT_VARIABLES );
    EmitVariables( parseData->node[1], data->m_scope, &vars );
    Prof_End();

    Prof_Begin( PROF_EMIT_INFO_LIST );
    data->m_scope->Finalize();
    Prof_End();

    return true;
}


/* Grammar_Int  0x00426f60 */
parseVal_t Grammar_Int( int value )
{
    parseVal_t val;

    val.integer = value;

    return val;
}


/* Grammar_Node1  0x00426f80 */
parseVal_t Grammar_Node1( int type )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 1 * sizeof( parseVal_t ) );

    val.node[0].type = type;

    return val;
}


/* Grammar_Node2  0x00426fa0 */
parseVal_t Grammar_Node2( int type, parseVal_t a )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 2 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;

    return val;
}


/* Grammar_Node3  0x00426fc0 */
parseVal_t Grammar_Node3( int type, parseVal_t a, parseVal_t b )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 3 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;

    return val;
}


/* Grammar_Node4  0x00426fe0 */
parseVal_t Grammar_Node4( int type, parseVal_t a, parseVal_t b, parseVal_t c )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 4 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;
    val.node[3]      = c;

    return val;
}


/* Grammar_Node5  0x00427030 */
parseVal_t Grammar_Node5( int type, parseVal_t a, parseVal_t b, parseVal_t c, parseVal_t d )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 5 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;
    val.node[3]      = c;
    val.node[4]      = d;

    return val;
}


/* Grammar_Node6  0x00427060 */
parseVal_t Grammar_Node6( int type, parseVal_t a, parseVal_t b, parseVal_t c, parseVal_t d,
                                 parseVal_t e )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 6 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;
    val.node[3]      = c;
    val.node[4]      = d;
    val.node[5]      = e;

    return val;
}


/* Grammar_Node7  0x004270a0 */
parseVal_t Grammar_Node7( int type, parseVal_t a, parseVal_t b, parseVal_t c, parseVal_t d,
                                 parseVal_t e, parseVal_t f )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 7 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;
    val.node[3]      = c;
    val.node[4]      = d;
    val.node[5]      = e;
    val.node[6]      = f;

    return val;
}


/* Grammar_Node8  0x004270e0 */
parseVal_t Grammar_Node8( int type, parseVal_t a, parseVal_t b, parseVal_t c, parseVal_t d,
                                 parseVal_t e, parseVal_t f, parseVal_t g )
{
    parseVal_t val;

    val.node = (parseVal_t *)malloc( 8 * sizeof( parseVal_t ) );

    val.node[0].type = type;
    val.node[1]      = a;
    val.node[2]      = b;
    val.node[3]      = c;
    val.node[4]      = d;
    val.node[5]      = e;
    val.node[6]      = f;
    val.node[7]      = g;

    return val;
}


/* Grammar_ListCreate  0x00427130 */
parseVal_t Grammar_ListCreate( parseVal_t value )
{
    parseListItem_t *item;
    parseVal_t       val;

    item = (parseListItem_t *)malloc( sizeof( *item ) );

    item->value = value;
    item->next  = 0;

    val.list = (parseList_t *)malloc( sizeof( *val.list ) );

    val.list->head = item;
    val.list->tail = item;

    return val;
}


/* Grammar_ListAppend  0x00427180 */
parseVal_t Grammar_ListAppend( parseVal_t list, parseVal_t value )
{
    parseListItem_t *item;

    item = (parseListItem_t *)malloc( sizeof( *item ) );

    item->value = value;
    item->next  = 0;

    list.list->tail->next = item;
    list.list->tail       = item;

    return list;
}


/* ObjectInfo::ObjectInfo  0x004324f0 */
ObjectInfo::ObjectInfo()
{
    m_HasFileExt = false;
    m_Version    = 0;
}


/* GetDefFile  0x00432bf0 */
void GetDefFile( std::string fileName )
{
    std::string                path;
    std::string                token;
    std::string                name;
    std::string                platform;
    std::string                dll;
    std::string                sourceDir;
    std::string                fileExt;
    std::vector< std::string > dlls;
    std::vector< bool >        dllIsAll;
    std::vector< std::string > targetDirs;
    bool                       hasFileExt;
    int                        version;
    FILE                      *f;
    int                        len;
    char                      *buf;
    char                      *text;
    ObjectInfo                *info;

    path = g_baseDir + "deffiles" + "/" + fileName;

    f = fopen( path.c_str(), "rb" );

    if ( !f )
    {
        return;
    }

    fseek( f, 0, SEEK_END );
    len = ftell( f );

    if ( len <= 0 )
    {
        fclose( f );
        return;
    }

    fseek( f, 0, SEEK_SET );

    buf = (char *)malloc( len + 1 );

    if ( (int)fread( buf, 1, len, f ) != len )
    {
        printf( "ERROR: Short read in ParseDefFiles\n" );
        fclose( f );
        free( buf );
        return;
    }

    fclose( f );

    buf[len] = 0;

    hasFileExt = false;
    version    = 0;
    text       = buf;

    for ( ;; )
    {
        token = COM_Parse( &text );

        if ( token == "#name" )
        {
            name = COM_Parse( &text );
            continue;
        }

        if ( token == "#dll" )
        {
            platform = COM_Parse( &text );
            dll      = COM_Parse( &text );

            if ( g_platform )
            {
                if ( platform == g_platformNames[g_platform] )
                {
                    dlls.push_back( dll );
                    dllIsAll.push_back( false );
                }
                else if ( platform == "all" )
                {
                    dlls.push_back( dll );
                    dllIsAll.push_back( true );
                }
            }

            continue;
        }

        if ( token == "#source_dir" )
        {
            sourceDir = COM_Parse( &text );
            continue;
        }

        if ( token == "#target_dir" )
        {
            targetDirs.push_back( std::string( COM_Parse( &text ) ) );
            continue;
        }

        if ( token == "#file_ext" )
        {
            fileExt    = COM_Parse( &text );
            hasFileExt = true;
            continue;
        }

        if ( token == "#version" )
        {
            version = Legacy_atol( COM_Parse( &text ) );
            continue;
        }

        break;
    }

    COM_RestoreParseSession( &text );

    free( buf );

    if ( !name.size() || !targetDirs.size() )
    {
        return;
    }

    info = new ObjectInfo;

    g_defFiles[fileName] = info;

    info->m_FileName        = fileName;
    info->m_Name            = name;
    info->m_Dll             = dlls;
    info->m_DllIsAll        = dllIsAll;
    info->m_SourceDirectory = sourceDir;
    info->m_TargetDirectory = targetDirs;
    info->m_FileExt         = fileExt;
    info->m_HasFileExt      = hasFileExt;
    info->m_Version         = version;
}
