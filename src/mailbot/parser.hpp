#ifndef MAILBOT_PARSER
#define MAILBOT_PARSER
#include "mail.hpp"
#include "base.hpp"
#include <lua.hpp>

namespace mailbot {

class Parser {

private:

    static Parser * pInstance ;
    static Mail * mail ;

    void logMessage ( const char * msg ) ;
    static int lua_pushEmptyString ( lua_State * L , std::string * str ) ;
    static int lua_pushBox ( lua_State * L , Box * box ) ;
    static int lua_pushStlString ( lua_State * L , std::string * str ) ;
    static int lua_pushListOfStrings ( lua_State * L , std::list<std::string *> * lst ) ;
    static int lua_pushListOfBoxes ( lua_State * L , std::list<Box *> * lst ) ;
    void setLuaFunctions ( lua_State * L ) ;
    static int checkLog ( void ) ;
    Parser ( const char * fname ) ;
    ~Parser ( void ) ;

public:

    void runPlugins ( void ) ;

    static int lua_getBody ( lua_State * L ) ;
    static int lua_getCc ( lua_State * L ) ;
    static int lua_getTo ( lua_State * L ) ;
    static int lua_getMessageId ( lua_State * L ) ;
    static int lua_getOriginalMessageId ( lua_State * L ) ;
    static int lua_getReplyTo ( lua_State * L ) ;
    static int lua_getInReplyTo ( lua_State * L ) ;
    static int lua_getReferences ( lua_State * L ) ;
    static int lua_getDeliveredTo ( lua_State * L ) ;
    static int lua_getAttachments ( lua_State * L ) ;
    static int lua_getSubject ( lua_State * L ) ;
    static int lua_getUserAgent ( lua_State * L ) ;
    static int lua_getBcc ( lua_State * L ) ;
    static int lua_getFrom ( lua_State * L ) ;
    static int lua_getSender ( lua_State * L ) ;
    static int lua_getOrganization ( lua_State * L ) ;
    static int lua_getDate ( lua_State * L ) ;

    static Parser * New ( const char * fname )
    {
        if ( pInstance == NULL )
        {
            pInstance = new Parser ( fname ) ;
        }// IF

        return pInstance ;

    };// Singleton Constructor

    static void Delete ( void )
    {
        if ( pInstance != NULL )
        {
            delete pInstance ;
            pInstance = NULL ;
        }// IF

    };// Singleton Destructor
};

};

#endif
