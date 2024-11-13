#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

#define PORT 8080

int main()
{
    struct pollfd socket_poll;
    struct sockaddr_in sl_address, cl_address;
    int http_socketfd, clientfd;
    int nfds = 0;
    socklen_t cl_address_len = sizeof(cl_address);
    memset(&sl_address, 0, sizeof(sl_address));

    sl_address.sin_family = AF_INET;
    sl_address.sin_port = htons(PORT);
    sl_address.sin_addr.s_addr = INADDR_ANY;

    memset(&cl_address, 0, sizeof(cl_address));

    printf("Creating socket...\n");
    http_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    nfds = 1;
    if(http_socketfd == -1)
    {
        printf("Creating the socket failed\n");
        return 1;
    }

    socket_poll.fd = http_socketfd;
    socket_poll.events = POLLIN;

    sigset_t signal_mask;
    sigemptyset(&signal_mask);

    printf("Binding socket...\n");
    if(bind(http_socketfd, (struct sockaddr *) &sl_address, sizeof(sl_address)) == -1)
    {
        close(http_socketfd);
        printf("Binding socket failed\n");
        return 1;
    };

    printf("Socket successfully created and bound.\n");

    listen(http_socketfd, 1);
    printf("Socket listening on port %d\n", PORT);

    while(nfds >= 1)
    {
        int poll_count = poll(&socket_poll, nfds, 50);
        if(poll_count == -1)
        {
            printf("Error during polling socket input events\n");
            nfds = 0;
            break;
        }

        if(socket_poll.revents & POLLIN)
        {
            printf("Socket polled due to incoming event\n");
            clientfd = accept(http_socketfd, (struct sockaddr *) &cl_address, &cl_address_len);
            if(clientfd == -1)
            {
                printf("Opening client socket file descriptor failed.\n");
            }

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cl_address.sin_addr, client_ip, sizeof(client_ip));
            printf("Client from address %s has connected to the socket.\n", client_ip);
   
            close(clientfd);
        }
    }

    close(http_socketfd);
    return 0;
}

