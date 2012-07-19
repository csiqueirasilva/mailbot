#ifndef MAILBOT_ATTACHMENT
#define MAILBOT_ATTACHMENT

#include <string>

namespace mailbot {

class Attachment {

private:
    std::string * filename ;
    std::string * filepath ;
    std::string * filetype ;

public:
    std::string * getName ( void ) ;
    std::string * getPath ( void ) ;
    std::string * getType ( void ) ;

    Attachment ( const char * fname, const char * fpath, const char * ftype ) ;
    ~Attachment ( void ) ;
};

}

#endif
