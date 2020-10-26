#ifndef VS_NET_H
#define VS_NET_H

extern int net_connect(const char* host_name, int port, int msec);
extern int net_disconnect(void);
extern int net_transfer(unsigned char* params, void* result);
extern int net_is_connected(void);

#endif