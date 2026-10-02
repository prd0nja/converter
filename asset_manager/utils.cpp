/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\utils.cpp */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <eh.h>

#include "utils.h"
#include "assertive.h"

std::string g_baseDir;              /* 0x00486160 */
std::string g_srcCacheBaseDir;      /* 0x00486200 */
std::string g_tgtCacheBaseDir;      /* 0x0048621c */


/* InitBuf  0x0042f060 */
void InitBuf( buf_t *buf, char *string, int maxsize )
{
    buf->maxsize = maxsize;
    buf->string  = string;
    buf->curpos  = string;

    *string = 0;
}


/* WriteIndent  0x0042f080 */
void WriteIndent( buf_t *buf, int indent )
{
    assert( buf->maxsize - ( buf->curpos - buf->string ) > indent );

    while ( indent )
    {
        *buf->curpos = '\t';
        buf->curpos++;
        indent--;
    }

    *buf->curpos = 0;
}


/* ReadLine  0x0042f180 */
char *ReadLine( FILE *fp, char *buf, int size )
{
    char *start;
    int   len;

    while ( fgets( buf, size, fp ) )
    {
        buf[size - 1] = 0;

        start = buf;
        while ( isspace( *start ) )
        {
            start++;
        }

        len = (int)strlen( start );

        while ( len && isspace( start[len - 1] ) )
        {
            len--;
        }

        if ( len )
        {
            start[len] = 0;
            return start;
        }
    }

    return NULL;
}


/* SetBaseDir  0x00430e30 */
void SetBaseDir( const char *currDir, int levelsUp )
{
    char *buf;
    char *end;

    buf = (char *)_alloca( strlen( currDir ) + 1 );
    strcpy( buf, currDir );

    assert( currDir );

    for ( ;; )
    {
        end = buf + strlen( buf ) - 1;

        while ( *end != '\\' && *end != '/' )
        {
            *end = 0;
            end--;
        }

        if ( !levelsUp )
        {
            break;
        }

        levelsUp--;
        *end = 0;
    }

    g_baseDir = buf;

    g_srcCacheBaseDir = g_baseDir + "assetsrccache" + "/";
    g_tgtCacheBaseDir = g_baseDir + "assettgtcache" + "/";
}


/* EscapeString  0x00430cc0 */
std::string EscapeString( std::string str )
{
    char *buf;
    char *out;
    int   len;
    int   i;

    len = (int)str.size();
    buf = (char *)_alloca( len * 2 + 1 );
    out = buf;

    for ( i = 0; i < len; i++ )
    {
        switch ( str.c_str()[i] )
        {
        case '\n':
            *out++ = '\\';
            *out = 'n';
            break;
        case '\\':
            *out++ = '\\';
            *out = '\\';
            break;
        case '\t':
            *out++ = '\\';
            *out = 't';
            break;
        case '\r':
            *out++ = '\\';
            *out = 'r';
            break;
        case '\"':
            *out++ = '\\';
            *out = '\"';
            break;
        default:
            *out = str.c_str()[i];
            break;
        }

        out++;
    }

    *out = 0;

    return std::string( buf );
}


/* MakeRelativePath  0x00431040 */
std::string MakeRelativePath( std::string base, std::string full )
{
    int  base_len;
    int  full_len;
    int  base_pos;
    int  full_pos;
    char base_char;

    base_len = (int)base.size();
    full_len = (int)full.size();
    base_pos = 0;
    full_pos = 0;

    for ( ;; )
    {
        base_char = base.c_str()[base_pos];

        if ( base_char == '\\' || base_char == '/' )
        {
            do
            {
                base_pos++;
                if ( base_pos >= base_len )
                {
                    break;
                }
                base_char = base.c_str()[base_pos];
            }
            while ( base_char == '\\' || base_char == '/' );

            if ( full_pos >= full_len )
            {
                return std::string( "" );
            }

            if ( full.c_str()[full_pos] != '\\' && full.c_str()[full_pos] != '/' )
            {
                return std::string( "" );
            }

            do
            {
                full_pos++;
                if ( full_pos >= full_len )
                {
                    return std::string( "" );
                }
            }
            while ( full.c_str()[full_pos] == '\\' || full.c_str()[full_pos] == '/' );
        }

        if ( base_pos >= base_len )
        {
            assert( full_pos <= full_len );
            return std::string( full.c_str() + full_pos );
        }

        assert( !( base_char == '\\' || base_char == '/' ) );

        if ( full_pos >= full_len )
        {
            return std::string( "" );
        }

        if ( _strnicmp( base.c_str() + base_pos, full.c_str() + full_pos, 1 ) )
        {
            return std::string( "" );
        }

        base_pos++;
        full_pos++;
    }
}


