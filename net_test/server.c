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
    strcpy(address.sun_path, "/tmp/mjc_engine");

    struct sockaddr_un dest_address;
    dest_address.sun_family = AF_LOCAL;
    strcpy(dest_address.sun_path, "/tmp/mjc_client");
    
    int rc = bind(handle, (const struct sockaddr*) &address, sizeof(struct sockaddr_un));
    if (rc < 0) {
        printf("failed to bind socket.\n");
        return -2;
    }

    char mesg[] = "Hello from server!";
    int packet_size = sizeof(mesg);
    int sent_bytes = sendto(
            handle
            ,mesg
            ,packet_size
            ,0
            ,(struct sockaddr *)&dest_address
            ,sizeof(struct sockaddr_un));

    if (sent_bytes != packet_size) {
        printf("Failed to send packet: sent %u out of %d bytes.\n", sent_bytes, packet_size);
    } else {
        printf("Success.\n");
    }

    unlink(address.sun_path);

    return 0;
}
