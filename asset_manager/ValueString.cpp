/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueString.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* ValueString::ValueString  0x00436050 */
ValueString::ValueString( Variable *variable ) : Value( variable )
{
    assert( variable->m_type->CastTypeString() );

    m_value = variable->m_type->CastTypeString()->m_default;
}


/* ValueString::ToScript  0x00436160 */
void ValueString::ToScript( int indent, buf_t *buf )
{
    std::string value;

    ( void )indent;

    value = EscapeString( m_value );

    AppendBuf( buf, "\"", 1 );
    AppendBuf( buf, value.c_str(), (int)value.size() );
    AppendBuf( buf, "\"\n", 2 );
}


/* ValueString::FromScript  0x00436260 */
bool ValueString::FromScript( char **text )
{
    m_value = COM_Parse( text );

    SetModified();

    return true;
}


/* ValueString::ToString  0x00436020 */
std::string ValueString::ToString( void )
{
    return m_value;
}
