#pragma once
// ? not singletons, just static objects
#include "asio/io_context.hpp"

namespace static_objects{
    inline asio::io_context& asio_context() {
        static asio::io_context instance;
        return instance;
    }
}//nms