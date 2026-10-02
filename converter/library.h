/* Original: .\library.h */

#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

#define CONVERTER_VERSION   2

class Variable;
class ValueScope;
class ConvertInfo;

class ConvertData
{
public:
    ConvertData();                                                              /* 0x00401a40 */

    virtual const char  *GetAssetName( void );                                  /* 0x00401550 */
    virtual int          GetVerbose( void );                                    /* 0x00401460 */
    virtual const char  *GetSourceDirectory( void );                            /* 0x004015a0 */
    virtual const char  *GetTargetDirectory( unsigned int index );              /* 0x004015c0 */
    virtual const char  *GetRelativeTargetDirectory( unsigned int index );      /* 0x00401650 */
    virtual const char  *GetBaseDirectory( void );                              /* 0x004016e0 */
    virtual const char  *GetAssetDirectory( void );                             /* 0x00401700 */
    virtual const char  *GetDefaultAssetDirectory( void );                      /* 0x00401720 */
    virtual const char  *GetPlatformAssetDirectory( void );                     /* 0x00401740 */
    virtual const char  *GetVarName( const char *name );                        /* 0x00401990 */
    virtual ConvertData *GetSubData( const char *name );                        /* 0x004019d0 */
    virtual bool         IsPrimType( void );                                    /* 0x00401a70 */
    virtual const char  *GetString( void );                                     /* 0x00401c30 */
    virtual int          GetInt( void );                                        /* 0x00401d10 */
    virtual float        GetFloat( void );                                      /* 0x00401e20 */
    virtual void         AddTargetFile( const char *fileName );                 /* 0x00403f50 */
    virtual void         AddUnchangedTargetFile( const char *fileName );        /* 0x00403f60 */
    virtual void         AddGeneratedFile( const char *fileName );              /* 0x00404110 */
    virtual void         AddWildcardFiles( const char *dir, const char *name,
                                           const char *extension );             /* 0x004045b0 */
    virtual int          GetPrimType( void );                                   /* 0x00401b20 */
    virtual void         AddDebugFile( const char *fileName );                  /* 0x00403f70 */
    virtual int          GetPlatform( void );                                   /* 0x00401c00 */
    virtual const char  *GetPlatformName( void );                               /* 0x00401c10 */

    void         SetVariable( Variable *var );                                  /* 0x004017d0 */
    Variable    *FindVariable( const char *name );                              /* 0x00401870 */

    int          m_version;     /* +0x04 */
    Variable    *m_var;         /* +0x08 */
    ValueScope  *m_parent;      /* +0x0c */
    std::string  m_string;      /* +0x10 */
    ConvertData *m_next;        /* +0x2c */
    ConvertInfo *m_info;        /* +0x30 */
};

typedef int ( *ConvertFunc_t )( ConvertData *data );

#endif
