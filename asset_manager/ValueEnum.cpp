/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueEnum.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* ValueEnum::SetValue  0x00434b60 */
void ValueEnum::SetValue( const std::string &name )
{
    TypeEnum *type;
    int       id;
    bool      success;

    type = m_parent->m_type->CastTypeEnum();

    if ( !type->FindValue( name, &id ) )
    {
        return;
    }

    success = type->GetValueName( id, &m_value );

    assert( success );
}


/* ValueEnum::ValueEnum  0x00434d40 */
ValueEnum::ValueEnum( Variable *variable ) : Value( variable )
{
    TypeEnum *type;
    bool      success;

    type = variable->m_type->CastTypeEnum();

    if ( type->m_maxId + 1 <= 0 )
    {
        m_value = "";
    }
    else
    {
        success = type->GetValueName( 0, &m_value );

        assert( success );
    }
}


/* ValueEnum::ToScript  0x00434e30 */
void ValueEnum::ToScript( int indent, buf_t *buf )
{
    std::string text;

    ( void )indent;

    text = "\"" + EscapeString( m_value ) + "\"\n";

    AppendBuf( buf, text.c_str(), (int)text.size() );
}


/* ValueEnum::FromScript  0x00434fa0 */
bool ValueEnum::FromScript( char **text )
{
    SetValue( std::string( COM_Parse( text ) ) );

    SetModified();

    return true;
}


/* ValueEnum::ToString  0x00434d10 */
std::string ValueEnum::ToString( void )
{
    return m_value;
}
