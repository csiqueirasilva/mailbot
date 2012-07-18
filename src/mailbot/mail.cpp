#include "mail.hpp"

namespace mailbot {

    Mail * Mail::pInstance = NULL ;

    Mail::Mail ( const char * fname )
    {
        parseFromFile ( fname ) ;
    }// Class Constructor

    Mail::~Mail ( void )
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

    }// Class Destructor

    void Mail::parseHeader ( vmime::ref<vmime::message> msg )
    {
        //Start parsing the header

        vmime::ref<const vmime::header> hdr = msg->getHeader() ;

        // get date
        try
        {
            throw hdr->Date()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->date = NULL ;
        }// catch
        catch ( std::string e )
        {
            // Convert the date of the message to UTC
            vmime::datetime dt = vmime::utility::datetimeUtils::toUniversalTime( *hdr->Date()->getValue().dynamicCast<const vmime::datetime>() ) ;

            this->date = new boost::posix_time::ptime(boost::gregorian::date(dt.getYear(),dt.getMonth(),dt.getDay()),
                boost::posix_time::hours(dt.getHour())+boost::posix_time::minutes(dt.getMinute())+ boost::posix_time::seconds(dt.getSecond())
            );
        }// catch

//    std::list<std::string *> * cc ;
//    std::string * to ;
//    std::list<Attachment *> * attachments ;

        // get userAgent
        try
        {
            throw hdr->UserAgent()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->userAgent = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->userAgent = new std::string(e);
        }// catch

        // get userAgent
        try
        {
            throw hdr->UserAgent()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->userAgent = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->userAgent = new std::string(e);
        }// catch

        // get Bcc
        try
        {
            throw hdr->Bcc()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->bcc = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->bcc = new std::string(e);
        }// catch

        // get organization
        try
        {
            throw hdr->Organization()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->organization = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->organization = new std::string(e);
        }// catch

        // get subject
        try
        {
            throw hdr->Subject()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->subject = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->subject = new std::string(e);
        }// catch

        // get messageId
        try
        {
            throw hdr->MessageId()->getValue()->generate() ;
        } // try
        catch ( vmime::exception e )
        {
            this->messageId = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->messageId = new std::string(e) ;
        }// catch

        // get deliveredTo
        try
        {
            throw hdr->DeliveredTo()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
            this->deliveredTo = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->deliveredTo = new std::string(e) ;
        }// catch

        // get inReplyTo
        try
        {
            throw hdr->InReplyTo()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
			this->inReplyTo = NULL ;
        }// catch
        catch ( std::string e )
        {
			this->inReplyTo = new std::string(e) ;
        }// catch

    }// Function parseHeader

    void Mail::parseBody ( vmime::ref<vmime::message> msg )
    {
        //Set the body output into the body variable

        vmime::utility::outputStreamStringAdapter out(*(this->body));

        vmime::messageParser mp(msg);

        vmime::ref<const vmime::contentHandler> cts;

        //Get the first available body part

        vmime::ref<const vmime::textPart> tp = mp.getTextPartAt(0) ;

        //If it is HTML, get the plain text
        if(tp->getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
        {

            vmime::ref <const vmime::htmlTextPart> htp =
            tp.dynamicCast <const vmime::htmlTextPart>() ;

            cts = htp->getPlainText();

        }/* IF */
        else // Or get just get what it is there
        {

            cts = tp->getText();

        }/* ELSE */

        //Finish the output into the body variable
        cts->extract(out) ;
        out.flush();

    }// Function parseBody

    void Mail::parseFromFile ( const char * fname )
    {
        this->body = new std::string();

        //Get file as stream
        std::ifstream eml( fname , std::ios::in ) ;

        if ( !eml )
        {
            std::cerr << "Erro ao abrir o arquivo de email " << fname << std::endl;
            delete pInstance ;
            exit(EXIT_FAILURE);
        }// IF

        //Vmime initialization pipeline

        vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

        vmime::charset ch(vmime::charsets::UTF_8);

        vmime::ref<vmime::message> msg = vmime::create <vmime::message>();

        vmime::utility::inputStreamAdapter is(eml);

        vmime::string data;

        vmime::utility::outputStreamStringAdapter os(data);

        vmime::utility::bufferedStreamCopy(is, os);

        msg->parse(data);

        //End of Vmime initialization pipeline

        this->parseBody(msg);

        this->parseHeader(msg);

    }// Function parseFromFile

    void Mail::Print ( void )
    {
        std::cout << "body: " ;

        if ( this->body != NULL )
        {
            std::cout << this->body->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// else

        std::cout << std::endl ;

        std::cout << "subject: " ;

        if ( this->subject != NULL )
        {
            std::cout << this->subject->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// else

        std::cout << std::endl ;

        std::cout << "message-id: " ;

        if ( this->messageId != NULL )
        {
            std::cout << this->messageId->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// else

        std::cout << std::endl ;

        std::cout << "user-agent: " ;

        if ( this->userAgent != NULL )
        {
            std::cout << this->userAgent->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// else

        std::cout << std::endl ;

    }// Function Print

}// MAILBOT
