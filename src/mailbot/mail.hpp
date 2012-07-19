#include "attachment.hpp"
#include "base.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>

#define DEFAULT_ATTACHMENT_FOLDER "/tmp/"

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
    boost::posix_time::ptime * date ;
    void parseFromFile ( const char * fname ) ;
    void parseHeader ( vmime::ref<vmime::message> msg ) ;
    void parseBody ( vmime::ref<vmime::message> msg ) ;
    Mail ( const char * fname ) ;
    ~Mail ( void ) ;
public:
    static Mail * New ( const char * fname )
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

    void Print ( void ) ;

};

}
