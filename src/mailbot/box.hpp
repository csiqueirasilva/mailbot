#ifndef MAILBOT_BOX
#define MAILBOT_BOX

#include <string>

namespace mailbot {

class Box {

private:
    std::string * name ;
    std::string * mail ;

public:
    std::string * getName ( void ) ;
    std::string * getMail ( void ) ;

    Box ( const char * name, const char * mail ) ;
    ~Box ( void ) ;
};

}

#endif
