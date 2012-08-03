#include "src/mailbot.hpp"

int main (int argc, char ** argv)
{

    mailbot::Mail * mail = mailbot::Mail::New(argc > 1 ? static_cast<const char *>(argv[1]) : "/var/dev/mail-bot/samples/1.eml") ;

    mail->Print();

    mail->Delete();

    return 1 ;
}
