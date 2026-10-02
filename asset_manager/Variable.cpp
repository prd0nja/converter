/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\Variable.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* Variable::Variable  0x00437300 */
Variable::Variable()
{
    m_type  = 0;
    m_value = 0;
}


/* Variable::~Variable  0x00437340 */
Variable::~Variable()
{
    if ( m_value )
    {
        delete m_value;
        m_value = 0;
    }
}


/* Variable::CreateValue  0x00436870 */
void Variable::CreateValue( void )
{
    assert( !m_value );

    m_value = m_type->CreateDefaultValue( this );
}


/* Variable::FromScript  0x004368d0 */
bool Variable::FromScript( char **text )
{
    return m_value->FromScript( text );
}


/* Variable::SetModified  0x00439320 */
void Variable::SetModified( void )
{
}


/* Variable::ToScript  0x00437ed0 */
void Variable::ToScript( int indent, buf_t *buf )
{
    std::string name;

    WriteIndent( buf, indent );
    AppendBuf( buf, "\"", 1 );
    name = EscapeString( m_name );
    AppendBuf( buf, name.c_str(), (int)name.size() );
    AppendBuf( buf, "\" ", 2 );

    m_value->ToScript( indent, buf );
}


/* ValueScope::ToScript  0x00437ff0 */
void ValueScope::ToScript( int indent, buf_t *buf )
{
    std::list< Variable * >::iterator it;

    for ( it = m_variables.begin(); it != m_variables.end(); ++it )
    {
        assert( ( *it )->m_name != "}" );

        ( *it )->ToScript( indent, buf );
    }
}


static bool SkipBraces( char **text );


/* SkipParens  0x00438100 */
static bool SkipParens( char **text )
{
    std::string token;

    for ( ;; )
    {
        token = COM_Parse( text );

        if ( !token.size() )
        {
            if ( !*text )
            {
                return false;
            }

            continue;
        }

        switch ( token[0] )
        {
        case '(':
            if ( !SkipParens( text ) )
            {
                return false;
            }
            break;

        case ')':
            return true;

        case '{':
            if ( !SkipBraces( text ) )
            {
                return false;
            }
            break;

        case '}':
            return false;
        }
    }
}


/* SkipBraces  0x004382e0 */
static bool SkipBraces( char **text )
{
    std::string token;

    for ( ;; )
    {
        token = COM_Parse( text );

        if ( !token.size() )
        {
            if ( !*text )
            {
                return false;
            }

            continue;
        }

        switch ( token[0] )
        {
        case '(':
            if ( !SkipParens( text ) )
            {
                return false;
            }
            break;

        case ')':
            return false;

        case '{':
            if ( !SkipBraces( text ) )
            {
                return false;
            }
            break;

        case '}':
            return true;
        }
    }
}


/* SkipValue  0x004384b0 */
static bool SkipValue( char **text )
{
    std::string token;

    token = COM_Parse( text );

    if ( !token.size() )
    {
        return *text != 0;
    }

    switch ( token[0] )
    {
    case '(':
        return SkipParens( text );

    case ')':
    case '}':
        return false;

    case '{':
        return SkipBraces( text );
    }

    return true;
}


/* ValueScope::FromScript  0x00438670 */
bool ValueScope::FromScript( char **text )
{
    std::string  token;
    Variable    *var;

    for ( ;; )
    {
        token = COM_Parse( text );

        if ( !*text )
        {
            return true;
        }

        if ( token == "}" )
        {
            COM_RestoreParseSession( text );
            return true;
        }

        var = Find( token );

        if ( var )
        {
            if ( !var->m_value->FromScript( text ) )
            {
                return false;
            }
        }
        else if ( !SkipValue( text ) )
        {
            return false;
        }
    }
}


/* ValueScope::ResetIndex  0x00438bb0 */
void ValueScope::ResetIndex( void )
{
    m_index.clear();

    m_indexed = false;
    m_rover   = m_variables.end();
}


/* ValueScope::~ValueScope  0x00438f90 */
ValueScope::~ValueScope()
{
    std::list< Variable * >::iterator it;

    for ( it = m_variables.begin(); it != m_variables.end(); ++it )
    {
        delete *it;
    }

    m_variables.clear();
    m_index.clear();
}


/* ValueScope::ValueScope  0x004393e0 */
ValueScope::ValueScope()
{
    m_indexed = false;
    m_rover   = m_variables.end();
}


/* ValueScope::BuildIndex  0x00439780 */
void ValueScope::BuildIndex( void )
{
    std::list< Variable * >::reverse_iterator it;

    if ( m_indexed )
    {
        return;
    }

    m_indexed = true;

    assert( !m_index.size() );

    for ( it = m_variables.rbegin(); it != m_variables.rend(); ++it )
    {
        m_index[( *it )->m_name] = *it;
    }
}


/* ValueScope::AddVariable  0x00439800 */
Variable *ValueScope::AddVariable( const std::string &name, Type *type )
{
    Variable *var;

    var = type->CreateVariable();

    if ( m_indexed )
    {
        m_index[name] = var;
    }

    var->m_name = name;

    m_variables.push_back( var );

    var->m_type = type;

    return var;
}


/* ValueScope::Find  0x00439b40 */
Variable *ValueScope::Find( const std::string &name )
{
    std::map< std::string, Variable * >::iterator it;

    if ( !m_indexed )
    {
        for ( ;; )
        {
            if ( m_rover == m_variables.end() )
            {
                m_rover = m_variables.begin();
            }
            else
            {
                ++m_rover;
            }

            if ( m_rover == m_variables.end() )
            {
                BuildIndex();
                break;
            }

            if ( ( *m_rover )->m_name == name )
            {
                return *m_rover;
            }
        }
    }

    assert( m_indexed );

    it = m_index.find( name );

    if ( it == m_index.end() )
    {
        return 0;
    }

    return it->second;
}


/* ValueScope::Finalize  0x004369e0 */
void ValueScope::Finalize( void )
{
    std::list< Variable * >::iterator it;

    for ( it = m_variables.begin(); it != m_variables.end(); ++it )
    {
        if ( ( *it )->m_value )
        {
            ( *it )->m_value->Finalize();
        }
    }
}
