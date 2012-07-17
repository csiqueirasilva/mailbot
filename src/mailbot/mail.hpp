#include "mailbot/attachment.hpp"
#include "boost/date_time.hpp"

namespace mailbot {

class Mail {

private:
    static Mail * pInstance ;
    std::string * header ;
    std::string * body ;
    std::list<std::string *> * cc ;
    std::string * to ;
    std::string * messageId ;
    std::string * inReplyTo ;
    std::string * deliveredTo ;
    std::list<Attachment *> attachments ;
    std::string * subject ;
    std::string * userAgent ;
    std::string * bcc ;
    std::string * organization ;
    boost::date_time * date ;
    void parseFromFile ( char * fname ) ;
    void freeAll ( void ) ;
    static Mail ( char * fname ) { parseFromFile ( fname ) ; } ;
    static ~Mail ( void ) { } ;
public:


};

}
