/* Original: .\converter.cpp */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "converter.h"
#include "q_import.h"
#include "assertive.h"
#include "utils.h"
#include "parser.h"

static ConvertData *g_freeConvertData;                  /* 0x204f599c */


/* ConvertData::ConvertData  0x00401a40 */
ConvertData::ConvertData()
{
    m_version = CONVERTER_VERSION;
    m_var     = 0;
    m_parent  = 0;
    m_next    = 0;
    m_info    = 0;
}


/* ConvertData::SetVariable  0x004017d0 */
void ConvertData::SetVariable( Variable *var )
{
    Type *type;

    m_var = var;

    type = var->m_type;

    assert( type );

    if ( type->CastTypeData() )
    {
        m_parent = ( (ValueData *)var->m_value )->m_scope;
    }
    else if ( type->CastTypeStruct() )
    {
        m_parent = &( (ValueStruct *)var->m_value )->m_scope;
    }
    else if ( type->CastTypeMap() )
    {
        m_parent = &( (ValueMap *)var->m_value )->m_scope;
    }
    else
    {
        m_parent = 0;
    }
}


/* ConvertData::FindVariable  0x00401870 */
Variable *ConvertData::FindVariable( const char *name )
{
    assert( m_var->m_type );

    if ( !m_parent )
    {
        return 0;
    }

    if ( !name )
    {
        return 0;
    }

    return m_parent->Find( std::string( name ) );
}


/* ConvertData::GetAssetName  0x00401550 */
const char *ConvertData::GetAssetName( void )
{
    const char *name;
    const char *colon;

    name  = m_var->m_name.c_str();
    colon = strchr( name, ':' );

    if ( colon )
    {
        return colon + 1;
    }

    return name;
}


/* ConvertData::GetVerbose  0x00401460 */
int ConvertData::GetVerbose( void )
{
    return g_verbose;
}


/* ConvertData::GetSourceDirectory  0x004015a0 */
const char *ConvertData::GetSourceDirectory( void )
{
    return m_info->m_SourceDirectory.c_str();
}


/* ConvertData::GetTargetDirectory  0x004015c0 */
const char *ConvertData::GetTargetDirectory( unsigned int index )
{
    assert( index < m_info->m_TargetDirectory.size() );

    return m_info->m_TargetDirectory[index].c_str();
}


/* ConvertData::GetRelativeTargetDirectory  0x00401650 */
const char *ConvertData::GetRelativeTargetDirectory( unsigned int index )
{
    assert( index < m_info->m_RelativeTargetDirectory.size() );

    return m_info->m_RelativeTargetDirectory[index].c_str();
}


/* ConvertData::GetBaseDirectory  0x004016e0 */
const char *ConvertData::GetBaseDirectory( void )
{
    return g_baseDir.c_str();
}


/* ConvertData::GetAssetDirectory  0x00401700 */
const char *ConvertData::GetAssetDirectory( void )
{
    return g_assetDir[ASSET_DIR_CURRENT].c_str();
}


/* ConvertData::GetDefaultAssetDirectory  0x00401720 */
const char *ConvertData::GetDefaultAssetDirectory( void )
{
    return g_assetDir[ASSET_DIR_DEFAULT].c_str();
}


/* ConvertData::GetPlatformAssetDirectory  0x00401740 */
const char *ConvertData::GetPlatformAssetDirectory( void )
{
    return g_assetDir[ASSET_DIR_SHARED].c_str();
}


/* ConvertData::GetVarName  0x00401990 */
const char *ConvertData::GetVarName( const char *name )
{
    std::list< Variable * >::iterator it;
    Variable                         *var;

    assert( m_var->m_type );

    if ( !m_parent )
    {
        return 0;
    }

    if ( name )
    {
        var = m_parent->Find( std::string( name ) );

        if ( !var )
        {
            return 0;
        }

        for ( it = m_parent->m_variables.begin(); *it != var; ++it )
        {
        }

        ++it;
    }
    else
    {
        it = m_parent->m_variables.begin();
    }

    if ( it == m_parent->m_variables.end() )
    {
        return 0;
    }

    return ( *it )->m_name.c_str();
}


/* ConvertData::GetSubData  0x004019d0 */
ConvertData *ConvertData::GetSubData( const char *name )
{
    Variable    *var;
    ConvertData *data;

    var = FindVariable( name );

    if ( !var )
    {
        return 0;
    }

    data = new ConvertData;

    data->m_next    = g_freeConvertData;
    g_freeConvertData = data;

    data->SetVariable( var );
    data->m_info = m_info;

    return data;
}


/* ConvertData::IsPrimType  0x00401a70 */
bool ConvertData::IsPrimType( void )
{
    Type *type;

    assert( m_var );

    type = m_var->m_type;

    assert( type );

    if ( type->CastTypeData() )
    {
        return false;
    }

    if ( type->CastTypeStruct() )
    {
        return false;
    }

    return !type->CastTypeMap();
}


/* ConvertData::GetPrimType  0x00401b20 */
int ConvertData::GetPrimType( void )
{
    Type *type;

    assert( m_var );
    assert( IsPrimType() );

    type = m_var->m_type;

    assert( type );

    if ( type->CastTypeInteger() )
    {
        return 1;
    }

    return type->CastTypeFloat() ? 2 : 0;
}


/* ConvertData::GetString  0x00401c30 */
const char *ConvertData::GetString( void )
{
    std::string value;

    if ( !m_var )
    {
        return 0;
    }

    assert( m_var->m_value );

    value = m_var->m_value->ToString();

    m_string.assign( value );

    return m_string.c_str();
}


/* ConvertData::GetInt  0x00401d10 */
int ConvertData::GetInt( void )
{
    if ( !m_var )
    {
        return 0;
    }

    assert( m_var->m_value );

    return Legacy_atol( m_var->m_value->ToString().c_str() );
}


/* ConvertData::GetFloat  0x00401e20 */
float ConvertData::GetFloat( void )
{
    if ( !m_var )
    {
        return 0.0f;
    }

    assert( m_var->m_value );

    return (float)Legacy_atof( m_var->m_value->ToString().c_str() );
}


/* ConvertData::GetPlatform  0x00401c00 */
int ConvertData::GetPlatform( void )
{
    return g_platform;
}


/* ConvertData::GetPlatformName  0x00401c10 */
const char *ConvertData::GetPlatformName( void )
{
    return g_platformNames[g_platform].c_str();
}
