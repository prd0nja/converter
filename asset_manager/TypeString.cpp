/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeString.cpp */

#include "parser.h"


/* TypeString::CreateDefaultValue  0x0042e5f0 */
Value *TypeString::CreateDefaultValue( Variable *variable )
{
    return new ValueString( variable );
}


/* TypeString::CastTypeString  0x0042e650 */
TypeString *TypeString::CastTypeString( void )
{
    return this;
}


/* TypeString::TypeString  0x0042e660 */
TypeString::TypeString( std::string defaultValue )
{
    m_default = defaultValue;
}
