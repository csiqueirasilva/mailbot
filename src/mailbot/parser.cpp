#include "parser.hpp"

namespace mailbot {

    Parser * Parser::pInstance = NULL ;

    int Parser::checkLog ( void )
    {
        if ( stat( "/var/log/mail-bot" , &buf ) != 0 ) //If directory don`t exist
        {
            if ( mkdir( "/var/log/mail-bot" , S_IRWXU | S_IRWXG | S_IRWXO ) != 0 ) // 0777
            {
                std::cout << "Error creating log directory: " << strerror ( errno ) << std::endl ;
                return 0 ;
            }// IF

        }// IF

        return 1 ;

    }// End of function checkLog

};
