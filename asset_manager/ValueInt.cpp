/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueInt.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"


/* ValueInt::ValueInt  0x00435350 */
ValueInt::ValueInt( Variable *variable ) : Value( variable )
{
    m_value = variable->m_type->CastTypeInteger()->m_default;
}


/* ValueInt::ToScript  0x00435410 */
void ValueInt::ToScript( int indent, buf_t *buf )
{
    TypeInt *type;
    char     text[128];

    ( void )indent;

    type = m_parent->m_type->CastTypeInteger();

    if ( m_value < type->m_min )
    {
        m_value = type->m_min;
        SetModified();
    }
    else if ( m_value > type->m_max )
    {
        m_value = type->m_max;
        SetModified();
    }

    sprintf( text, "\"%d\"\n", m_value );

    AppendBuf( buf, text, (int)strlen( text ) );
}


/* ValueInt::FromScript  0x004354d0 */
bool ValueInt::FromScript( char **text )
{
    m_value = Legacy_atol( COM_Parse( text ) );

    SetModified();

    return true;
}


/* ValueInt::ToString  0x00435570 */
std::string ValueInt::ToString( void )
{
    char buf[128];

    sprintf( buf, "%d", m_value );

    return std::string( buf );
}
