/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\ValueData.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "q_import.h"
#include "assertive.h"


/* ValueData::LoadDefFile  0x004342b0 */
void ValueData::LoadDefFile( void )
{
    bool success;

    Prof_Begin( PROF_EMIT_DEF );
    success = EmitDef( this );
    Prof_End();

    if ( !success )
    {
        delete m_scope;
        m_scope = 0;
    }
}


/* ValueData::ValueData  0x00434330 */
ValueData::ValueData( Variable *variable ) : Value( variable )
{
    assert( variable->m_type->CastTypeData() );

    m_scope = 0;

    SetModified();
}


/* ValueData::~ValueData  0x00434400 */
ValueData::~ValueData()
{
    delete m_scope;
    m_scope = 0;
}


/* ValueData::ToScript  0x00434570 */
void ValueData::ToScript( int indent, buf_t *buf )
{
    std::string fileName;

    Prof_Begin( PROF_DATA_TO_SCRIPT );

    if ( m_fileName.size() )
    {
        AppendBuf( buf, "( \"", 3 );
        fileName = EscapeString( m_fileName );
        AppendBuf( buf, fileName.c_str(), (int)fileName.size() );
        AppendBuf( buf, "\" )\n", 4 );
    }
    else
    {
        AppendBuf( buf, "\n", 1 );
    }

    WriteIndent( buf, indent );
    AppendBuf( buf, "{\n", 2 );
    m_scope->ToScript( indent + 1, buf );
    WriteIndent( buf, indent );
    AppendBuf( buf, "}\n", 2 );

    Prof_End();
}


/* ValueData::SetFileName  0x00434740 */
void ValueData::SetFileName( std::string fileName )
{
    DefFileMap_t::iterator it;

    Prof_Begin( PROF_DATA_SET_FILENAME );

    if ( fileName == m_fileName )
    {
        Prof_End();
        return;
    }

    m_fileName = fileName;

    if ( !m_fileName.size() )
    {
        m_sourceFileName = "";

        delete m_scope;
        m_scope = 0;

        Prof_End();
        return;
    }

    it = g_defFiles.find( m_fileName );

    m_sourceFileName = it != g_defFiles.end() ? it->second->m_Name : std::string( "" );

    delete m_scope;
    m_scope = new ValueScope;

    LoadDefFile();

    Prof_End();
}


/* ValueData::FromScript  0x004348d0 */
bool ValueData::FromScript( char **text )
{
    std::string token;

    Prof_Begin( PROF_DATA_FROM_SCRIPT );

    token = COM_Parse( text );

    if ( token != "(" )
    {
        SetFileName( std::string( "" ) );
    }
    else
    {
        SetFileName( std::string( COM_Parse( text ) ) );
        SetModified();

        token = COM_Parse( text );

        if ( token != ")" )
        {
            Prof_End();
            return false;
        }

        token = COM_Parse( text );
    }

    if ( token != "{" )
    {
        Prof_End();
        return false;
    }

    if ( !m_scope || !m_scope->FromScript( text ) )
    {
        Prof_End();
        return false;
    }

    token = COM_Parse( text );

    Prof_End();

    return token == "}";
}
