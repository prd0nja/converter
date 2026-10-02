/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeInt.cpp */

#include "parser.h"


/* TypeInt::TypeInt  0x0042e040 */
TypeInt::TypeInt( int defaultValue, int minValue, int maxValue )
{
    if ( defaultValue < minValue || defaultValue > maxValue )
    {
        throw "default value out of range";
    }

    m_max     = maxValue;
    m_default = defaultValue;
    m_min     = minValue;
}


/* TypeInt::CreateDefaultValue  0x0042e0c0 */
Value *TypeInt::CreateDefaultValue( Variable *variable )
{
    return new ValueInt( variable );
}


/* TypeInt::CastTypeInteger  0x0042e120 */
TypeInt *TypeInt::CastTypeInteger( void )
{
    return this;
}
