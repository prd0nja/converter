/* Original: c:\trees\cod3-pc\cod3-modtools\src\asset_manager\asset_manager\parser/parser.h */

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <list>
#include <map>

#include "utils.h"

class Type;
class TypeColor;
class TypeData;
class TypeEnum;
class TypeFloat;
class TypeInt;
class TypeMap;
class TypeString;
class TypeStruct;
class Value;
class Variable;
class ValueScope;
class ObjectInfo;


void AppendBuf( buf_t *buf, const char *text, int numChars );                   /* 0x00433d60 */


class Value
{
public:
    Value( Variable *variable );                                                /* 0x00433bf0 */
    virtual ~Value();                                                           /* 0x00433c90 */

    virtual void        ToScript( int indent, buf_t *buf ) = 0;
    virtual bool        FromScript( char **text ) = 0;
    virtual std::string ToString( void );                                       /* 0x00433d30 */
    virtual void        Finalize( void );                                       /* 0x00433cb0 */
    virtual void        SetModified( void );                                    /* 0x00433cc0 */

    Variable *m_parent;     /* +0x04 */
};


class ValueInt : public Value
{
public:
    ValueInt( Variable *variable );                                             /* 0x00435350 */

    virtual void        ToScript( int indent, buf_t *buf );                     /* 0x00435410 */
    virtual bool        FromScript( char **text );                              /* 0x004354d0 */
    virtual std::string ToString( void );                                       /* 0x00435570 */

    int m_value;            /* +0x08 */
};


class ValueFloat : public Value
{
public:
    ValueFloat( Variable *variable );                                           /* 0x00435070 */

    virtual void        ToScript( int indent, buf_t *buf );                     /* 0x00435130 */
    virtual bool        FromScript( char **text );                              /* 0x00435200 */
    virtual std::string ToString( void );                                       /* 0x004352a0 */

    double m_value;         /* +0x08 */
};


class ValueColor : public Value
{
public:
    ValueColor( Variable *variable );                                           /* 0x00433de0 */

    virtual void        ToScript( int indent, buf_t *buf );                     /* 0x00433eb0 */
    virtual bool        FromScript( char **text );                              /* 0x00434080 */
    virtual std::string ToString( void );                                       /* 0x00434160 */

    float m_value[4];       /* +0x08 */
};


class ValueString : public Value
{
public:
    ValueString( Variable *variable );                                          /* 0x00436050 */

    virtual void        ToScript( int indent, buf_t *buf );                     /* 0x00436160 */
    virtual bool        FromScript( char **text );                              /* 0x00436260 */
    virtual std::string ToString( void );                                       /* 0x00436020 */

    std::string m_value;    /* +0x08 */
};


class ValueEnum : public Value
{
public:
    ValueEnum( Variable *variable );                                            /* 0x00434d40 */

    virtual void        ToScript( int indent, buf_t *buf );                     /* 0x00434e30 */
    virtual bool        FromScript( char **text );                              /* 0x00434fa0 */
    virtual std::string ToString( void );                                       /* 0x00434d10 */

    void SetValue( const std::string &name );                                   /* 0x00434b60 */

    std::string m_value;    /* +0x08 */
};


class ValueScope
{
public:
    ValueScope();                                                               /* 0x004393e0 */
    ~ValueScope();                                                              /* 0x00438f90 */

    Variable *Find( const std::string &name );                                  /* 0x00439b40 */
    Variable *AddVariable( const std::string &name, Type *type );               /* 0x00439800 */
    void      BuildIndex( void );                                               /* 0x00439780 */
    void      ResetIndex( void );                                               /* 0x00438bb0 */
    bool      FromScript( char **text );                                        /* 0x00438670 */
    void      ToScript( int indent, buf_t *buf );                               /* 0x00437ff0 */
    void      Finalize( void );                                                 /* 0x004369e0 */

    std::list< Variable * >               m_variables;
    std::map< std::string, Variable * >   m_index;
    std::list< Variable * >::iterator     m_rover;
    bool                                  m_indexed;
};


class ValueStruct : public Value
{
public:
    ValueStruct( Variable *variable );                                          /* 0x00436410 */

    virtual void ToScript( int indent, buf_t *buf );                            /* 0x00436300 */
    virtual bool FromScript( char **text );                                     /* 0x004365b0 */
    virtual void Finalize( void );                                              /* 0x00436390 */

    ValueScope m_scope;     /* +0x08 */
};


class ValueMap : public Value
{
public:
    ValueMap( Variable *variable, Type *valueType );                            /* 0x004355f0 */

    virtual void ToScript( int indent, buf_t *buf );                            /* 0x00435850 */
    virtual bool FromScript( char **text );                                     /* 0x00435b70 */
    virtual void Finalize( void );                                              /* 0x00435680 */

