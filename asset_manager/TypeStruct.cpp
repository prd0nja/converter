/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeStruct.cpp */

#include "parser.h"


/* TypeStruct::CreateDefaultValue  0x0042e730 */
Value *TypeStruct::CreateDefaultValue( Variable *variable )
{
    return new ValueStruct( variable );
}


/* TypeStruct::CastTypeStruct  0x0042e790 */
TypeStruct *TypeStruct::CastTypeStruct( void )
{
    return this;
}