long Legacy_atol( const char *nptr )
{
    unsigned long total;
    int           sign;
    int           c;

    while ( isspace( (unsigned char)*nptr ) )
    {
        nptr++;
    }

    c    = (unsigned char)*nptr++;
    sign = c;

    if ( c == '-' || c == '+' )
    {
        c = (unsigned char)*nptr++;
    }

    total = 0;

    while ( c >= '0' && c <= '9' )
    {
        total = total * 10 + ( c - '0' );
        c     = (unsigned char)*nptr++;
    }

    if ( sign == '-' )
    {
        total = 0 - total;
    }

    return (long)total;
}


#define LEGACY_DEC_DIGITS   2200

typedef struct
{
    char digits[LEGACY_DEC_DIGITS];
    int  count;
    int  exp10;
} legacyDec_t;


static void Legacy_DecTrim( legacyDec_t *dec )
{
    int lead;

    lead = 0;

    while ( lead < dec->count && dec->digits[lead] == '0' )
    {
        lead++;
    }

    if ( lead )
    {
        memmove( dec->digits, dec->digits + lead, dec->count - lead );
        dec->count -= lead;
        dec->exp10 -= lead;
    }

    while ( dec->count && dec->digits[dec->count - 1] == '0' )
    {
        dec->count--;
    }

    if ( !dec->count )
    {
        dec->exp10 = 0;
    }
}


static void Legacy_DecFromDouble( double value, legacyDec_t *dec )
{
    char        buf[800];
    const char *in;

    sprintf( buf, "%.770e", value < 0.0 ? -value : value );

    dec->count               = 0;
    dec->digits[dec->count++] = buf[0];

    for ( in = buf + 2; *in != 'e'; in++ )
    {
        dec->digits[dec->count++] = *in;
    }

    dec->exp10 = atoi( in + 1 ) + 1;

    Legacy_DecTrim( dec );
}


static void Legacy_DecAdd( const legacyDec_t *a, const legacyDec_t *b, legacyDec_t *out )
{
    static int sum[LEGACY_DEC_DIGITS * 2];
    int        lo;
    int        hi;
    int        width;
    int        carry;
    int        i;

    lo = a->exp10 - a->count < b->exp10 - b->count ? a->exp10 - a->count : b->exp10 - b->count;
    hi = a->exp10 > b->exp10 ? a->exp10 : b->exp10;

    width = hi - lo + 1;

    memset( sum, 0, width * sizeof( sum[0] ) );

    for ( i = 0; i < a->count; i++ )
    {
        sum[a->exp10 - 1 - i - lo] += a->digits[i] - '0';
    }

    for ( i = 0; i < b->count; i++ )
    {
        sum[b->exp10 - 1 - i - lo] += b->digits[i] - '0';
    }

    carry = 0;

    for ( i = 0; i < width; i++ )
    {
        sum[i] += carry;
        carry   = sum[i] / 10;
        sum[i] %= 10;
    }

    out->count = 0;

    for ( i = width - 1; i >= 0; i-- )
    {
        out->digits[out->count++] = (char)( '0' + sum[i] );
    }

    out->exp10 = hi + 1;

    Legacy_DecTrim( out );
}


static bool Legacy_DecEqual( const legacyDec_t *a, const legacyDec_t *b )
{
    return a->count == b->count && a->exp10 == b->exp10 && !memcmp( a->digits, b->digits, a->count );
}


