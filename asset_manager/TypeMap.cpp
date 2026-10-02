/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeMap.cpp */

#include "parser.h"
#include "assertive.h"


/* TypeMap::TypeMap  0x0042e160 */
TypeMap::TypeMap()
{
    m_type = 0;
}


/* TypeMap::TypeMap  0x0042e180 */
TypeMap::TypeMap( Type *valueType )
{
    m_type = valueType;
}


/* TypeMap::CreateDefaultValue  0x0042e1a0 */
Value *TypeMap::CreateDefaultValue( Variable *variable )
{
    assert( m_type );

    return new ValueMap( variable, m_type );
}


/* TypeMap::CastTypeMap  0x0042e250 */
TypeMap *TypeMap::CastTypeMap( void )
{
    return this;
}
