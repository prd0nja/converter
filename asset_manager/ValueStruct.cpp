/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueStruct.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* ValueStruct::ToScript  0x00436300 */
void ValueStruct::ToScript( int indent, buf_t *buf )
{
    AppendBuf( buf, "\n", 1 );
    WriteIndent( buf, indent );
    AppendBuf( buf, "{\n", 2 );
    m_scope.ToScript( indent + 1, buf );
    WriteIndent( buf, indent );
    AppendBuf( buf, "}\n", 2 );
}


/* ValueStruct::Finalize  0x00436390 */
void ValueStruct::Finalize( void )
{
    m_scope.Finalize();
}


/* ValueStruct::ValueStruct  0x00436410 */
ValueStruct::ValueStruct( Variable *variable ) : Value( variable )
{
    std::map< std::string, Type * >::iterator it;
    TypeStruct                               *type;

    type = variable->m_type->CastTypeStruct();

    for ( it = type->m_members.begin(); it != type->m_members.end(); ++it )
    {
        assert( it->first.length() );

        m_scope.AddVariable( it->first, it->second );
    }
}


/* ValueStruct::FromScript  0x004365b0 */
bool ValueStruct::FromScript( char **text )
{
    std::string token;

    token = COM_Parse( text );

    if ( token != "{" )
    {
        return false;
    }

    if ( !m_scope.FromScript( text ) )
    {
        return false;
    }

    token = COM_Parse( text );

    if ( token != "}" )
    {
        return false;
    }

    SetModified();

    return true;
}
