cmake_minimum_required( VERSION 3.21 )

project( converter LANGUAGES C CXX )

if( NOT MSVC )
    message( FATAL_ERROR
        "converter requires MSVC.  The original is an x87 MSVC build and other "
        "compilers change floating-point results." )
endif()

if( NOT CMAKE_SIZEOF_VOID_P EQUAL 4 )
    message( FATAL_ERROR
        "converter must be built for 32-bit x86.  Configure with:  "
        "cmake -B build -A Win32" )
endif()

if( NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE )
endif()

set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_SOURCE_DIR}/bin" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin" )

set( SRC_CONVERTER
    converter/converter.cpp
    converter/convertdata.cpp
    converter/gdt.cpp
    converter/q_import.c
)

set( SRC_ASSET_MANAGER
    asset_manager/assertive.cpp
    asset_manager/grammar.cpp
    asset_manager/parser.cpp
    asset_manager/server.cpp
    asset_manager/Type.cpp
    asset_manager/TypeColor.cpp
    asset_manager/TypeData.cpp
    asset_manager/TypeEnum.cpp
    asset_manager/TypeFloat.cpp
    asset_manager/TypeInt.cpp
    asset_manager/TypeMap.cpp
    asset_manager/TypeString.cpp
    asset_manager/TypeStruct.cpp
    asset_manager/utils.cpp
    asset_manager/Value.cpp
    asset_manager/ValueColor.cpp
    asset_manager/ValueData.cpp
    asset_manager/ValueEnum.cpp
    asset_manager/ValueFloat.cpp
    asset_manager/ValueInt.cpp
    asset_manager/ValueMap.cpp
    asset_manager/ValueString.cpp
    asset_manager/ValueStruct.cpp
    asset_manager/Variable.cpp
)

add_executable( converter
    ${SRC_CONVERTER}
    ${SRC_ASSET_MANAGER}
)

target_include_directories( converter PRIVATE
    converter
    asset_manager
)

target_compile_definitions( converter PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_compile_options( converter PRIVATE
    /arch:IA32
    /fp:precise
    /GS-
    /GR-
    /W3
    /wd4996 /wd4244 /wd4018 /wd4267 /wd4305 /wd4101 /wd4700
    $<$<COMPILE_LANGUAGE:CXX>:/EHsc>
    $<$<NOT:$<CONFIG:Debug>>:/O2>
)

target_link_options( converter PRIVATE
    /SUBSYSTEM:CONSOLE
    /STACK:0x100000,0x1000
    /DYNAMICBASE:NO
    /FIXED
    /OPT:REF
    /OPT:ICF
    legacy_stdio_float_rounding.obj
)

set( LIBMYSQL_DEF "${CMAKE_SOURCE_DIR}/asset_manager/libmysql.def" )
set( LIBMYSQL_LIB "${CMAKE_CURRENT_BINARY_DIR}/libmysql.lib" )

add_custom_command(
    OUTPUT  "${LIBMYSQL_LIB}"
    COMMAND "${CMAKE_AR}" /nologo /machine:x86 "/def:${LIBMYSQL_DEF}" "/out:${LIBMYSQL_LIB}"
    DEPENDS "${LIBMYSQL_DEF}"
)
add_custom_target( libmysql DEPENDS "${LIBMYSQL_LIB}" )
add_dependencies( converter libmysql )

target_link_options( converter PRIVATE
    /ALTERNATENAME:_mysql_close@4=_mysql_close
    /ALTERNATENAME:_mysql_init@4=_mysql_init
    /ALTERNATENAME:_mysql_real_connect@32=_mysql_real_connect
)

target_link_libraries( converter PRIVATE
    user32 gdi32 kernel32 advapi32 winmm shell32
    "${LIBMYSQL_LIB}"
)

source_group( "converter"     FILES ${SRC_CONVERTER} )
source_group( "asset_manager" FILES ${SRC_ASSET_MANAGER} )
