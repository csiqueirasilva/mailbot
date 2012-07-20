#include "src/mailbot.hpp"

int main (void)
{

    mailbot::Mail * mail = mailbot::Mail::New("/var/dev/mail-bot/samples/5.eml") ;

    mail->Print();

    mail->Delete();

    return 1 ;
}
