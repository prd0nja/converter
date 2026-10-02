/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueMap.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* ValueMap::ValueMap  0x004355f0 */
ValueMap::ValueMap( Variable *variable, Type *valueType ) : Value( variable )
{
    m_valueType = valueType;
}


/* CompareEntryNames  0x00435650 */
static int CompareEntryNames( const void *a, const void *b )
{
    return _stricmp( *(const char **)a, *(const char **)b );
}


/* ValueMap::Finalize  0x00435680 */
void ValueMap::Finalize( void )
{
    m_scope.Finalize();
}


/* ValueMap::AddEntry  0x004357d0 */
Variable *ValueMap::AddEntry( const std::string &name )
{
    return m_scope.AddVariable( name, m_valueType );
}


/* ValueMap::ToScript  0x00435850 */
void ValueMap::ToScript( int indent, buf_t *buf )
{
    std::list< Variable * >::iterator             it;
    std::map< std::string, Variable * >::iterator entry;
    const char                                  **names;
    int                                           count;
    int                                           i;

    AppendBuf( buf, "\n", 1 );
    WriteIndent( buf, indent );
    AppendBuf( buf, "{\n", 2 );

    count = (int)m_scope.m_variables.size();

    if ( count )
    {
        names = (const char **)malloc( count * sizeof( *names ) );

        i = 0;

        for ( it = m_scope.m_variables.begin(); it != m_scope.m_variables.end(); ++it )
        {
            names[i++] = ( *it )->m_name.c_str();
        }

        assert( i == count );

        qsort( names, count, sizeof( *names ), CompareEntryNames );

        m_scope.BuildIndex();

        for ( i = 0; i < count; i++ )
        {
            entry = m_scope.m_index.find( std::string( names[i] ) );

            assert( entry != m_scope.m_index.end() );

            entry->second->ToScript( indent + 1, buf );
        }

        free( names );
    }

    WriteIndent( buf, indent );
    AppendBuf( buf, "}\n", 2 );
}


/* ValueMap::FromScript  0x00435b70 */
bool ValueMap::FromScript( char **text )
{
    std::string  token;
    Variable    *var;

    token = COM_Parse( text );

    if ( token != "{" )
    {
        return false;
    }

    for ( ;; )
    {
        token = COM_Parse( text );

        if ( !*text )
        {
            break;
        }

        if ( token == "}" )
        {
            COM_RestoreParseSession( text );
            break;
        }

        if ( m_scope.Find( token ) )
        {
            printf( "duplicate entry '%s'\n", token.c_str() );
            return false;
        }

        var = AddEntry( token );

        if ( !var || !var->FromScript( text ) )
        {
            return false;
        }
    }

    token = COM_Parse( text );

    if ( token != "}" )
    {
        return false;
    }

    SetModified();

    return true;
}
