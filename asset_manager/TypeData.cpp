/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeData.cpp */

#include "parser.h"


/* TypeData::CreateDefaultValue  0x0042d970 */
Value *TypeData::CreateDefaultValue( Variable *variable )
{
    return new ValueData( variable );
}


/* TypeData::CastTypeData  0x0042d9d0 */
TypeData *TypeData::CastTypeData( void )
{
    return this;
}
