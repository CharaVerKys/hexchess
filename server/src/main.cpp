#include <log/logger.hpp>
#include <log/write>

int main(){
    Logger::instance()->setLogDir("/home/charaverk/hexchess/log/server"); //todo create dir and make it variable
    Logger::instance()->init();
    

    cvk::write(log::to::main) << log::lvl::good << "log start";
    cvk::write(log::to::main) << "norm";
    cvk::write(log::to::main) << log::lvl::critical << "crit";

    using namespace log;
    using write_ = cvk::write;
    write_(to::main) << lvl::good << "less code";
    #define good_log write_(to::main) << lvl::good
    good_log << "macros...";


    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    Logger::instance()->exit();
    std::exit(0);
}