#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include "string"

#include "../functions/funcs.h"

using namespace std;

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[4 * ARRAY_SIZE];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 14 ); // 8888

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");


    //Listen
    listen(socket_desc , 3);
    //Accept and incoming connection
    puts("Initial waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

    //signal(SIGCHLD, SIG_IGN);
    int pid;
    int count = 0;
    while((pid = fork()) >= 0)
    {
        if (pid == 0) // if son works
        {
            if (client_sock < 0)
            {
                perror("accept failed");
                return 1;
            }
            puts("Connection accepted");

            //Receive a message from client
            if( (read_size = recv(client_sock , client_message , 4000 , 0)) > 0 ) {
                // Тут ми десеріалізуємо прийняті дані
                boost::array <int, ARRAY_SIZE> a = deserialize(client_message);

                sort(a);

                //for(auto chr: a) cout << chr << ",";

                //Send the message back to client
                string response = serialize(a);
                cout << "Sending to client: " << response.length() << " bytes " << endl;
                write(client_sock, response.c_str(), response.length());
            }
            exit(0);
        }
        else if (pid > 0) // Father works
        {
            wait();
            close(client_sock);
            count++;
            cout << "Waiting #" << count << endl;
            client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        }
    }

    return 0;
}