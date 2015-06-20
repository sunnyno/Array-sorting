#include "client.h"
#include "../functions/funcs.h"
#include <boost/asio.hpp>
#include <fstream>

string serialize();

using boost::asio::ip::tcp;

tcp_client::tcp_client()
{
    sock = -1;
    port = 0;
    address = "";
}

/**
    Connect to a host on a certain port number
*/
bool tcp_client::conn(string address , int port)
{
    //create socket if it is not already created
    if(sock == -1)
    {
        //Create socket
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)
        {
            perror("Could not create socket");
        }

        cout<<"Socket created\n";
    }
    else    {   /* OK , nothing */  }

    //setup address structure
    if(inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;

        //resolve the hostname, its not an ip address
        if ( (he = gethostbyname( address.c_str() ) ) == NULL)
        {
            //gethostbyname failed
            herror("gethostbyname");
            cout<<"Failed to resolve hostname\n";

            return false;
        }

        //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
        addr_list = (struct in_addr **) he->h_addr_list;

        for(int i = 0; addr_list[i] != NULL; i++)
        {
            //strcpy(ip , inet_ntoa(*addr_list[i]) );
            server.sin_addr = *addr_list[i];

            cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

            break;
        }
    }

        //plain ip address
    else
    {
        server.sin_addr.s_addr = inet_addr( address.c_str() );
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    cout<<"Connected\n";
    return true;
}

/**
    Send data to the connected host
*/
bool tcp_client::send_data(string data)
{
    //Send some data
    if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
        perror("Send failed : ");
        return false;
    }
    return true;
}

/**
    Receive data from the connected host
*/
string tcp_client::receive(int size=512)
{
    char buffer[size];

    //Receive a reply from the server
    if( recv(sock, buffer, sizeof(buffer), 0) < 0)
    {
        puts("recv failed");
    }

    string reply = "";
    for(auto chr: buffer)
    {
        if(chr == '\n') break;
        reply += chr;
    }

    return reply;
}

int main(int argc , char *argv[])
{
    tcp_client c;
    string host = "127.0.0.1";

    // Підключаємось
    c.conn(host , 16);

    // Формуємо дані для відправки на сервер
    boost::array<int, ARRAY_SIZE> a;
    for(int i = 0; i < ARRAY_SIZE; i++)
        a[i] = ARRAY_SIZE - i + 4;

    string outbound_data_ = serialize(a);

    // Відправляємо на сервер
    c.send_data(outbound_data_);

    // Приймаємо відповідь від сервера
    cout<<"----------------------------\n\n";
    a = deserialize(c.receive(4 * ARRAY_SIZE));

    ofstream f("client/Client.txt");
    for(auto i: a) f << i << ", ";
    f.close();

    cout << " Written to Client.txt" << endl;
    cout<<"\n\n----------------------------\n\n";

    return 0;
}


