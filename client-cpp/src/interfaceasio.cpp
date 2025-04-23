#include "interfaceasio.hpp"
#include "asio.hpp"

void InterfaceAsio::initAsioContext(){assert(asio_);asio_->runContext();}

void InterfaceAsio::setAsioPtr(Asio<InterfaceAsio>* ptr){asio_ = ptr;}
