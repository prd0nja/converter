/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\Value.cpp */

#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "assertive.h"


/* Value::Value  0x00433bf0 */
Value::Value( Variable *variable )
{
    assert( variable );

    m_parent = variable;
}


/* Value::~Value  0x00433c90 */
Value::~Value()
{
}


/* Value::ToString  0x00433d30 */
std::string Value::ToString( void )
{
    return std::string( "" );
}


/* Value::Finalize  0x00433cb0 */
void Value::Finalize( void )
{
}


/* Value::SetModified  0x00433cc0 */
void Value::SetModified( void )
{
    assert( m_parent );

    m_parent->SetModified();
}


/* AppendBuf  0x00433d60 */
void AppendBuf( buf_t *buf, const char *text, int numChars )
{
    assert( buf->maxsize - ( buf->curpos - buf->string ) > numChars );

    memcpy( buf->curpos, text, numChars );

    buf->curpos += numChars;
    *buf->curpos = 0;
}
