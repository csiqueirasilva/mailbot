#include "parser.hpp"
#include <sys/stat.h>
#include <cerrno>
#include <dirent.h>
#include <ctime>
#include <cstring>
#include <sstream>

namespace mailbot {

    std::ofstream * mainLog ;

    Mail * Parser::mail = NULL ;

    Parser * Parser::pInstance = NULL ;

    Parser::Parser ( const char * fname )
    {
        checkLog();

        mail = mailbot::Mail::New( fname ) ;

        if ( !mail )
        {
            this->logMessage ( "Could not allocate memory to parse the mail file.") ;
            exit(EXIT_FAILURE) ;
        }//IF

    }// End of Class Constructor

    Parser::~Parser ( void )
    {
        mail->Delete() ;
    }// End of Class Destructor

    void Parser::logMessage ( const char * msg )
    {
        time_t tmp = time(NULL) ;
        std::string buf(ctime(&tmp)) ;
        *mainLog << "[" << buf.replace(buf.find('\n'),buf.find('\n'),"") << "] " << msg << std::endl ;
    }// End of functin logMessage

    void Parser::runPlugins ( void )
    {
        DIR * dir ;
        struct dirent * drnt ;

        if ( ! ( dir = opendir ( "/var/dev/mail-bot/plugins" ) ) )
        {
           this->logMessage("Could not open the plugins directory: ") ;
           return ;
        }// IF

        do // WHILE
        {
            drnt = readdir ( dir ) ;
            if ( !drnt ) break ;
            if ( strcmp( "." , drnt->d_name ) == 0 || strcmp( ".." , drnt->d_name ) == 0 ) continue ;

            struct stat buf ;

            std::string pluginFilePath = "/var/dev/mail-bot/plugins/" + std::string(drnt->d_name) + "/plugin.lua" ;
            std::string configFilePath = "/var/dev/mail-bot/plugins/" + std::string(drnt->d_name) + "/config.lua" ;

            if ( stat( pluginFilePath.c_str(), &buf ) ) // This was added since my office server couldn't return the type of the file properly. drnt->d_type was always a DT_UNKNOWN.
                                                            // I believe this happened due to the version of the file system.
                                                            // Reference: http://www.kernel.org/doc/man-pages/online/pages/man3/readdir.3.html
            {
                this->logMessage( std::string( "Plugin " + std::string(drnt->d_name) + " does not contain a plugin.lua file on the root or is not a directory." ).c_str() ) ;
            }//IF

            //If everything went OK, start running the lua plugin.

            lua_State *L = lua_open() ;

            luaL_openlibs( L ) ;

            lua_pushstring( L , std::string("/var/dev/mail-bot/plugins/" + std::string(drnt->d_name)).c_str() ) ;
            lua_setglobal( L , "PLUGIN_PATH" ) ;

            if ( !stat( configFilePath.c_str(), &buf ) )
            {

                if ( luaL_dofile( L, configFilePath.c_str() ) )
                {
                    this->logMessage( std::string(std::string("Plugin ") + std::string(drnt->d_name) + " configuration file returned: " + std::string(lua_tostring( L, -1 ))).c_str() ) ;
                }// IF

            }// IF

            this->setLuaFunctions( L ) ;

            if ( luaL_dofile( L, pluginFilePath.c_str() ) )
            {
                this->logMessage( std::string(std::string("Plugin ") + std::string(drnt->d_name) + " returned: " + std::string(lua_tostring( L, -1 ))).c_str() ) ;
            }// IF

            lua_close( L ) ;

        } while ( true ) ;

        closedir( dir ) ;

    }// End of function runPlugins

