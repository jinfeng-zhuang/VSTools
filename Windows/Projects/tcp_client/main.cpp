#include <stdio.h>

#include <vs/net.h>

int main(int argc, char* argv[])
{
    net_connect("10.86.79.96", 65528, 0);

    if (net_is_connected()) {
        printf("Network connected\n");
    }
    else {
        printf("Network not connected\n");
    }

    return 0;
}
