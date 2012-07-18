#include <cstdio>
#include "src/mailbot.hpp"

int main (void)
{
    mailbot::Mail * mail = mailbot::Mail::New("z:\\mail-bot\\samples\\1.eml") ;

    mail->Delete();

    return 1 ;
}
