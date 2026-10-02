/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\TypeEnum.cpp */

#include "parser.h"
#include "assertive.h"


/* TypeEnum::CreateDefaultValue  0x0042da10 */
Value *TypeEnum::CreateDefaultValue( Variable *variable )
{
    return new ValueEnum( variable );
}


/* TypeEnum::CastTypeEnum  0x0042da70 */
TypeEnum *TypeEnum::CastTypeEnum( void )
{
    return this;
}


/* TypeEnum::FindValue  0x0042daa0 */
bool TypeEnum::FindValue( const std::string &name, int *id )
{
    unsigned int i;

    for ( i = 0; i < m_values.size(); i++ )
    {
        if ( m_values[i].m_name == name )
        {
            *id = m_values[i].m_id;
            return true;
        }
    }

    return false;
}


/* TypeEnum::TypeEnum  0x0042dcc0 */
TypeEnum::TypeEnum( const char **valueNames, const int *valueIds, int numValues )
{
    int i;

    assert( numValues >= 0 );
    assert( ( !valueNames ) == ( !numValues ) );

    m_maxId = -1;

    if ( !valueNames )
    {
        return;
    }

    m_values.resize( numValues );

    for ( i = 0; i < numValues; i++ )
    {
        m_values[i].m_name = valueNames[i];
        m_values[i].m_id   = valueIds ? valueIds[i] : i;

        if ( m_maxId < m_values[i].m_id )
        {
            m_maxId = m_values[i].m_id;
        }
    }
}


/* TypeEnum::GetValueName  0x0042dec0 */
bool TypeEnum::GetValueName( int id, std::string *name )
{
    unsigned int i;

    for ( i = 0; i < m_values.size(); i++ )
    {
        if ( m_values[i].m_id == id )
        {
            *name = m_values[i].m_name;
            return true;
        }
    }

    return false;
}
