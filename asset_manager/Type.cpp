/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\Type.cpp */

#include "parser.h"


/* Type::~Type  0x0042d630 */
Type::~Type()
{
}


/* Type::CreateVariable  0x0042d640 */
Variable *Type::CreateVariable( void )
{
    Variable *var;

    var = new Variable;

    var->m_type = this;
    var->CreateValue();

    return var;
}


/* Type::CastTypeColor  0x0042d6b0 */
TypeColor *Type::CastTypeColor( void )
{
    return 0;
}


/* Type::CastTypeData  0x0042d6c0 */
TypeData *Type::CastTypeData( void )
{
    return 0;
}


/* Type::CastTypeEnum  0x0042d6d0 */
TypeEnum *Type::CastTypeEnum( void )
{
    return 0;
}


/* Type::CastTypeFloat  0x0042d6e0 */
TypeFloat *Type::CastTypeFloat( void )
{
    return 0;
}


/* Type::CastTypeInteger  0x0042d6f0 */
TypeInt *Type::CastTypeInteger( void )
{
    return 0;
}


/* Type::CastTypeMap  0x0042d700 */
TypeMap *Type::CastTypeMap( void )
{
    return 0;
}


/* Type::CastTypeString  0x0042d710 */
TypeString *Type::CastTypeString( void )
{
    return 0;
}


/* Type::CastTypeStruct  0x0042d720 */
TypeStruct *Type::CastTypeStruct( void )
{
    return 0;
}