static bool Legacy_DecIsMidpoint( const legacyDec_t *value, double low, double high )
{
    legacyDec_t lowDec;
    legacyDec_t halfDec;
    legacyDec_t midDec;

    Legacy_DecFromDouble( low, &lowDec );
    Legacy_DecFromDouble( ( high - low ) * 0.5, &halfDec );
    Legacy_DecAdd( &lowDec, &halfDec, &midDec );

    return Legacy_DecEqual( value, &midDec );
}


static const char *Legacy_ScanNumber( const char *text, bool atofSyntax, legacyDec_t *dec, bool *negative,
                                      bool *found )
{
    bool nonZero;
    int  exponent;
    int  expSign;
    int  i;

    *negative = false;
    *found    = false;
    nonZero   = false;

    dec->count = 0;
    dec->exp10 = 0;

    if ( *text == '-' || *text == '+' )
    {
        *negative = *text == '-';
        text++;
    }

    while ( *text >= '0' && *text <= '9' )
    {
        *found = true;

        if ( nonZero || *text != '0' )
        {
            nonZero = true;

            if ( dec->count < 25 )
            {
                dec->digits[dec->count++] = *text;
            }

            dec->exp10++;
        }

        text++;
    }

    if ( *text == '.' )
    {
        text++;

        while ( *text >= '0' && *text <= '9' )
        {
            *found = true;

            if ( nonZero || *text != '0' )
            {
                nonZero = true;

                if ( dec->count < 25 )
                {
                    dec->digits[dec->count++] = *text;
                }
            }
            else
            {
                dec->exp10--;
            }

            text++;
        }
    }

    if ( ( *text == 'e' || *text == 'E' || ( atofSyntax && ( *text == 'd' || *text == 'D' ) ) ) && ( atofSyntax || *found ) )
    {
        text++;

        expSign  = 1;
        exponent = 0;

        if ( *text == '-' || *text == '+' )
        {
            expSign = *text == '-' ? -1 : 1;
            text++;
        }

        while ( *text >= '0' && *text <= '9' )
        {
            if ( exponent < 100000 )
            {
                exponent = exponent * 10 + ( *text - '0' );
            }

            text++;
        }

        if ( dec->count )
        {
            dec->exp10 += expSign * exponent;
        }
    }

    if ( dec->count == 25 )
    {
        if ( dec->digits[23] >= '5' )
        {
            for ( i = 23; i >= 0; i-- )
            {
                if ( dec->digits[i] != '9' )
                {
                    dec->digits[i]++;
                    break;
                }

                dec->digits[i] = '0';
            }

            if ( i < 0 )
            {
                memmove( dec->digits + 1, dec->digits, 24 );
                dec->digits[0] = '1';
                dec->exp10++;
            }
        }

        dec->count = 24;
    }

    Legacy_DecTrim( dec );

    return text;
}


static void Legacy_DecToString( const legacyDec_t *dec, char *buf )
{
    sprintf( buf, "0.%.*se%d", dec->count, dec->digits, dec->exp10 );
}


static bool Legacy_DecTie( const legacyDec_t *dec, int bits, double *lower )
{
    unsigned __int64 mantissa;
    int              exponent;
    int              length;
    int              i;

    if ( dec->count > 19 )
    {
        return false;
    }

    mantissa = 0;

    for ( i = 0; i < dec->count; i++ )
    {
        mantissa = mantissa * 10 + ( dec->digits[i] - '0' );
    }

    exponent = dec->exp10 - dec->count;

    if ( exponent >= 0 )
    {
        if ( exponent > 27 )
        {
            return false;
        }

        while ( !( mantissa & 1 ) )
        {
            mantissa >>= 1;
            exponent++;
        }

        for ( i = 0; i < dec->exp10 - dec->count; i++ )
        {
            if ( mantissa > 0xffffffffffffffffULL / 5 )
            {
                return false;
            }

            mantissa *= 5;
        }
    }
    else
    {
        if ( exponent < -27 )
        {
            return false;
        }

        for ( i = 0; i < -exponent; i++ )
        {
            if ( mantissa % 5 )
            {
                return false;
            }

            mantissa /= 5;
        }

        while ( !( mantissa & 1 ) )
        {
            mantissa >>= 1;
            exponent++;
        }
    }

    for ( length = 0; length < 64 && ( mantissa >> length ); length++ )
    {
    }

    if ( length != bits + 1 )
    {
        return false;
    }

    *lower = ldexp( (double)( ( mantissa - 1 ) / 2 ), exponent + 1 );

    return true;
}


