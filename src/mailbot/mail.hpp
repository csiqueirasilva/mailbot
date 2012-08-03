#include "attachment.hpp"
#include "box.hpp"
#include "base.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>

#define DEFAULT_ATTACHMENT_FOLDER "/tmp/"

namespace mailbot {

class Parser::Mail {

private:
    static Parser::Mail * pInstance ;
    std::ifstream log ;
    std::string * body ;
    std::list<Box *> * cc ;
    std::list<Box *> * to ;
    std::string * messageId ;
    std::string * inReplyTo ;
    std::string * deliveredTo ;
    std::list<Attachment *> * attachments ;
    std::string * subject ;
    std::string * userAgent ;
    std::list<Box *> * bcc ;
    Box * from ;
    Box * sender ;
    std::string * organization ;
    boost::posix_time::ptime * date ;
    void parseFromFile ( const char * fname ) ;
    void parseHeader ( vmime::ref<vmime::message> msg ) ;
    void parseBody ( vmime::ref<vmime::message> msg ) ;
    int checkLog ( void ) ;
    Parser::Mail ( const char * fname ) ;
    ~Parser::Mail ( void ) ;
public:
    static Parser::Mail * New ( const char * fname )
    {
        if ( pInstance == NULL )
        {
            pInstance = new Parser::Mail ( fname ) ;
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
