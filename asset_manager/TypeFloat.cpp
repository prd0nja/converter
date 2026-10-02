/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeFloat.cpp */

#include "parser.h"


/* TypeFloat::TypeFloat  0x0042df10 */
TypeFloat::TypeFloat( float defaultValue, float minValue, float maxValue )
{
    if ( defaultValue < minValue || defaultValue > maxValue )
    {
        throw "default value out of range";
    }

    m_min     = minValue;
    m_default = defaultValue;
    m_max     = maxValue;
}


/* TypeFloat::CreateDefaultValue  0x0042dfa0 */
Value *TypeFloat::CreateDefaultValue( Variable *variable )
{
    return new ValueFloat( variable );
}


/* TypeFloat::CastTypeFloat  0x0042e000 */
TypeFloat *TypeFloat::CastTypeFloat( void )
{
    return this;
}