static double Legacy_DecToDouble( const legacyDec_t *dec, bool negative )
{
    char   buf[64];
    double value;
    double low;

    if ( !dec->count )
    {
        return negative ? -0.0 : 0.0;
    }

    Legacy_DecToString( dec, buf );

    value = strtod( buf, NULL );

    if ( value >= DBL_MIN && value <= DBL_MAX )
    {
        if ( Legacy_DecTie( dec, 53, &low ) )
        {
            if ( low >= DBL_MIN )
            {
                value = low;
            }
        }
        else if ( dec->count > 19 )
        {
            low = nextafter( value, 0.0 );

            if ( Legacy_DecIsMidpoint( dec, low, value ) )
            {
                value = low;
            }
        }
    }

    return negative ? -value : value;
}


static float Legacy_DecToFloat( const legacyDec_t *dec, bool negative )
{
    char   buf[64];
    double exact;
    double units;
    double whole;
    double low;
    float  value;
    float  lowFloat;

    if ( !dec->count )
    {
        return negative ? -0.0f : 0.0f;
    }

    Legacy_DecToString( dec, buf );

    exact = strtod( buf, NULL );

    if ( exact < FLT_MIN )
    {
        units = ldexp( exact, 149 );
        whole = floor( units );

        if ( units - whole >= 0.75 )
        {
            whole += 1.0;
        }

        value = (float)ldexp( whole, -149 );

        return negative ? -value : value;
    }

    value = strtof( buf, NULL );

    if ( value <= FLT_MAX )
    {
        if ( Legacy_DecTie( dec, 24, &low ) )
        {
            value = (float)low;
        }
        else if ( dec->count > 19 )
        {
            lowFloat = nextafterf( value, 0.0f );

            if ( Legacy_DecIsMidpoint( dec, lowFloat, value ) )
            {
                value = lowFloat;
            }
        }
    }

    return negative ? -value : value;
}


double Legacy_atof( const char *nptr )
{
    legacyDec_t dec;
    bool        negative;
    bool        found;

    while ( isspace( (unsigned char)*nptr ) )
    {
        nptr++;
    }

    Legacy_ScanNumber( nptr, true, &dec, &negative, &found );

    if ( !found )
    {
        return 0.0;
    }

    return Legacy_DecToDouble( &dec, negative );
}


typedef struct
{
    int  sign;
    int  decpt;
    char mantissa[24];
} legacyFlt_t;


