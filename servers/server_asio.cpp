#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "../functions/funcs.h"

using boost::asio::ip::tcp;
using namespace std;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new tcp_connection(io_service));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        cout << " Starting" << endl;

        // Приймаємо задачу
        boost::array<char, 4 * ARRAY_SIZE> buf;
        boost::system::error_code error;

        size_t len = socket_.read_some(boost::asio::buffer(buf), error);

        cout << " Server asio received " << len << endl;

        // Тут ми десеріалізуємо прийняті дані
        boost::array<int, ARRAY_SIZE> a = deserialize(convert_array(buf));

        sort(a);

        // Тут ми відправляємо відповідь
        boost::asio::async_write(socket_, boost::asio::buffer(serialize(a)),
                                 boost::bind(&tcp_connection::handle_write,
                                             shared_from_this(), boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }


private:
    tcp_connection(boost::asio::io_service& io_service) : socket_(io_service) { }
    void handle_write(const boost::system::error_code&, size_t)
    { }

    tcp::socket socket_;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 14))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        cout << "Starting accept #" << i << endl;
        i++;
        tcp_connection::pointer new_connection =
                tcp_connection::create(acceptor_.get_io_service());
        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&tcp_server::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
    {
        if (!error)
            new_connection->start();

        start_accept();
    }

    tcp::acceptor acceptor_;
    int i = 0;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        tcp_server server(io_service);
        io_service.run();
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
    }

    return 0;
}