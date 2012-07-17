#ifndef MAILBOT_ATTACHMENT
#define MAILBOT_ATTACHMENT

#include "mailbot/base.hpp"

namespace mailbot {

class Attachment {

private:
    std::string * filename ;
    std::string * filepath ;

public:
    std::string * getName ( void ) ;
    std::string * getPath ( void ) ;
    Attachment ( char * fname, char * fpath ) ;
    ~Attachment ( void ) ;
};

}

#endif
