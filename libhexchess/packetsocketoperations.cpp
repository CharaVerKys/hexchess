#include "packetsocketoperations.hpp"
#include "static_objects.hpp"

#define mac_asyncResume(handle)\
    asio::post(static_objects::asio_context(),[h = (handle)]{assert(not h.done()); h.resume();})

cvk::socket::await::sendPacket::sendPacket(asio::ip::tcp::socket&sock, lhc::protocol::PacketHeader const& header, std::vector<std::byte> const& payload):socket_(sock){
    packet->setHeader(header);
    packet->setPayload(payload);
}
void cvk::socket::await::sendPacket::await_suspend(std::coroutine_handle<> h){
    std::size_t totalSize = packet->data.size();
    assert(totalSize == packet->getHeader().totalSize);
    details::send(socket_, std::move(packet), [this,h, totalSize](const std::error_code& ec,size_t lastSendedBytePos){
        if(ec){
            ec_ = ec;
            mac_asyncResume(h);
            // exit from this callback and then resume coroutine, because it may be a connection error
            return;
        }
        assert(lastSendedBytePos == totalSize); // ? offset was 0
        h.resume();
    });
}



cvk::socket::await::readPacket::readPacket(asio::ip::tcp::socket&soc):socket_(soc){}
void cvk::socket::await::readPacket::await_suspend(std::coroutine_handle<>h){
    packet_t headerBuf = std::make_shared<packet>(packet{std::vector<std::byte>(sizeof(lhc::protocol::PacketHeader))});
    details::read(socket_, headerBuf, 0, [headerBuf,h,this](const std::error_code& ec,size_t validDataInBuffer){
        if(ec){
            expected = tl::unexpected(ec);
            mac_asyncResume(h);
            // exit from this callback and then resume coroutine, because it may be a connection error
            return;
        }
        static_assert(sizeof(size_t) == 8);
        assert(validDataInBuffer == 16 and 16 == headerBuf->data.size() and headerBuf->getHeader().totalSize == 16); // dangerous... should not be in release todo
        if(validDataInBuffer not_eq sizeof(lhc::protocol::PacketHeader)){
            expected = tl::unexpected(std::make_error_code(std::errc::message_size));
            mac_asyncResume(h);
            return;
        }

        packet_t buffer = std::make_shared<packet>(packet{std::vector<std::byte>(headerBuf->getHeader().totalSize)});
        // std::ranges::copy(headerBuf->data,buffer->data.begin()); //copy header
        buffer->setHeader(headerBuf->getHeader());
        details::read(socket_,buffer,sizeof(size_t),[buffer,h,this](const std::error_code& ec,size_t validDataInBuffer){
            if(ec){
                expected = tl::unexpected(ec);
                mac_asyncResume(h);
                // exit from this callback and then resume coroutine, because it may be a connection error
                return;
            }
            assert(validDataInBuffer == buffer->data.size());
            expected = std::move(buffer);
            assert(buffer->data.empty());//???
            h.resume();
        });
    });
}

void cvk::socket::await::details::send(asio::ip::tcp::socket & sock, packet_t buffer, std::function<void(const std::error_code&,size_t lastSendedBytePos)>/*for std func*/ &&callback,size_t offset){
    auto continueWriting = [callback = std::move(callback),offset,buffer,&sock](const std::error_code&ec, size_t sendedBytes)/*for std func*/mutable{
        size_t remainToSend = buffer->data.size() - sendedBytes + offset;
        if(ec or remainToSend == 0){
            callback(ec,sendedBytes + offset);
            return;
        }
        //else
        size_t newOffset = sendedBytes + offset;
        send(sock,buffer,std::move(callback),newOffset);
    };
    sock.async_write_some(asio::buffer(buffer->data.data()+offset,buffer->data.size()-offset),continueWriting);
}

void cvk::socket::await::details::read(asio::ip::tcp::socket & sock, packet_t buffer, size_t offset, std::function<void(const std::error_code&,size_t validDataInBuffer)>/*for std func*/ &&callback){
    auto continueReading = [callback = std::move(callback),offset,buffer,&sock](const std::error_code&ec, size_t receivedBytes)/*for std func*/mutable{
        size_t validDataInBuffer = receivedBytes + offset;
        if(ec or validDataInBuffer == buffer->data.size()){
            callback(ec,validDataInBuffer);
            return;
        }
        //else
        read(sock,buffer,validDataInBuffer,std::move(callback));
    };
    sock.async_read_some(asio::buffer(buffer->data.data()+offset,buffer->data.size()-offset),continueReading);
}