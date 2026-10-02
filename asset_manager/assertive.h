/* Original: C:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\assertive.h */

#ifndef ASSERTIVE_H
#define ASSERTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

int  AssertFailed( const char *expression, const char *file, int line, int notification );

#ifdef __cplusplus
}
#endif

#ifdef assert
#undef assert
#endif

#define ASSERT_MESSAGE_SIZE 16384

#define assertx( exp, ... )                                                     \
    do                                                                          \
    {                                                                           \
        static int s_ignore = 0;                                                \
        if ( !s_ignore && !( exp ) )                                            \
        {                                                                       \
            char message[ASSERT_MESSAGE_SIZE];                                  \
            strcpy( message, #exp "\n\tMessage: " );                            \
            strcat( message, va( __VA_ARGS__ ) );                               \
            int result = AssertFailed( message, __FILE__, __LINE__, 0 );        \
            if ( result < 0 )                                                   \
            {                                                                   \
                s_ignore = 1;                                                   \
            }                                                                   \
            else if ( result > 0 )                                              \
            {                                                                   \
                __debugbreak();                                                 \
            }                                                                   \
        }                                                                       \
    }                                                                           \
    while ( 0 )

#define assert( exp )                                                           \
    do                                                                          \
    {                                                                           \
        static int s_ignore = 0;                                                \
        if ( !s_ignore && !( exp ) )                                            \
        {                                                                       \
            int result = AssertFailed( #exp, __FILE__, __LINE__, 0 );           \
            if ( result < 0 )                                                   \
            {                                                                   \
                s_ignore = 1;                                                   \
            }                                                                   \
            else if ( result > 0 )                                              \
            {                                                                   \
                __debugbreak();                                                 \
            }                                                                   \
        }                                                                       \
    }                                                                           \
    while ( 0 )

#endif