    Variable *AddEntry( const std::string &name );                              /* 0x004357d0 */

    Type      *m_valueType; /* +0x08 */
    ValueScope m_scope;     /* +0x0c */
};


class ValueData : public Value
{
public:
    ValueData( Variable *variable );                                            /* 0x00434330 */
    virtual ~ValueData();                                                       /* 0x00434400 */

    virtual void ToScript( int indent, buf_t *buf );                            /* 0x00434570 */
    virtual bool FromScript( char **text );                                     /* 0x004348d0 */

    void SetFileName( std::string fileName );                                   /* 0x00434740 */
    void LoadDefFile( void );                                                   /* 0x004342b0 */

    std::string m_sourceFileName;   /* +0x10 */
    ValueScope *m_scope;            /* +0x2c */
    std::string m_fileName;         /* +0x30 */
};


class Type
{
public:
    virtual ~Type();

    virtual Value      *CreateDefaultValue( Variable *variable ) = 0;
    virtual Variable   *CreateVariable( void );                                 /* 0x0042d640 */
    virtual TypeColor  *CastTypeColor( void );
    virtual TypeData   *CastTypeData( void );
    virtual TypeEnum   *CastTypeEnum( void );
    virtual TypeFloat  *CastTypeFloat( void );
    virtual TypeInt    *CastTypeInteger( void );
    virtual TypeMap    *CastTypeMap( void );
    virtual TypeString *CastTypeString( void );
    virtual TypeStruct *CastTypeStruct( void );
};


class TypeColor : public Type
{
public:
    TypeColor( float red, float green, float blue, float alpha );               /* 0x0042d750 */

    virtual Value     *CreateDefaultValue( Variable *variable );                /* 0x0042d8b0 */
    virtual TypeColor *CastTypeColor( void );

    float m_default[4];     /* +0x08 */
};


class TypeData : public Type
{
public:
    virtual Value    *CreateDefaultValue( Variable *variable );                 /* 0x0042d970 */
    virtual TypeData *CastTypeData( void );
};


struct enumValue_t
{
    std::string m_name;     /* +0x00 */
    int         m_id;       /* +0x1c */
};


class TypeEnum : public Type
{
public:
    TypeEnum( const char **valueNames, const int *valueIds, int numValues );    /* 0x0042dcc0 */

    virtual Value    *CreateDefaultValue( Variable *variable );                 /* 0x0042da10 */
    virtual TypeEnum *CastTypeEnum( void );

    bool FindValue( const std::string &name, int *id );                         /* 0x0042daa0 */
    bool GetValueName( int id, std::string *name );                             /* 0x0042dec0 */

    std::vector< enumValue_t > m_values;
    int                        m_maxId;
};


class TypeFloat : public Type
{
public:
    TypeFloat( float defaultValue, float minValue, float maxValue );            /* 0x0042df10 */

    virtual Value     *CreateDefaultValue( Variable *variable );                /* 0x0042dfa0 */
    virtual TypeFloat *CastTypeFloat( void );

    float m_min;            /* +0x08 */
    float m_max;            /* +0x0c */
    float m_default;        /* +0x10 */
};


class TypeInt : public Type
{
public:
    TypeInt( int defaultValue, int minValue, int maxValue );                    /* 0x0042e040 */

    virtual Value   *CreateDefaultValue( Variable *variable );                  /* 0x0042e0c0 */
    virtual TypeInt *CastTypeInteger( void );

    int m_min;              /* +0x08 */
    int m_max;              /* +0x0c */
    int m_default;          /* +0x10 */
};


class TypeMap : public Type
{
public:
    TypeMap();                                                                  /* 0x0042e160 */
    TypeMap( Type *valueType );                                                 /* 0x0042e180 */

    virtual Value   *CreateDefaultValue( Variable *variable );                  /* 0x0042e1a0 */
    virtual TypeMap *CastTypeMap( void );

    Type *m_type;           /* +0x08 */
};


class TypeString : public Type
{
public:
    TypeString( std::string defaultValue );                                     /* 0x0042e660 */

    virtual Value      *CreateDefaultValue( Variable *variable );               /* 0x0042e5f0 */
    virtual TypeString *CastTypeString( void );

    std::string m_default;  /* +0x08 */
};


class TypeStruct : public Type
{
public:
    virtual Value      *CreateDefaultValue( Variable *variable );               /* 0x0042e730 */
    virtual TypeStruct *CastTypeStruct( void );

    std::map< std::string, Type * > m_members;  /* +0x08 */
};


class Variable
{
public:
    Variable();                                                                 /* 0x00437300 */
    virtual ~Variable();                                                        /* 0x00437340 */

