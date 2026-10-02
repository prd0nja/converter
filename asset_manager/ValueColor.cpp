/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueColor.cpp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"


/* ValueColor::ValueColor  0x00433de0 */
ValueColor::ValueColor( Variable *variable ) : Value( variable )
{
    m_value[0] = variable->m_type->CastTypeColor()->m_default[0];
    m_value[1] = variable->m_type->CastTypeColor()->m_default[1];
    m_value[2] = variable->m_type->CastTypeColor()->m_default[2];
    m_value[3] = variable->m_type->CastTypeColor()->m_default[3];
}


/* ValueColor::ToScript  0x00433eb0 */
void ValueColor::ToScript( int indent, buf_t *buf )
{
    char text[2048];

    ( void )indent;

    Legacy_sprintf( text, "\"%f %f %f %f\"\n", m_value[0], m_value[1], m_value[2], m_value[3] );

    AppendBuf( buf, text, (int)strlen( text ) );
}


/* ValueColor::FromScript  0x00434080 */
bool ValueColor::FromScript( char **text )
{
    std::string token;

    token = COM_Parse( text );

    Legacy_sscanf( token.c_str(), "%f %f %f %f", &m_value[0], &m_value[1], &m_value[2], &m_value[3] );

    SetModified();

    return true;
}


/* ValueColor::ToString  0x00434160 */
std::string ValueColor::ToString( void )
{
    char buf[2048];

    Legacy_sprintf( buf, "%f %f %f %f", m_value[0], m_value[1], m_value[2], m_value[3] );

    return std::string( buf );
}
