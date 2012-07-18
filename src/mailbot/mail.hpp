#include "attachment.hpp"
#include "base.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

namespace mailbot {

class Mail {

private:
    static Mail * pInstance ;
    std::string * body ;
    std::list<std::string *> * cc ;
    std::string * to ;
    std::string * messageId ;
    std::string * inReplyTo ;
    std::string * deliveredTo ;
    std::list<Attachment *> * attachments ;
    std::string * subject ;
    std::string * userAgent ;
    std::string * bcc ;
    std::string * organization ;
    boost::gregorian::date * date ;
    void parseFromFile ( const char * fname ) ;
    void freeAll ( void ) ;
    Mail ( char * fname ) ;
    ~Mail ( void ) ;
public:
    static Mail * New ( char * fname )
    {
        if ( pInstance == NULL )
        {
            pInstance = new Mail ( fname ) ;
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

}