    void ToScript( int indent, buf_t *buf );                                    /* 0x00437ed0 */
    bool FromScript( char **text );                                             /* 0x004368d0 */
    void CreateValue( void );                                                   /* 0x00436870 */
    void SetModified( void );                                                   /* 0x00439320 */

    std::string m_name;     /* +0x08 */
    Type       *m_type;     /* +0x24 */
    Value      *m_value;    /* +0x28 */
};


class ObjectInfo
{
public:
    ObjectInfo();                                                               /* 0x004324f0 */

    std::string                m_FileName;              /*              +0x00 */
    std::string                m_Name;                  /* #name        +0x1c */
    std::vector< std::string > m_Dll;                   /* #dll         +0x38 */
    std::vector< bool >        m_DllIsAll;              /*              +0x48 */
    std::string                m_SourceDirectory;       /* #source_dir  +0x5c */
    std::string                m_FileExt;               /* #file_ext    +0x78 */
    bool                       m_HasFileExt;            /*              +0x94 */
    int                        m_Version;               /* #version     +0x98 */
    std::vector< std::string > m_TargetDirectory;       /* #target_dir  +0x9c */
};


typedef union parseVal_u parseVal_t;
typedef struct parseListItem_s parseListItem_t;
typedef struct parseList_s parseList_t;

union parseVal_u
{
    int          type;
    int          integer;
    float        number;
    char        *string;
    parseVal_t  *node;
    parseList_t *list;
    Type        *typeDef;
};

struct parseListItem_s
{
    parseVal_t       value;
    parseListItem_t *next;
};

struct parseList_s
{
    parseListItem_t *head;
    parseListItem_t *tail;
};

enum
{
    ENUM_main               = 1,
    ENUM_enum               = 18,
    ENUM_int                = 19,
    ENUM_float              = 20,
    ENUM_color              = 21,
    ENUM_float_value        = 22,
    ENUM_int_value          = 23,
    ENUM_struct             = 24,
    ENUM_map                = 25,
    ENUM_simple_variable    = 26,
    ENUM_variable_exec      = 29,
    ENUM_variable           = 32,
    ENUM_string             = 34,
    ENUM_string_default     = 35,
    ENUM_variable_param     = 43,
    ENUM_data               = 44,
    ENUM_keyenum            = 48
};


typedef std::map< std::string, ObjectInfo * > DefFileMap_t;

extern DefFileMap_t g_defFiles;                                                 /* 0x004f5160 */

void         GetDefFile( std::string fileName );                                /* 0x00432bf0 */
parseVal_t  *GetParseData( const std::string &fileName );                       /* 0x00426b60 */
bool         EmitDef( ValueData *data );                                        /* 0x00426e20 */
void         Grammar_Error( int pos, const char *fmt, ... );                    /* 0x004227e0 */
bool         Grammar_ParseText( const char *text, parseVal_t *root );           /* 0x0041fcc0 */
parseVal_t   Grammar_Int( int value );                                          /* 0x00426f60 */
parseVal_t   Grammar_Node1( int type );                                         /* 0x00426f80 */
parseVal_t   Grammar_Node2( int type, parseVal_t a );                           /* 0x00426fa0 */
parseVal_t   Grammar_Node3( int type, parseVal_t a, parseVal_t b );             /* 0x00426fc0 */
parseVal_t   Grammar_Node4( int type, parseVal_t a, parseVal_t b, parseVal_t c );   /* 0x00426fe0 */
parseVal_t   Grammar_Node5( int type, parseVal_t a, parseVal_t b, parseVal_t c,
                            parseVal_t d );                                     /* 0x00427030 */
parseVal_t   Grammar_Node6( int type, parseVal_t a, parseVal_t b, parseVal_t c,
                            parseVal_t d, parseVal_t e );                       /* 0x00427060 */
parseVal_t   Grammar_Node7( int type, parseVal_t a, parseVal_t b, parseVal_t c,
                            parseVal_t d, parseVal_t e, parseVal_t f );         /* 0x004270a0 */
parseVal_t   Grammar_Node8( int type, parseVal_t a, parseVal_t b, parseVal_t c,
                            parseVal_t d, parseVal_t e, parseVal_t f,
                            parseVal_t g );                                     /* 0x004270e0 */
parseVal_t   Grammar_ListCreate( parseVal_t value );                            /* 0x00427130 */
parseVal_t   Grammar_ListAppend( parseVal_t list, parseVal_t value );           /* 0x00427180 */
void         GetParseErrorLine( const char *text, const char *pos, int *line,
                                std::string *lineText, std::string *marker );   /* 0x004248c0 */

#endif
