#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

#include <websocket_client.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Report a failure
void
fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class WebsockSession : public std::enable_shared_from_this<WebsockSession>
{
    tcp::resolver resolver_;
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::mutex read_mutex_;
    std::queue< std::string > data_;

public:
    // Resolver and socket require an io_context
    explicit WebsockSession(net::io_context& ioc)
        : resolver_(net::make_strand(ioc))
        , ws_(net::make_strand(ioc))
    {
    }

    // Start the asynchronous operation
    void run( char const* host,
              char const* port )
    {
        // Save these for later
        host_ = host;

        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &WebsockSession::on_resolve,
                shared_from_this()));
    }

    void write(std::string& data) {
        ws_.async_write(
            net::buffer(data),
            beast::bind_front_handler(
                &WebsockSession::on_write,
                shared_from_this()));
    }

    // Probably don't want all these copies!
    void read( std::queue< std::string >& to_process) {
        std::unique_lock< std::mutex > lock(read_mutex_);

        while (!data_.empty()) {
            to_process.push( std::move(data_.front()) );
            data_.pop();
        }
    }

    void
        on_resolve(
            beast::error_code ec,
            tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // Set the timeout for the operation
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                &WebsockSession::on_connect,
                shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
            return fail(ec, "connect");

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
        {
            req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-async");
        }));

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
            beast::bind_front_handler(
                &WebsockSession::on_handshake,
                shared_from_this()));

    }

    void on_handshake(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "handshake");

        // Send the message
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &WebsockSession::on_read,
                shared_from_this()));
    }

    void on_write(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &WebsockSession::on_read,
                shared_from_this()));
    }

    void on_read(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        {
            std::unique_lock< std::mutex > lock(read_mutex_);
           
            data_.push(beast::buffers_to_string(buffer_.data()));
        }

        // Read another message
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &WebsockSession::on_read,
                shared_from_this()));
    }

    void on_close(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence
        std::cout << beast::make_printable(buffer_.data()) << std::endl;
    }
};

////////////////////////////////////////

WebsocketClient::WebsocketClient( boost::asio::io_context& io,
                                  std::string host, 
                                  std::string port ) 
{
    session_ = std::make_shared<WebsockSession>(io);

    session_->run( host.data(), port.data() );
}

void WebsocketClient::send(std::string& data) 
{

}

void WebsocketClient::read( std::queue< std::string >& to_process )
{
    session_->read( to_process );
}