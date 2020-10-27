#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <string.h>
#include <stdio.h>

#include <vs/log.h>

static SOCKET _socket = INVALID_SOCKET;
static char   _host_name[256];
static int    _port = 0;

int net_disconnect(void)
{
    closesocket(_socket);

    WSACleanup();

    return 0;
}

int net_connect(const char* host_name, int port, int msec)
{
    struct sockaddr_in  host_addr;
    int                 opt_val = 1;
    int                 opt_len = sizeof(opt_val);
    WSADATA ws;

    memset(&host_addr, 0, sizeof(host_addr));

    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.S_un.S_addr = inet_addr(host_name);
    host_addr.sin_port = htons((short)port);

    if (_socket != INVALID_SOCKET)
        closesocket(_socket);

    if (0 != WSAStartup(0x0101, &ws)) {
        goto FAILED;
    }

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    // 该选项会禁用Nagle算法，所有的都是私家车，有数据就交互
    // Nagle算法 大家都做公交车，会有等待
    // 游戏会禁用该算法，避免 PK 时技能没及时甩到服务器上
    setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)& opt_val, opt_len);

    // 心跳用于检测对端是否还在响应
    // 但是该机制间隔时间太长了，默认 2 小时
    setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)& opt_val, opt_len);

    //setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)& msec, sizeof(msec));
    //setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)& msec, sizeof(msec));

    vs_log(LOG_MASK_NET, VS_LOG_VERBOSE, "%s %d\n", __FUNCTION__, __LINE__);

    if (-1 == connect(_socket, (struct sockaddr*) & host_addr, sizeof(host_addr))) {
        goto FAILED;
    }

    vs_log(LOG_MASK_NET, VS_LOG_VERBOSE, "%s %d\n", __FUNCTION__, __LINE__);

    strcpy(_host_name, host_name);
    _port = port;

    return 0;

FAILED:
    if (_socket != INVALID_SOCKET) {
        closesocket(_socket);
    }

    return -1;
}

int net_transfer(unsigned char* request, void* response)
{
    unsigned int request_len = 0;
    unsigned int response_length = 0;

    unsigned char len[2];
    unsigned char* p = NULL;

    unsigned int count = 0;
    int length = 0;

    if (_socket == INVALID_SOCKET)
        return -1;

    p = (unsigned char*)response;

    request_len = (unsigned short)request[2] + ((unsigned short)request[3] << 8) + 4;

    // send command
    if (send(_socket, (const char*)request, request_len, 0) <= 0) {
        vs_log(LOG_MASK_NET, VS_LOG_WARNING, "send failed\n");
        goto FAILED;
    }

    if (NULL == p) {
        goto NO_RESPONSE;
    }

    // receive data length
    if (recv(_socket, (char*)len, sizeof(len), 0) <= 0) {
        vs_log(LOG_MASK_NET, VS_LOG_WARNING, "recv len failed\n");
        goto FAILED;
    }

    // TODO compare with request data length
    response_length = len[0] + ((int)len[1] << 8);
    count = response_length;

    vs_log(LOG_MASK_NET, VS_LOG_DEBUG, "response len: %d\n", response_length);

    // receive data
    while (count > 0)
    {
        length = recv(_socket, (char*)p, count, 0);

        if (length <= 0) {
            vs_log(LOG_MASK_NET, VS_LOG_WARNING, "recv data failed\n");
            goto FAILED;
        }

        count -= length;
        p += length;
    }

    return response_length;

FAILED:
    if (_socket != INVALID_SOCKET) {
        closesocket(_socket);
    }
    return -1;

NO_RESPONSE:
    return 0;
}

int net_is_connected(void)
{
    if (_socket != INVALID_SOCKET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