static void Legacy_FltOut( double value, legacyFlt_t *flt )
{
    unsigned __int64 bits;
    unsigned __int64 mantissa;
    char             buf[64];
    char             exact[800];
    const char      *digits;
    const char      *exponent;
    const char      *in;
    char            *out;
    int              i;

    memcpy( &bits, &value, sizeof( bits ) );

    flt->sign = ( bits >> 63 ) ? '-' : ' ';
    mantissa  = bits & 0x000fffffffffffffULL;

    if ( ( ( bits >> 52 ) & 0x7ff ) == 0x7ff )
    {
        if ( !mantissa )
        {
            strcpy( flt->mantissa, "1#INF" );
        }
        else if ( ( bits >> 63 ) && mantissa == 0x0008000000000000ULL )
        {
            strcpy( flt->mantissa, "1#IND" );
        }
        else if ( mantissa & 0x0008000000000000ULL )
        {
            strcpy( flt->mantissa, "1#QNAN" );
        }
        else
        {
            strcpy( flt->mantissa, "1#SNAN" );
        }

        flt->decpt = 1;
        return;
    }

    if ( !( bits & 0x7fffffffffffffffULL ) )
    {
        strcpy( flt->mantissa, "0" );
        flt->sign  = ' ';
        flt->decpt = 0;
        return;
    }

    if ( value < 0.0 )
    {
        value = -value;
    }

    sprintf( buf, "%.17e", value );

    digits   = 0;
    exponent = 0;

    if ( buf[18] == '5' )
    {
        sprintf( exact, "%.770e", value );

        if ( exact[18] == '5' )
        {
            for ( in = exact + 19; *in == '0'; in++ )
            {
            }

            if ( *in == 'e' )
            {
                digits   = exact;
                exponent = in;
            }
        }
    }

    if ( !digits )
    {
        sprintf( buf, "%.16e", value );

        digits   = buf;
        exponent = strchr( buf, 'e' );
    }

    out    = flt->mantissa;
    *out++ = digits[0];

    for ( i = 2; i < 18; i++ )
    {
        *out++ = digits[i];
    }

    *out = 0;

    flt->decpt = atoi( exponent + 1 ) + 1;
}


static void Legacy_FpToStr( char *buf, int digits, legacyFlt_t *flt )
{
    char       *pbuf;
    const char *mantissa;

    pbuf     = buf;
    mantissa = flt->mantissa;

    *pbuf++ = '0';

    while ( digits > 0 )
    {
        *pbuf++ = *mantissa ? *mantissa++ : '0';
        digits--;
    }

    *pbuf = 0;

    if ( digits >= 0 && *mantissa >= '5' )
    {
        pbuf--;

        while ( *pbuf == '9' )
        {
            *pbuf-- = '0';
        }

        *pbuf += 1;
    }

    if ( *buf == '1' )
    {
        flt->decpt++;
    }
    else
    {
        memmove( buf, buf + 1, strlen( buf + 1 ) + 1 );
    }
}


static void Legacy_CfToE( char *out, const char *digits, int ndec, legacyFlt_t *flt, bool zero )
{
    int exponent;

    *out++ = digits[0];

    if ( ndec > 0 )
    {
        *out++ = '.';
        memcpy( out, digits + 1, ndec );
        out += ndec;
    }

    exponent = zero ? 0 : flt->decpt - 1;

    sprintf( out, "e%c%03d", exponent < 0 ? '-' : '+', exponent < 0 ? -exponent : exponent );
}


static void Legacy_CfToF( char *out, const char *digits, int ndec, int decpt, bool gFormat )
{
    int len;
    int zeros;

    len = (int)strlen( digits );

    if ( decpt <= 0 )
    {
        *out++ = '0';
    }
    else
    {
        memcpy( out, digits, decpt );
        out    += decpt;
        digits += decpt;
        len    -= decpt;
    }

    if ( ndec > 0 )
    {
        *out++ = '.';

        if ( decpt < 0 )
        {
            zeros = gFormat ? -decpt : ( ndec < -decpt ? ndec : -decpt );
            memset( out, '0', zeros );
            out += zeros;
        }

        memcpy( out, digits, len );
        out += len;
    }

    *out = 0;
}


static void Legacy_CropZeros( char *buf )
{
    char *stop;

    while ( *buf && *buf != '.' )
    {
        buf++;
    }

    if ( *buf++ )
    {
        while ( *buf && *buf != 'e' && *buf != 'E' )
        {
            buf++;
        }

        stop = buf--;

        while ( *buf == '0' )
        {
            buf--;
        }

        if ( *buf == '.' )
        {
            buf--;
        }

        while ( ( *++buf = *stop++ ) != 0 )
        {
        }
    }
}


