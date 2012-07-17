#include "mail.hpp"

namespace mailbot {

    Mail * Mail::pInstance = NULL ;

    Mail::Mail ( char * fname )
    {
        parseFromFile ( fname ) ;
    }// Class Constructor

    Mail::~Mail ( void )
    {
        freeAll() ;
    }// Class Destructor

    void Mail::parseFromFile ( const char * fname )
    {
        std::ifstream eml( fname , std::ios::in ) ;

        if ( !eml )
        {
            std::cerr << "Erro ao abrir o arquivo de email " << fname << std::endl;
            exit(EXIT_FAILURE);
        }/* IF */

        vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

        vmime::charset ch(vmime::charsets::UTF_8);

        vmime::ref<vmime::message> msg = vmime::create <vmime::message>();

        vmime::utility::inputStreamAdapter is(eml);

        vmime::string data;

        vmime::utility::outputStreamStringAdapter os(data);

        vmime::utility::bufferedStreamCopy(is, os);

        msg->parse(data);

        this->body = new std::string();

        vmime::utility::outputStreamStringAdapter out(*(this->body));

        vmime::messageParser mp(msg);

        vmime::ref<const vmime::contentHandler> cts;

        vmime::ref<const vmime::textPart> tp = mp.getTextPartAt(0) ;

        if(tp->getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
        {

            vmime::ref <const vmime::htmlTextPart> htp =
            tp.dynamicCast <const vmime::htmlTextPart>() ;

            cts = htp->getPlainText();

        }/* IF */
        else
        {

            cts = tp->getText();

        }/* ELSE */

        cts->extract(out) ;
        out.flush();

        vmime::ref<const vmime::header> hdr = msg->getHeader() ;

        //headerDate = hdr->Date()->getValue()->generate() ;
        this->messageId = new std::string (hdr->MessageId()->getValue()->generate().c_str());

        std::cout << this->messageId << std::endl ;

        this->deliveredTo = new std::string (hdr->DeliveredTo()->getValue()->generate().c_str());

        try
        {
            throw hdr->InReplyTo()->getValue()->generate() ;
        }/* TRY */
        catch ( vmime::exception e )
        {
			this->inReplyTo = NULL ;
        }/* CATCH */
        catch ( std::string e )
        {
			this->inReplyTo = new std::string(e.c_str()) ;
        }/* CATCH */

    }// Function parseFromFile

    void Mail::freeAll ( void )
    {
        if ( this->body )
        {
            delete this->body ;
        }// IF

        if ( this->attachments != NULL )
        {
            for ( std::list<std::string *>::iterator it = this->cc->begin(); it != this->cc->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->cc ;

        }// IF

        if ( this->to )
        {
            delete this->to ;
        }// IF

        if ( this->messageId )
        {
            delete this->messageId ;
        }// IF

        if ( this->inReplyTo )
        {
            delete this->inReplyTo ;
        }// IF

        if ( this->deliveredTo )
        {
            delete this->deliveredTo ;
        }// IF

        if ( this->attachments )
        {
            for ( std::list<Attachment *>::iterator it = this->attachments->begin(); it != this->attachments->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->attachments ;

        }// IF

        if ( this->subject )
        {
            delete this->subject ;
        }// IF

        if ( this->userAgent )
        {
            delete this->userAgent ;
        }// IF

        if ( this->bcc )
        {
            delete this->bcc ;
        }// IF

        if ( this->organization )
        {
            delete this->organization ;
        }// IF

        if ( this->date )
        {
            delete this->date ;
        }// IF

    }// Function freeAll

}// MAILBOT
