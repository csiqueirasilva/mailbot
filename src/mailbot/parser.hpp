#include "mail.hpp"
#include "base.hpp"

namespace mailbot {

class Parser {

private:
    static Parser * pInstance ;

    int checkLog ( void ) ;
    Parser ( const char * fname ) ;
    ~Parser ( void ) ;

public:
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