    void Parser::setLuaFunctions ( lua_State * L )
    {
        lua_newtable( L ) ;

        lua_pushstring ( L , "getBody" ) ;
        lua_pushcfunction ( L , lua_getBody ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getCc" ) ;
        lua_pushcfunction ( L , lua_getCc ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getTo" ) ;
        lua_pushcfunction ( L , lua_getTo ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getMessageId" ) ;
        lua_pushcfunction ( L , lua_getMessageId ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getOriginalMessageId" ) ;
        lua_pushcfunction ( L , lua_getOriginalMessageId ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getReplyTo" ) ;
        lua_pushcfunction ( L , lua_getReplyTo ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getInReplyTo" ) ;
        lua_pushcfunction ( L , lua_getInReplyTo ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getReferences" ) ;
        lua_pushcfunction ( L , lua_getReferences ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getDeliveredTo" ) ;
        lua_pushcfunction ( L , lua_getDeliveredTo ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getAttachments" ) ;
        lua_pushcfunction ( L , lua_getAttachments ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getSubject" ) ;
        lua_pushcfunction ( L , lua_getSubject ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getUserAgent" ) ;
        lua_pushcfunction ( L , lua_getUserAgent ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getBcc" ) ;
        lua_pushcfunction ( L , lua_getBcc ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getFrom" ) ;
        lua_pushcfunction ( L , lua_getFrom ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getSender" ) ;
        lua_pushcfunction ( L , lua_getSender ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getOrganization" ) ;
        lua_pushcfunction ( L , lua_getOrganization ) ;
        lua_rawset ( L , -3 ) ;

        lua_pushstring ( L , "getDate" ) ;
        lua_pushcfunction ( L , lua_getDate ) ;
        lua_rawset ( L , -3 ) ;

        lua_setglobal( L , "mailbot" ) ;

    }// End of setLuaFunctions

    int Parser::lua_getBody ( lua_State * L )
    {
        return lua_pushEmptyString( L , mail->getBody() ) ;
    }// End of lua_getBody

    int Parser::lua_pushEmptyString ( lua_State * L , std::string * str )
    {
        if ( str->size() != 0 )
        {
            lua_pushstring( L , str->c_str() ) ;
        }//IF
        else
        {
            lua_pushnil( L ) ;
        }//ELSE

        return 1 ;
    }//End of lua_pushEmptyString

    int Parser::lua_getCc ( lua_State * L )
    {
        return lua_pushListOfBoxes( L , mail->getCc() ) ;
    }// End of lua_getCc

    // Is there a way to template the following two functions into one?

    int Parser::lua_pushListOfBoxes ( lua_State * L , std::list<Box *> * lst )
    {
        if ( !lst )
        {
            lua_pushnil( L ) ;
        }//IF
        else
        {
            int i = 1 ;
            lua_newtable( L ) ;

            for( std::list<Box *>::iterator it = lst->begin() ; it != lst->end() ; it++ , i++ )
            {
                lua_pushnumber( L , i ) ;
                lua_newtable( L ) ;

                lua_pushstring( L , "name" ) ;
                lua_pushstring( L , (*it)->getName()->c_str() ) ;
                lua_rawset( L , -3 ) ;

                lua_pushstring( L , "mail" ) ;
                lua_pushstring( L , (*it)->getMail()->c_str() ) ;
                lua_rawset( L , -3 ) ;

                lua_rawset( L , -3 ) ;

            }// FOR
        }//ELSE

        return 1 ;
    }// End of lua_pushListOfBoxes

    int Parser::lua_pushListOfStrings ( lua_State * L , std::list<std::string *> * lst )
    {
        if ( !lst )
        {
            lua_pushnil( L ) ;
        }//IF
        else
        {
            int i = 1 ;
            lua_newtable( L ) ;

            for( std::list<std::string *>::iterator it = lst->begin() ; it != lst->end() ; it++ , i++ )
            {

                lua_pushnumber( L , i ) ;
                lua_pushstring( L , (*it)->c_str() ) ;
                lua_rawset( L , -3 ) ;

            }// FOR
        }//ELSE

        return 1 ;
    }// End of lua_pushListOfStrings

    int Parser::lua_pushStlString ( lua_State * L , std::string * str )
    {
        if ( str )
        {
            lua_pushstring( L , str->c_str() ) ;
        }// IF
        else
        {
            lua_pushnil( L ) ;
        }//ELSE

        return 1 ;
    }// End of lua_pushStlString

    int Parser::lua_getTo ( lua_State * L )
    {
        return lua_pushListOfBoxes( L , mail->getTo() ) ;
    }// End of lua_getTo

    int Parser::lua_getMessageId ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getMessageId() ) ;
    }// End of lua_getMessageId

    int Parser::lua_getOriginalMessageId ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getOriginalMessageId() ) ;
    }// End of lua_getOriginalMessageId

    int Parser::lua_getReplyTo ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getReplyTo() ) ;
    }// End of lua_getReplyTo

    int Parser::lua_getInReplyTo ( lua_State * L )
    {
        return lua_pushListOfStrings ( L , mail->getInReplyTo() ) ;
    }// End of lua_getInReplyTo

    int Parser::lua_getReferences ( lua_State * L )
    {
        return lua_pushListOfStrings ( L , mail->getReferences() ) ;
    }// End of lua_getReferences

    int Parser::lua_getDeliveredTo ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getDeliveredTo() ) ;
    }// End of lua_getDeliveredTo

    int Parser::lua_getAttachments ( lua_State * L )
    {
        if ( !mail->getAttachments() )
        {
            lua_pushnil ( L ) ;
        }//IF
        else
        {

            int i = 1 ;
            lua_newtable( L ) ;

            for( std::list<Attachment *>::iterator it = mail->getAttachments()->begin() ; it != mail->getAttachments()->end() ; it++ , i++ )
            {

                lua_pushnumber( L , i ) ;
                lua_newtable( L ) ;

                lua_pushstring( L , "name" ) ;
                lua_pushstring( L , (*it)->getName()->c_str() ) ;
                lua_rawset( L , -3 ) ;

                lua_pushstring( L , "path" ) ;
                lua_pushstring( L , (*it)->getPath()->c_str() ) ;
                lua_rawset( L , -3 ) ;

                lua_pushstring( L , "type" ) ;
                lua_pushstring( L , (*it)->getType()->c_str() ) ;
                lua_rawset( L , -3 ) ;

                lua_rawset( L , -3 ) ;

            }// FOR
        }//ELSE

        return 1 ;
    }// End of lua_getAttachments

    int Parser::lua_pushBox ( lua_State * L , Box * box )
    {
        if( box )
        {
            lua_newtable( L ) ;

            lua_pushstring( L , "name" ) ;
            lua_pushstring( L , box->getName()->c_str() ) ;
            lua_rawset( L , -3 ) ;

            lua_pushstring( L , "mail" ) ;
            lua_pushstring( L , box->getMail()->c_str() ) ;
            lua_rawset( L , -3 ) ;

        }// IF
        else
        {
            lua_pushnil( L ) ;
        }//ELSE

        return 1 ;
    }// End of lua_getAttachments

    int Parser::lua_getSubject ( lua_State * L )
    {
        return lua_pushEmptyString ( L , mail->getSubject() ) ;
    }// End of lua_getSubject

    int Parser::lua_getUserAgent ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getUserAgent() ) ;
    }// End of lua_getUserAgent

    int Parser::lua_getBcc ( lua_State * L )
    {
        return lua_pushListOfBoxes( L , mail->getBcc() ) ;
    }// End of lua_getBcc

    int Parser::lua_getFrom ( lua_State * L )
    {
        return lua_pushBox ( L , mail->getFrom() ) ;
    }// End of lua_getFrom

    int Parser::lua_getSender ( lua_State * L )
    {
        return lua_pushBox ( L , mail->getSender() ) ;
    }// End of lua_getSender

    int Parser::lua_getOrganization ( lua_State * L )
    {
        return lua_pushStlString ( L , mail->getOrganization() ) ;
    }// End of lua_getOrganization

    int Parser::lua_getDate ( lua_State * L )
    {
        if( mail->getDate() )
        {
            lua_newtable( L ) ;

            lua_pushstring( L , "date" ) ;

            std::stringstream buf ;
            std::string str("") ;

            buf << mail->getDate()->date() ;
            buf >> str ;

            lua_pushstring( L , str.c_str() ) ;
            lua_rawset( L , -3 ) ;

            buf.flush() ;
            str = "" ;

            buf << mail->getDate()->time_of_day() ;
            buf >> str ;

            lua_pushstring( L , "time" ) ;
            lua_pushstring( L , str.c_str() ) ;
            lua_rawset( L , -3 ) ;

        }// IF
        else
        {
            lua_pushnil ( L ) ;
        }//ELSE

        return 1 ;
    }// End of lua_getDate

    int Parser::checkLog ( void )
    {
        struct stat buf ;

        if ( stat( "/var/log/mail-bot" , &buf ) != 0 ) //If directory don`t exist
        {
            if ( mkdir( "/var/log/mail-bot" , S_IRWXU | S_IRWXG | S_IRWXO ) != 0 ) // 0777
            {
                std::cout << "Error creating log directory: " << strerror ( errno ) << std::endl ;
                return 0 ;
            }// IF

        }// IF

        mainLog = new std::ofstream("/var/log/mail-bot/main.log", std::ios::out | std::ios::app ) ;

        return 1 ;

    }// End of function checkLog

};
