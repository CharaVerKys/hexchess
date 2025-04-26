#include "asio/executor_work_guard.hpp"
#include "sessionscontrol.hpp"
#include "static_objects.hpp"
#include <log/logger.hpp>
#include <log/write>

int main(){
    Logger::instance()->setLogDir("/home/charaverk/projects/hexchess/log/server"); //todo create dir and make it variable
    Logger::instance()->init();
    auto guard = asio::make_work_guard(static_objects::asio_context());

    SessionsControl sc;
    sc.init(2025);
    static_objects::asio_context().run();

    // no exit now
    Logger::instance()->exit();
    std::exit(0);
}