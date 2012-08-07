#include "mail.hpp"
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <vector>

namespace mailbot {

    extern std::ofstream * mainLog ;

    Mail * Mail::pInstance = NULL ;

    Mail::Mail ( const char * fname )
    {
        checkLog ( ) ;
        parseFromFile ( fname ) ;
    }// Class Constructor

    Mail::~Mail ( void )
    {
        delete this->chset ;

        delete this->log ;

        delete this->body ;

        if ( this->cc != NULL )
        {
            for ( std::list<Box *>::iterator it = this->cc->begin() ; it != this->cc->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->cc ;

        }// IF

        if ( this->references != NULL )
        {
            for ( std::list<std::string *>::iterator it = this->references->begin() ; it != this->references->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->references ;

        }// IF

        if ( this->inReplyTo != NULL )
        {
            for ( std::list<std::string *>::iterator it = this->inReplyTo->begin() ; it != this->inReplyTo->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->inReplyTo ;

        }// IF

        if ( this->originalMessageId )
        {
            delete this->originalMessageId ;
        }// IF

        if ( this->messageId )
        {
            delete this->messageId ;
        }// IF

        if ( this->attachments != NULL )
        {
            for ( std::list<Attachment *>::iterator it = this->attachments->begin() ; it != this->attachments->end() ; it++ )
            {
                remove((*(*it)->getPath() + *(*it)->getName()).c_str());
                delete *it ;
            }// FOR

            delete this->attachments ;

        }// IF

        if ( this->ReplyTo )
        {
            delete this->ReplyTo ;
        }// IF

        if ( this->deliveredTo )
        {
            delete this->deliveredTo ;
        }// IF

        if ( this->subject )
        {
            delete this->subject ;
        }// IF

        if ( this->userAgent )
        {
            delete this->userAgent ;
        }// IF

        if ( this->bcc != NULL )
        {
            for ( std::list<Box *>::iterator it = this->bcc->begin() ; it != this->bcc->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->bcc ;

        }// IF

        if ( this->to != NULL )
        {
            for ( std::list<Box *>::iterator it = this->to->begin() ; it != this->to->end() ; it++ )
            {
                delete *it ;
            }// FOR

            delete this->to ;

        }// IF

        if ( this->organization )
        {
            delete this->organization ;
        }// IF

        if ( this->sender )
        {
            delete this->sender ;
        }// IF

        if ( this->from )
        {
            delete this->from ;
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

        // get cc
        try
        {
            throw hdr->Cc()->getValue().dynamicCast<const vmime::addressList>()->toMailboxList() ;
        }// try
        catch ( vmime::exception e )
        {
            this->cc = NULL ;
        }// catch
        catch ( vmime::utility::ref<vmime::mailboxList> e )
        {
            this->cc = new std::list<Box *>() ;
            for ( int i = 0 ; i < e->getMailboxCount() ; i++ )
            {
                this->cc->push_back(new Box(e->getMailboxAt(i)->getName().getConvertedText(*this->chset).c_str(), e->getMailboxAt(i)->getEmail().c_str()));
            }// for

        }// catch

        // get to
        try
        {
            throw hdr->To()->getValue().dynamicCast<const vmime::addressList>()->toMailboxList() ;
        }// try
        catch ( vmime::exception e )
        {
            this->to = NULL ;
        }// catch
        catch ( vmime::utility::ref<vmime::mailboxList> e )
        {
            this->to = new std::list<Box *>() ;
            for ( int i = 0 ; i < e->getMailboxCount() ; i++ )
            {
                this->to->push_back(new Box(e->getMailboxAt(i)->getName().getConvertedText(*this->chset).c_str(), e->getMailboxAt(i)->getEmail().c_str()));
            }// for

        }// catch

        // get references
        try
        {
            throw hdr->References()->getValue().dynamicCast<const vmime::messageIdSequence>()->getMessageIdList() ;
        }// try
        catch ( vmime::exception e )
        {
            this->references = NULL ;
        }// catch
        catch ( std::vector<vmime::utility::ref<vmime::messageId const> > e )
        {
            this->references = new std::list<std::string *>() ;
            for ( unsigned int i = 0 ; i < e.size() ; i++ )
            {
                this->references->push_back(new std::string(e.at(i)->getId()));
            }// for

        }// catch

        // get inReplyTo
        try
        {
            throw hdr->InReplyTo()->getValue().dynamicCast<const vmime::messageIdSequence>()->getMessageIdList() ;
        }// try
        catch ( vmime::exception e )
        {
            this->inReplyTo = NULL ;
        }// catch
        catch ( std::vector<vmime::utility::ref<vmime::messageId const> > e )
        {
            this->inReplyTo = new std::list<std::string *>() ;
            for ( unsigned int i = 0 ; i < e.size() ; i++ )
            {
                this->inReplyTo->push_back(new std::string(e.at(i)->getId()));
            }// for

        }// catch

        // get bcc
        try
        {
            throw hdr->Bcc()->getValue().dynamicCast<const vmime::addressList>()->toMailboxList() ;
        }// try
        catch ( vmime::exception e )
        {
            this->bcc = NULL ;
        }// catch
        catch ( vmime::utility::ref<vmime::mailboxList> e )
        {
            this->bcc = new std::list<Box *>() ;
            for ( int i = 0 ; i < e->getMailboxCount() ; i++ )
            {
                this->bcc->push_back(new Box(e->getMailboxAt(i)->getName().getConvertedText(*this->chset).c_str(), e->getMailboxAt(i)->getEmail().c_str()));
            }// for

        }// catch

        // get sender
        try
        {
            throw hdr->Sender()->getValue().dynamicCast<const vmime::mailbox>() ;
        }// try
        catch ( vmime::exception e )
        {
            this->sender = NULL ;
        }// catch
        catch ( vmime::ref<const vmime::mailbox> e )
        {
            this->sender = new Box(e->getName().getConvertedText(*this->chset).c_str(), e->getEmail().c_str());
        }// catch

       // get from
        try
        {
            throw hdr->From()->getValue().dynamicCast<const vmime::mailbox>() ;
        }// try
        catch ( vmime::exception e )
        {
            this->from = NULL ;
        }// catch
        catch ( vmime::ref<const vmime::mailbox> e )
        {
            this->from = new Box(e->getName().getConvertedText(*this->chset).c_str(), e->getEmail().c_str());
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

        // get messageId
        try
        {
            throw hdr->MessageId()->getValue().dynamicCast<vmime::messageId const>()->getId() ;
        } // try
        catch ( vmime::exception e )
        {
            this->messageId = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->messageId = new std::string(e) ;
        }// catch

        // get OriginalMessageId
        try
        {
            throw hdr->OriginalMessageId()->getValue()->generate() ;
        } // try
        catch ( vmime::exception e )
        {
            this->originalMessageId = NULL ;
        }// catch
        catch ( std::string e )
        {
            this->originalMessageId = new std::string(e) ;
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

        // get ReplyTo
        try
        {
            throw hdr->ReplyTo()->getValue()->generate() ;
        }// try
        catch ( vmime::exception e )
        {
			this->ReplyTo = NULL ;
        }// catch
        catch ( std::string e )
        {
			this->ReplyTo = new std::string(e) ;
        }// catch

    }// Function parseHeader

    void Mail::parseBody ( vmime::ref<vmime::message> msg )
    {
        //Set the body output into the body variable

        vmime::messageParser mp(msg);

        vmime::ref<const vmime::contentHandler> cts;

        //Get the first available body part
        if ( mp.getTextPartCount() > 0 )
        {
            vmime::ref<const vmime::textPart> tp = mp.getTextPartAt(0) ;

            vmime ::utility::outputStreamStringAdapter out( *this->body ) ;
            vmime::utility::charsetFilteredOutputStream fout( tp->getCharset(), *this->chset, out );

            //If it is HTML, get the plain text
            if(tp->getType().getSubType() == vmime::mediaTypes::TEXT_HTML)
            {
                vmime::ref <const vmime::htmlTextPart> htp = tp.dynamicCast<const vmime::htmlTextPart>();
                cts = htp->getPlainText() ;
            }// IF
            else if ( tp->getType().getSubType() == vmime::mediaTypes::TEXT_PLAIN ) // Or get just get what it is there
            {
                cts = tp->getText() ;
            }// ELSE

            //Finish the output into the body variable
            cts->extract(fout) ;
            fout.flush();

        }//IF

        this->subject = new std::string( mp.getSubject().getConvertedText(*this->chset) );

        if ( mp.getAttachmentCount() > 0 )
        {

            this->attachments = new std::list<Attachment *> ;

            for ( int i = 0 ; i < mp.getAttachmentCount() ; i++ )
            {
                vmime::ref<const vmime::attachment> att = mp.getAttachmentAt(i) ;
                char szBuffer[255] ;
                char folBuffer[255] = DEFAULT_ATTACHMENT_FOLDER ;
                timeval t ;
                gettimeofday(&t,NULL) ;
                sprintf(szBuffer,"%d%s",static_cast<int>(t.tv_usec),att->getName().generate().c_str()) ;

                Attachment * parsedAtt = new Attachment(szBuffer, folBuffer , att->getType().generate().c_str() ) ;

                std::ofstream outFile(strcat(folBuffer,szBuffer)) ;

                vmime::utility::outputStreamAdapter adpFile(outFile ) ;

                att->getData()->extract(adpFile) ;
                adpFile.flush() ;

                outFile.close() ;

                this->attachments->push_front( parsedAtt ) ;

            }// FOR

        }// IF
        else
        {
            this->attachments = NULL ;
        }// ELSE

    }// Function parseBody

    int Mail::checkLog ( void )
    {
        this->log = new std::ofstream( "/var/log/mail-bot/mail_parse.log" , std::ios::out | std::ios::app ) ;

        return this->log ? 1 : 0 ;
    }// Function checkLog

    void Mail::parseFromFile ( const char * fname )
    {
        this->body = new std::string();

        //Get file as stream
        std::ifstream eml( fname , std::ios::in ) ;

        if ( !eml )
        {
            *this->log << "Could not find the file: " << fname << std::endl;
            delete pInstance ;
            exit(EXIT_FAILURE);
        }// IF

        //Vmime initialization pipeline

        vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();

        this->chset = new vmime::charset(vmime::charsets::UTF_8) ;

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
        std::cout << "subject: " ;

        if ( this->subject != NULL )
        {
            std::cout << this->subject->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "attachments: " ;

        if ( this->attachments != NULL )
        {
            int i = 1 ;
            for ( std::list<Attachment *>::iterator it = this->attachments->begin() ; it != this->attachments->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "Attachment " << i << ":"
                    << std::endl
                    << "filename: " << *(*it)->getName()
                    << std::endl
                    << "path: " << *(*it)->getPath()
                    << std::endl
                    << "type: " << *(*it)->getType()
                    << std::endl ;
            }// FOR

        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "date: " ;

        if ( this->date != NULL )
        {
            std::cout << this->date->date() << " " << this->date->time_of_day() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "deliveredTo: " ;

        if ( this->deliveredTo != NULL )
        {
            std::cout << this->deliveredTo->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "ReplyTo: " ;

        if ( this->ReplyTo != NULL )
        {
            std::cout << this->ReplyTo->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "message-id: " ;

        if ( this->messageId != NULL )
        {
            std::cout << this->messageId->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "original-message-id: " ;

        if ( this->originalMessageId != NULL )
        {
            std::cout << this->originalMessageId->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "references: " ;

        if ( this->references != NULL )
        {
            int i = 1 ;
            for ( std::list<std::string *>::iterator it = this->references->begin() ; it != this->references->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "reference " << i << ":"
                    << std::endl
                    << "ID: " << *(*it)
                    << std::endl ;
            }// FOR
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "in-reply-to: " ;

        if ( this->inReplyTo != NULL )
        {
            int i = 1 ;
            for ( std::list<std::string *>::iterator it = this->inReplyTo->begin() ; it != this->inReplyTo->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "in-reply-to " << i << ":"
                    << std::endl
                    << "ID: " << *(*it)
                    << std::endl ;
            }// FOR
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "user-agent: " ;

        if ( this->userAgent != NULL )
        {
            std::cout << this->userAgent->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "from: " ;

        if ( this->from != NULL )
        {
            std::cout << *this->from->getName()
            << " " << *this->from->getMail() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "sender: " ;

        if ( this->sender != NULL )
        {
            std::cout << *this->sender->getName()
            << " " << *this->sender->getMail() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "organization: " ;

        if ( this->organization != NULL )
        {
            std::cout << this->organization->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "To: " ;

        if ( this->to != NULL )
        {
            int i = 1 ;
            for ( std::list<Box *>::iterator it = this->to->begin() ; it != this->to->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "To " << i << ":"
                    << std::endl
                    << "Name: " << *(*it)->getName()
                    << std::endl
                    << "Email: " << *(*it)->getMail()
                    << std::endl ;
            }// FOR
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "cc: " ;

        if ( this->cc != NULL )
        {
            int i = 1 ;
            for ( std::list<Box *>::iterator it = this->cc->begin() ; it != this->cc->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "Cc " << i << ":"
                    << std::endl
                    << "Name: " << *(*it)->getName()
                    << std::endl
                    << "Email: " << *(*it)->getMail()
                    << std::endl ;
            }// FOR
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "bcc: " ;

        if ( this->bcc != NULL )
        {
            int i = 1 ;
            for ( std::list<Box *>::iterator it = this->bcc->begin() ; it != this->bcc->end() ; it++, i++ )
            {
                std::cout
                    << std::endl
                    << "Bcc " << i << ":"
                    << std::endl
                    << "Name: " << *(*it)->getName()
                    << std::endl
                    << "Email: " << *(*it)->getMail()
                    << std::endl ;
            }// FOR
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

        std::cout << "body: " ;

        if ( this->body != NULL )
        {
            std::cout << this->body->c_str() ;
        }// IF
        else
        {
            std::cout << "NULL VALUE" ;
        }// ELSE

        std::cout << std::endl ;

    }// Function Print

    std::string * Mail::getBody ( void )
    {
        return this->body ;
    }//Function getBody

    std::list<Box *> * Mail::getCc ( void )
    {
        return this->cc ;
    }//Function getCc

    std::list<Box *> * Mail::getTo ( void )
    {
        return this->to ;
    }//Function getTo

    std::string * Mail::getMessageId ( void )
    {
        return this->messageId ;
    }//Function getMessageId

    std::string * Mail::getOriginalMessageId ( void )
    {
        return this->originalMessageId ;
    }//Function getOriginalMessageId

    std::string * Mail::getReplyTo ( void )
    {
        return this->ReplyTo ;
    }//Function replyTo

    std::list<std::string *> * Mail::getInReplyTo ( void )
    {
        return this->inReplyTo ;
    }//Function inReplyTo

    std::list<std::string *> * Mail::getReferences ( void )
    {
        return this->references ;
    }//Function getReferences

    std::string * Mail::getDeliveredTo ( void )
    {
        return this->deliveredTo ;
    }//Function getDeliveredTo

    std::list<Attachment *> * Mail::getAttachments ( void )
    {
        return this->attachments ;
    }//Function getAttachments

    std::string * Mail::getSubject ( void )
    {
        return this->subject ;
    }//Function getSubject

    std::string * Mail::getUserAgent ( void )
    {
        return this->userAgent ;
    }//Function getUserAgent

    std::list<Box *> * Mail::getBcc ( void )
    {
        return this->bcc ;
    }//Function getBcc

    Box * Mail::getFrom ( void )
    {
        return this->from ;
    }//Function getFrom

    Box * Mail::getSender ( void )
    {
        return this->sender ;
    }//Function getSender

    std::string * Mail::getOrganization ( void )
    {
        return this->organization ;
    }//Function getOrganization

    boost::posix_time::ptime * Mail::getDate ( void )
    {
        return this->date ;
    }//Function getDate

}// MAILBOT
