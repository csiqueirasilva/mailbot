#ifndef MAILBOT_MAIL
#define MAILBOT_MAIL

#include "attachment.hpp"
#include "box.hpp"
#include "base.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>

#define DEFAULT_ATTACHMENT_FOLDER "/tmp/"

namespace mailbot {

class Mail {

private:
    static Mail * pInstance ;
    std::ofstream * log ;
    std::string * body ;
    std::list<Box *> * cc ;
    std::list<Box *> * to ;
    std::string * messageId ;
    std::string * originalMessageId ;
    std::string * ReplyTo ;
    std::list<std::string *> * inReplyTo ;
    std::list<std::string *> * references ;
    std::string * deliveredTo ;
    std::list<Attachment *> * attachments ;
    std::string * subject ;
    std::string * userAgent ;
    std::list<Box *> * bcc ;
    Box * from ;
    Box * sender ;
    std::string * organization ;
    boost::posix_time::ptime * date ;
    vmime::charset * chset ;
    void parseFromFile ( const char * fname ) ;
    void parseHeader ( vmime::ref<vmime::message> msg ) ;
    void parseBody ( vmime::ref<vmime::message> msg ) ;
    int checkLog ( void ) ;
    Mail ( const char * fname ) ;
    ~Mail ( void ) ;
public:

    /* GET */
    std::string * getBody ( void ) ;
    std::list<Box *> * getCc ( void ) ;
    std::list<Box *> * getTo ( void ) ;
    std::string * getMessageId ( void ) ;
    std::string * getOriginalMessageId ( void ) ;
    std::string * getReplyTo ( void ) ;
    std::list<std::string *> * getInReplyTo ( void ) ;
    std::list<std::string *> * getReferences ( void ) ;
    std::string * getDeliveredTo ( void ) ;
    std::list<Attachment *> * getAttachments ( void ) ;
    std::string * getSubject ( void ) ;
    std::string * getUserAgent ( void ) ;
    std::list<Box *> * getBcc ( void ) ;
    Box * getFrom ( void ) ;
    Box * getSender ( void ) ;
    std::string * getOrganization ( void ) ;
    boost::posix_time::ptime * getDate ( void ) ;

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

};

#endif
