/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueFloat.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"


/* ValueFloat::ValueFloat  0x00435070 */
ValueFloat::ValueFloat( Variable *variable ) : Value( variable )
{
    m_value = variable->m_type->CastTypeFloat()->m_default;
}


/* ValueFloat::ToScript  0x00435130 */
void ValueFloat::ToScript( int indent, buf_t *buf )
{
    TypeFloat *type;
    char       text[1024];

    ( void )indent;

    type = m_parent->m_type->CastTypeFloat();

    if ( type->m_min > m_value )
    {
        m_value = type->m_min;
        SetModified();
    }
    else if ( type->m_max < m_value )
    {
        m_value = type->m_max;
        SetModified();
    }

    Legacy_sprintf( text, "\"%g\"\n", m_value );

    AppendBuf( buf, text, (int)strlen( text ) );
}


/* ValueFloat::FromScript  0x00435200 */
bool ValueFloat::FromScript( char **text )
{
    m_value = Legacy_atof( COM_Parse( text ) );

    SetModified();

    return true;
}


/* ValueFloat::ToString  0x004352a0 */
std::string ValueFloat::ToString( void )
{
    char buf[1024];

    Legacy_sprintf( buf, "%g", m_value );

    return std::string( buf );
}
