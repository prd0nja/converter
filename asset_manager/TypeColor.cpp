/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeColor.cpp */

#include "parser.h"


/* TypeColor::TypeColor  0x0042d750 */
TypeColor::TypeColor( float red, float green, float blue, float alpha )
{
    if ( red < 0.0f || red > 1.0f )
    {
        throw "red value must be in the range 0 to 1";
    }

    if ( green < 0.0f || green > 1.0f )
    {
        throw "green value must be in the range 0 to 1";
    }

    if ( blue < 0.0f || blue > 1.0f )
    {
        throw "blue value must be in the range 0 to 1";
    }

    if ( alpha < 0.0f || alpha > 1.0f )
    {
        throw "alpha value must be in the range 0 to 1";
    }

    m_default[0] = red;
    m_default[1] = green;
    m_default[2] = blue;
    m_default[3] = alpha;
}


/* TypeColor::CreateDefaultValue  0x0042d8b0 */
Value *TypeColor::CreateDefaultValue( Variable *variable )
{
    return new ValueColor( variable );
}


/* TypeColor::CastTypeColor  0x0042d910 */
TypeColor *TypeColor::CastTypeColor( void )
{
    return this;
}
