#include "attachment.hpp"

namespace mailbot {

    Attachment::Attachment ( char * fname, char * fpath )
    {
        this->filename = new std::string ( fname ) ;
        this->filepath = new std::string ( fpath ) ;
    }// Constructor

    Attachment::~Attachment ( void )
    {
        delete this->filename ;
        delete this->filepath ;
    }// Destructor

    std::string * Attachment::getName ( void )
    {
        return this->filename ;
    }// getName

    std::string * Attachment::getPath ( void )
    {
        return this->filepath ;
    }// getPath

}// MAILBOT