static void Legacy_FormatFloat( char *buf, double value, char format, int precision )
{
    legacyFlt_t flt;
    char        digits[400];
    char       *out;
    int         magnitude;
    bool        zero;

    Legacy_FltOut( value, &flt );

    zero = flt.mantissa[0] == '0';

    out = buf;

    if ( flt.sign == '-' )
    {
        *out++ = '-';
    }

    if ( format == 'e' )
    {
        Legacy_FpToStr( digits, precision + 1, &flt );
        Legacy_CfToE( out, digits, precision, &flt, zero );
        return;
    }

    if ( format == 'f' )
    {
        Legacy_FpToStr( digits, precision + flt.decpt, &flt );
        Legacy_CfToF( out, digits, precision, flt.decpt, false );
        return;
    }

    if ( !precision )
    {
        precision = 1;
    }

    Legacy_FpToStr( digits, precision, &flt );

    digits[precision] = 0;

    magnitude = flt.decpt - 1;

    if ( magnitude < -4 || magnitude >= precision )
    {
        Legacy_CfToE( out, digits, precision - 1, &flt, zero );
    }
    else
    {
        Legacy_CfToF( out, digits, precision - flt.decpt, flt.decpt, true );
    }

    Legacy_CropZeros( buf );
}


/* CRC32  0x00420870 */
unsigned CRC32( const char *data, int len, unsigned crc )
{
    const char *end;
    int         i;

    end = data + len;
    crc = ~crc;

    while ( data != end )
    {
        crc ^= (unsigned char)*data;

        for ( i = 0; i < 8; i++ )
        {
            crc = ( crc >> 1 ) ^ ( 0xedb88320 * ( crc & 1 ) );
        }

        data++;
    }

    return ~crc;
}


int Legacy_vsprintf( char *buf, const char *fmt, va_list args )
{
    char        spec[64];
    char        number[1100];
    const char *start;
    char       *out;
    char       *p;
    bool        leftAlign;
    bool        zeroPad;
    char        signChar;
    int         width;
    int         precision;
    int         len;
    int         pad;

    out = buf;

    while ( *fmt )
    {
        if ( *fmt != '%' )
        {
            *out++ = *fmt++;
            continue;
        }

        start = fmt++;

        if ( *fmt == '%' )
        {
            *out++ = *fmt++;
            continue;
        }

        leftAlign = false;
        zeroPad   = false;
        signChar  = 0;

        for ( ;; )
        {
            if ( *fmt == '-' )
            {
                leftAlign = true;
            }
            else if ( *fmt == '0' )
            {
                zeroPad = true;
            }
            else if ( *fmt == '+' )
            {
                signChar = '+';
            }
            else if ( *fmt == ' ' )
            {
                if ( !signChar )
                {
                    signChar = ' ';
                }
            }
            else if ( *fmt != '#' )
            {
                break;
            }

            fmt++;
        }

        width = 0;

        if ( *fmt == '*' )
        {
            width = va_arg( args, int );
            fmt++;
        }
        else
        {
            while ( *fmt >= '0' && *fmt <= '9' )
            {
                width = width * 10 + ( *fmt++ - '0' );
            }
        }

        precision = -1;

        if ( *fmt == '.' )
        {
            fmt++;
            precision = 0;

            if ( *fmt == '*' )
            {
                precision = va_arg( args, int );
                fmt++;
            }
            else
            {
                while ( *fmt >= '0' && *fmt <= '9' )
                {
                    precision = precision * 10 + ( *fmt++ - '0' );
                }
            }
        }

        while ( *fmt == 'h' || *fmt == 'l' || *fmt == 'L' )
        {
            fmt++;
        }

        if ( *fmt == 'e' || *fmt == 'E' || *fmt == 'f' || *fmt == 'g' || *fmt == 'G' )
        {
            p = number;

            Legacy_FormatFloat( number, va_arg( args, double ),
                                (char)( *fmt == 'E' ? 'e' : *fmt == 'G' ? 'g' : *fmt ),
                                precision < 0 ? 6 : precision );

            if ( *fmt == 'E' || *fmt == 'G' )
            {
                for ( p = number; *p; p++ )
                {
                    if ( *p == 'e' )
                    {
                        *p = 'E';
                    }
                }

                p = number;
            }

            if ( *p == '-' )
            {
                signChar = *p++;
            }

            len = (int)strlen( p ) + ( signChar ? 1 : 0 );
            pad = width > len ? width - len : 0;

            if ( !leftAlign && !zeroPad )
            {
                while ( pad-- > 0 )
                {
                    *out++ = ' ';
                }
            }

            if ( signChar )
            {
                *out++ = signChar;
            }

            if ( !leftAlign && zeroPad )
            {
                while ( pad-- > 0 )
                {
                    *out++ = '0';
                }
            }

            strcpy( out, p );
            out += strlen( p );

            if ( leftAlign )
            {
                while ( pad-- > 0 )
                {
                    *out++ = ' ';
                }
            }

            fmt++;
            continue;
        }

        len = (int)( fmt - start ) + 1;

        if ( len >= (int)sizeof( spec ) )
        {
            len = (int)sizeof( spec ) - 1;
        }

        memcpy( spec, start, len );
        spec[len] = 0;

        if ( *fmt == 's' )
        {
            out += sprintf( out, spec, va_arg( args, const char * ) );
        }
        else if ( *fmt == 'p' )
        {
            out += sprintf( out, spec, va_arg( args, void * ) );
        }
        else
        {
            out += sprintf( out, spec, va_arg( args, int ) );
        }

        if ( *fmt )
        {
            fmt++;
        }
    }

    *out = 0;

    return (int)( out - buf );
}


