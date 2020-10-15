#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

static int fd = -1;
static struct sockaddr_in addr;
static socklen_t addrlen;
static struct sockaddr_in cli_addr;
static socklen_t cli_addrlen;
static struct timeval timeout;
static fd_set fds;

int hal_recv(unsigned char *buffer, unsigned int length, unsigned int us)
{
    int ret;

    if (-1 == fd) {
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        if (fd >= 0) {
            timeout.tv_sec = 0;
            timeout.tv_usec = us;
            
            //setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

            memset(&addr, 0, sizeof(struct sockaddr_in));

            addr.sin_family = AF_INET;
            addr.sin_port = htons(8000);
            addr.sin_addr.s_addr = INADDR_ANY;

            addrlen = sizeof(struct sockaddr_in);

            ret = bind(fd, (const struct sockaddr *)&addr, addrlen);
            if (-1 == ret) {
                close(fd);
                fd = -1;
            }
        }
    }
    
    if (fd >= 0){
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        ret = select(fd+1, &fds, NULL, NULL, &timeout);
        if ((ret > 0) && FD_ISSET(fd, &fds)) {
            // MUST reset the parameters, otherwise it will fail
            memset(&cli_addr, 0, sizeof(struct sockaddr_in));
            cli_addrlen = 0;
            memset(buffer, 0, length);

            ret = recvfrom(fd, buffer, length, 0, (struct sockaddr *)&cli_addr, &cli_addrlen);
            if (-1 == ret) {
                close(fd);
                fd = -1;
            }
            else {
                return ret;
            }
        }
    }

    return 0;
}

