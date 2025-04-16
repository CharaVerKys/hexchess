#include <log/logger.hpp>
#include <log/write>

int main(){
    Logger::instance()->setLogDir("/home/charaverk/hexchess/log/server"); //todo create dir and make it variable
    Logger::instance()->init();
    

    Logger::instance()->exit();
    std::exit(0);
}