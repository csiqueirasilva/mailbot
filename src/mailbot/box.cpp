#include "box.hpp"

namespace mailbot {

    Box::Box ( const char * name, const char * mail )
    {
        this->name = new std::string ( name ) ;
        this->mail = new std::string ( mail ) ;
    }// Constructor

    Box::~Box ( void )
    {
        delete this->name ;
        delete this->mail ;
    }// Destructor

    std::string * Box::getName ( void )
    {
        return this->name ;
    }// getName

    std::string * Box::getMail ( void )
    {
        return this->email ;
    }// getMail

}// MAILBOT
