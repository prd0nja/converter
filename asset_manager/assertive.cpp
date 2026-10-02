/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\assertive.cpp */

#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <string.h>

#include "assertive.h"


typedef BOOL ( __stdcall *SymGetModuleInfo_t )( HANDLE, DWORD, PIMAGEHLP_MODULE );
typedef BOOL ( __stdcall *SymGetSymFromAddr_t )( HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL );
typedef BOOL ( __stdcall *SymGetLineFromAddr_t )( HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE );
typedef DWORD ( __stdcall *SymLoadModule_t )( HANDLE, HANDLE, PSTR, PSTR, DWORD, DWORD );
typedef DWORD ( __stdcall *SymGetOptions_t )( void );
typedef DWORD ( __stdcall *SymSetOptions_t )( DWORD );
typedef BOOL ( __stdcall *SymInitialize_t )( HANDLE, PSTR, BOOL );
typedef PVOID ( __stdcall *SymFunctionTableAccess_t )( HANDLE, DWORD );
typedef BOOL ( __stdcall *StackWalk_t )( DWORD, HANDLE, HANDLE, LPSTACKFRAME, PVOID,
                                         PREAD_PROCESS_MEMORY_ROUTINE,
                                         PFUNCTION_TABLE_ACCESS_ROUTINE,
                                         PGET_MODULE_BASE_ROUTINE,
                                         PTRANSLATE_ADDRESS_ROUTINE );

static SymGetModuleInfo_t       s_SymGetModuleInfo;         /* 0x004b4980 */
static SymGetSymFromAddr_t      s_SymGetSymFromAddr;        /* 0x004b4984 */
static SymGetLineFromAddr_t     s_SymGetLineFromAddr;       /* 0x004b4988 */
static SymLoadModule_t          s_SymLoadModule;            /* 0x004b498c */
static SymGetOptions_t          s_SymGetOptions;            /* 0x004b4990 */
static SymSetOptions_t          s_SymSetOptions;            /* 0x004b4994 */
static SymInitialize_t          s_SymInitialize;            /* 0x004b4998 */
static SymFunctionTableAccess_t s_SymFunctionTableAccess;   /* 0x004b499c */
static StackWalk_t              s_StackWalk;                /* 0x205060e0 */

static int s_symbolsLoaded;                                 /* 0x004b49a0 */


/* Assert_FrameName  0x0041fd40 */
static int Assert_FrameName( HANDLE process, DWORD addr, char *out )
{
    IMAGEHLP_MODULE moduleInfo;
    IMAGEHLP_SYMBOL symbol;
    IMAGEHLP_LINE   line;
    DWORD           displacement;
    int             len;

    len = sprintf( out, "0x%08x:\t", addr );

    memset( &moduleInfo, 0, sizeof( moduleInfo ) );
    moduleInfo.SizeOfStruct = sizeof( moduleInfo );

    if ( !s_SymGetModuleInfo( process, addr, &moduleInfo ) )
    {
        strcpy( out + len, "name unavailable - SymGetModuleInfo() failed\n" );
        return (int)strlen( out );
    }

    memset( &symbol, 0, sizeof( symbol ) );
    symbol.SizeOfStruct  = sizeof( symbol );
    symbol.MaxNameLength = MAX_PATH;

    if ( !s_SymGetSymFromAddr( process, addr, &displacement, &symbol ) )
    {
        strcpy( out + len, "name unavailable - SymGetSymFromAddr() failed\n" );
        return (int)strlen( out );
    }

    memset( &line, 0, sizeof( line ) );
    line.SizeOfStruct = sizeof( line );

    if ( s_SymGetLineFromAddr( process, addr, &displacement, &line ) )
    {
        len += sprintf( out + len, "%s line %d\n", line.FileName, line.LineNumber );
    }
    else
    {
        len += sprintf( out + len, "%s\n", symbol.Name );
    }

    return len;
}


/* Assert_GetModuleBase  0x0041fef0 */
static DWORD __stdcall Assert_GetModuleBase( HANDLE process, DWORD addr )
{
    IMAGEHLP_MODULE       moduleInfo;
    MEMORY_BASIC_INFORMATION memInfo;
    char                  moduleName[MAX_PATH];
    HANDLE                file;

    if ( s_SymGetModuleInfo( process, addr, &moduleInfo ) )
    {
        return moduleInfo.BaseOfImage;
    }

    if ( VirtualQueryEx( process, (LPCVOID)addr, &memInfo, sizeof( memInfo ) ) )
    {
        if ( GetModuleFileNameA( (HMODULE)memInfo.AllocationBase, moduleName, MAX_PATH ) )
        {
            file = CreateFileA( moduleName, GENERIC_READ, FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, 0, NULL );

            return s_SymLoadModule( process, file, moduleName, NULL,
                                    (DWORD)memInfo.AllocationBase, 0 );
        }
    }

    return 0;
}


