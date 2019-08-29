#pragma once

#include <memory>
#include <string>
#include <queue>
#include <mutex>

// Forward declare the context
namespace boost {
namespace asio {
    class io_context;
}
}

class WebsockSession;

class WebsocketClient {
public:
    WebsocketClient::WebsocketClient( boost::asio::io_context& io,
                                      std::string host,
                                      std::string port );

    void send( std::string& data );

    // Should probably just be raw buffers
    void read( std::queue< std::string >& to_process );

private:
    std::shared_ptr< WebsockSession > session_;
};
