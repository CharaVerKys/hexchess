#pragma once

#include "expected"
#include "protocol.hpp"
#include <asio/ip/tcp.hpp>
#include <coroutine>
#include <ranges>

namespace cvk::socket{
    struct packet{
        std::vector<std::byte> data;
        ////
        lhc::protocol::PacketHeader getHeader(){
            if(data.size() < sizeof(lhc::protocol::PacketHeader)){return {};}
            lhc::protocol::PacketHeader h;
            std::copy(data.begin(), data.begin() + sizeof(lhc::protocol::PacketHeader), reinterpret_cast<std::byte*>(&h));
            return h;
        }
        void setHeader(lhc::protocol::PacketHeader const& header){
            if(not data.empty() and data.size() not_eq sizeof(lhc::protocol::PacketHeader)){
                throw std::logic_error("invalid packet to set header");
            }
            data.resize(16);
            std::memcpy(data.data(),&header,sizeof(header));
        }
        void setHeader_noCheck(lhc::protocol::PacketHeader const& header){
            std::memcpy(data.data(),&header,sizeof(header));
        }
        void setPayload(std::vector<std::byte> const& payload){
            if(data.size() not_eq sizeof(lhc::protocol::PacketHeader)){
                throw std::logic_error("invalid packet to set payload");
            }
            std::ranges::copy(payload,std::back_inserter(data));
        }
        std::ranges::drop_view<std::views::all_t<std::vector<std::byte, std::allocator<std::byte>> &>>
        getPayload() {
          return data | std::views::drop(sizeof(lhc::protocol::PacketHeader));
        }
    };
    using packet_t = std::shared_ptr<packet>;
    namespace await{
        struct sendPacket : public std::suspend_always{
            sendPacket(asio::ip::tcp::socket&, lhc::protocol::PacketHeader const&, std::vector<std::byte> const&);
            void await_suspend(std::coroutine_handle<>);
            [[nodiscard]] std::error_code await_resume(){return ec_;}
            private: asio::ip::tcp::socket& socket_; std::error_code ec_; packet_t packet;
        };
        struct readPacket{
            using expected_t = tl::expected<packet_t,std::error_code>;
            readPacket(asio::ip::tcp::socket&);
            bool await_ready(){return false;}
            void await_suspend(std::coroutine_handle<>);
            [[nodiscard]] expected_t await_resume(){return std::move(expected);}
            private: asio::ip::tcp::socket& socket_; expected_t expected;
        };
        namespace details{
            void send(asio::ip::tcp::socket&, packet_t buffer, std::function<void(const std::error_code&,size_t lastSendedBytePos)> &&callback, size_t offset =0);
            void read(asio::ip::tcp::socket&, packet_t buffer, size_t offset, std::function<void(const std::error_code&,size_t validDataInBuffer)> &&callback);
    }
}
} // namespace