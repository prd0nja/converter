/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\server.cpp */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "server.h"
#include "assertive.h"
#include "q_import.h"

typedef struct st_mysql MYSQL;

extern "C" MYSQL * __stdcall mysql_init( MYSQL *mysql );
extern "C" MYSQL * __stdcall mysql_real_connect( MYSQL *mysql, const char *host, const char *user,
                                                 const char *passwd, const char *db,
                                                 unsigned int port, const char *unixSocket,
                                                 unsigned long clientFlag );
extern "C" void    __stdcall mysql_close( MYSQL *mysql );

extern std::string g_cacheSqlServer;
extern std::string g_cacheSqlDb;

struct serverGlob_t
{
    unsigned char mysql[0x220];                         /* +0x000 */
};

static bool         serverInited;                       /* 0x004b4a28 */
static serverGlob_t serverGlob;                         /* 0x004b4a48 */


/* Server_Init  0x0042be00 */
static bool Server_Init( void )
{
    MYSQL *mysql;

    if ( serverInited )
    {
        return true;
    }

    memset( &serverGlob, 0, sizeof( serverGlob ) );

    mysql = (MYSQL *)serverGlob.mysql;

    printf( "Connecting to cache servers...\n" );

    Prof_Begin( PROF_SERVER_SQL );

    if ( !mysql_init( mysql ) )
    {
        Prof_End();
        return false;
    }

    assert( g_cacheSqlServer.length() );
    assert( g_cacheSqlDb.length() );

    if ( !mysql_real_connect( mysql, g_cacheSqlServer.c_str(), "assetdb", NULL,
                              g_cacheSqlDb.c_str(), 0, NULL, 0 ) )
    {
        Prof_End();
        return false;
    }

    Prof_End();

    Prof_Begin( PROF_SERVER_PERFORCE );
    Prof_End();

    return false;
}


/* Server_BeginAsset  0x0042bfd0 */
bool Server_BeginAsset( ConvertData *import, int version )
{
    assert( import );

    if ( !Server_Init() )
    {
        printf( "Server Init Failed.\n" );
    }

    return false;
}


/* Server_Shutdown  0x004274a0 */
void Server_Shutdown( void )
{
    if ( !serverInited )
    {
        return;
    }

    Prof_Begin( PROF_SERVER_SQL );
    mysql_close( (MYSQL *)serverGlob.mysql );
    Prof_End();

    serverInited = false;
}
