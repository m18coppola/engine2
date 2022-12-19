#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    int handle = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (handle <= 0) {
        printf("failed to create socket.\n");
        return -1;
    }
    struct sockaddr_un address;
    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, "/tmp/mjc_client");

    int rc = bind(handle, (const struct sockaddr*) &address, sizeof(struct sockaddr_un));
    if (rc < 0) {
        printf("failed to bind socket.\n");
        return -2;
    }

    int waiting = 1;
    while (waiting) {
        char packet_data[256];

        unsigned int packet_size = sizeof(packet_data);

        struct sockaddr_un from_address;
        socklen_t fromLength = sizeof(from_address);
        int bytes_received = recvfrom(
                handle
                ,packet_data
                ,packet_size
                ,0
                ,(struct sockaddr *)&from_address
                ,&fromLength);
        if (bytes_received <= 0) {
            break;
        } else {
            printf("I got: %s\n", packet_data);
            waiting = 0;
        }

    }

    unlink(address.sun_path);

    return 0;
}