/* Assert_StackTrace  0x0041ffd0 */
static int Assert_StackTrace( char *out, int frameCount, int skipCount )
{
    HANDLE      process;
    HANDLE      thread;
    HMODULE     imagehlp;
    CONTEXT     context;
    STACKFRAME  frame;
    int         len;
    int         i;
    int         total;

    process = GetCurrentProcess();
    thread  = GetCurrentThread();

    if ( !s_symbolsLoaded )
    {
        imagehlp = LoadLibraryA( "IMAGEHLP.DLL" );

        if ( !imagehlp )
        {
            s_symbolsLoaded = -1;
            return 0;
        }

        s_SymGetModuleInfo       = (SymGetModuleInfo_t)GetProcAddress( imagehlp, "SymGetModuleInfo" );
        s_SymGetSymFromAddr      = (SymGetSymFromAddr_t)GetProcAddress( imagehlp, "SymGetSymFromAddr" );
        s_SymGetLineFromAddr     = (SymGetLineFromAddr_t)GetProcAddress( imagehlp, "SymGetLineFromAddr" );
        s_SymLoadModule          = (SymLoadModule_t)GetProcAddress( imagehlp, "SymLoadModule" );
        s_SymGetOptions          = (SymGetOptions_t)GetProcAddress( imagehlp, "SymGetOptions" );
        s_SymSetOptions          = (SymSetOptions_t)GetProcAddress( imagehlp, "SymSetOptions" );
        s_SymInitialize          = (SymInitialize_t)GetProcAddress( imagehlp, "SymInitialize" );
        s_SymFunctionTableAccess = (SymFunctionTableAccess_t)GetProcAddress( imagehlp, "SymFunctionTableAccess" );
        s_StackWalk              = (StackWalk_t)GetProcAddress( imagehlp, "StackWalk" );

        if ( !s_SymGetModuleInfo || !s_SymGetSymFromAddr || !s_SymGetLineFromAddr
             || !s_SymLoadModule || !s_SymGetOptions || !s_SymSetOptions
             || !s_SymInitialize || !s_SymFunctionTableAccess || !s_StackWalk )
        {
            s_symbolsLoaded = -1;
            return 0;
        }

        s_SymSetOptions( s_SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS );

        if ( !s_SymInitialize( process, NULL, FALSE ) )
        {
            strcpy( out, "failed, SymInitialize() == 0\n" );
            s_symbolsLoaded = -1;
            return (int)strlen( out );
        }

        s_symbolsLoaded = 1;
    }
    else if ( s_symbolsLoaded < 0 )
    {
        return 0;
    }

    context.ContextFlags = CONTEXT_FULL;

    if ( !GetThreadContext( thread, &context ) )
    {
        return 0;
    }

    memset( &frame, 0, sizeof( frame ) );

    frame.AddrPC.Offset    = context.Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrStack.Mode   = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;

    len   = 0;
    total = frameCount + skipCount;

    for ( i = 0; i < total; i++ )
    {
        if ( !s_StackWalk( IMAGE_FILE_MACHINE_I386, process, thread, &frame, &context,
                           NULL, (PFUNCTION_TABLE_ACCESS_ROUTINE)s_SymFunctionTableAccess,
                           (PGET_MODULE_BASE_ROUTINE)Assert_GetModuleBase, NULL ) )
        {
            break;
        }

        if ( frame.AddrPC.Offset && i >= skipCount )
        {
            len += Assert_FrameName( process, frame.AddrPC.Offset, out + len );
        }
    }

    return len;
}


/* AssertFailed  0x004202c0 */
int AssertFailed( const char *expression, const char *file, int line, int notification )
{
    char  moduleFileName[MAX_PATH];
    char  errorMessage[256];
    char  text[32768];
    DWORD lastError;
    int   len;

    lastError = GetLastError();

    FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, lastError, 0, errorMessage, sizeof( errorMessage ), NULL );

    if ( !GetModuleFileNameA( NULL, moduleFileName, sizeof( moduleFileName ) ) )
    {
        strcpy( moduleFileName, "<unknown>" );
    }

    len = sprintf( text,
                   "Expression:\n\t%s\n\nModule:\t%s\nFile:\t%s\nLine:\t%d\n"
                   "GetLastError(0x%08x): %s\nPartial stack trace:\n",
                   expression, moduleFileName, file, line, lastError, errorMessage );

    len += Assert_StackTrace( text + len, 10, 2 );

    strcpy( text + len, "\nPress RETRY to debug\n" );

    switch ( MessageBoxA( NULL, text,
                          notification ? "Debug Notification" : "Debug Assertion Failed!",
                          MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_DEFBUTTON2
                          | MB_TASKMODAL | MB_SETFOREGROUND ) )
    {
    case IDABORT:
        ChangeDisplaySettingsA( NULL, 0 );
        ExitProcess( (UINT)-1 );

    case IDIGNORE:
        return GetKeyState( VK_SHIFT ) ? -1 : 0;

    default:
        return 1;
    }
}
