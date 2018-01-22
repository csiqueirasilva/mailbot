#include "src/mailbot.hpp"
#include <iostream>

int main (int argc, char ** argv)
{
    mailbot::Parser * parser = mailbot::Parser::New( argc > 1 ? static_cast<const char *>(argv[1]) : "/var/dev/mail-bot/samples/5.eml" ) ;

    parser->runPlugins() ;

    parser->Delete() ;

    return 0;
}
