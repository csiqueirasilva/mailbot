#include "attachment.hpp"

namespace mailbot {

    Attachment::Attachment ( const char * fname, const char * fpath, const char * ftype )
    {
        this->filename = new std::string ( fname ) ;
        this->filepath = new std::string ( fpath ) ;
        this->filetype = new std::string ( ftype ) ;
    }// Constructor

    Attachment::~Attachment ( void )
    {
        delete this->filename ;
        delete this->filepath ;
        delete this->filetype ;
    }// Destructor

    std::string * Attachment::getName ( void )
    {
        return this->filename ;
    }// getName

    std::string * Attachment::getPath ( void )
    {
        return this->filepath ;
    }// getPath

    std::string * Attachment::getType ( void )
    {
        return this->filetype ;
    }// getType

}// MAILBOT