int Legacy_sprintf( char *buf, const char *fmt, ... )
{
    va_list args;
    int     len;

    va_start( args, fmt );
    len = Legacy_vsprintf( buf, fmt, args );
    va_end( args );

    return len;
}


int Legacy_vsscanf( const char *text, const char *fmt, va_list args )
{
    static legacyDec_t dec;
    unsigned long      number;
    bool               negative;
    bool               found;
    int                started;
    int                count;

    count = 0;

    while ( *fmt )
    {
        if ( isspace( (unsigned char)*fmt ) )
        {
            while ( isspace( (unsigned char)*text ) )
            {
                text++;
            }

            fmt++;
            continue;
        }

        if ( *fmt != '%' )
        {
            if ( *text != *fmt )
            {
                return count;
            }

            text++;
            fmt++;
            continue;
        }

        fmt++;

        while ( isspace( (unsigned char)*text ) )
        {
            text++;
        }

        if ( *fmt == 'd' )
        {
            negative = *text == '-';

            if ( *text == '-' || *text == '+' )
            {
                text++;
            }

            number  = 0;
            started = 0;

            while ( *text >= '0' && *text <= '9' )
            {
                number = number * 10 + ( *text++ - '0' );
                started++;
            }

            if ( !started )
            {
                return count;
            }

            *va_arg( args, int * ) = (int)( negative ? 0 - number : number );
            count++;
        }
        else if ( *fmt == 'f' )
        {
            text = Legacy_ScanNumber( text, false, &dec, &negative, &found );

            if ( !found )
            {
                return count;
            }

            *va_arg( args, float * ) = Legacy_DecToFloat( &dec, negative );
            count++;
        }
        else
        {
            return count;
        }

        fmt++;
    }

    return count;
}


int Legacy_sscanf( const char *text, const char *fmt, ... )
{
    va_list args;
    int     count;

    va_start( args, fmt );
    count = Legacy_vsscanf( text, fmt, args );
    va_end( args );

    return count;
}


static void Legacy_Terminate( void )
{
    static const char message[] =
        "\r\nThis application has requested the Runtime to terminate it in an unusual way.\n"
        "Please contact the application's support team for more information.\r\n";
    DWORD written;

    WriteFile( GetStdHandle( STD_ERROR_HANDLE ), message, sizeof( message ) - 1, &written, NULL );

    _exit( 3 );
}


static int Legacy_InstallTerminate( void )
{
    set_terminate( Legacy_Terminate );

    return 0;
}


static int s_legacyTerminate = Legacy_InstallTerminate();
