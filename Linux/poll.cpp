#include <iostream>  // cout
#include <unistd.h>
#include <string.h>  // strlen
#include <vector>
using std::cout;
using std::endl;
using std::vector;

#include <sys/types.h>   // socket
#include <sys/socket.h>  // socket
#include <sys/epoll.h>  // epoll
//#include <netinet/in.h>  // sockaddr_in / htonl
#include <arpa/inet.h>  // inet_addr
#include <poll.h>        // poll
#include <fcntl.h>  // fcntl // <--setnoneblocking <--epoll

namespace ns_socket{

    void func_define()
    {
        cout << "__FILE__:" << __FILE__ << endl;
        cout << "__LINE__:" << __LINE__ << endl;
        cout << "__DATE__:" << __DATE__ << endl;
        cout << "__TIME__:" << __TIME__ << endl;
        cout << "__TIMESTAMP__:" << __TIMESTAMP__ << endl;
    }

    // 简单客户端代码
    void func_simpleClient(u_short s_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12345), char *szMsg = "Hello Server")
    {
        typedef int SOCKET;
#define MAX_PATH 260
        sockaddr_in addrServer = {};
        addrServer.sin_addr.s_addr = s_addr;
        addrServer.sin_port = sin_port;
        addrServer.sin_family = AF_INET;
        char szRecv[MAX_PATH] = {};
        //
        SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        int iConnectRtn = connect(sockClient, (sockaddr *)&addrServer, sizeof(addrServer));
        cout << "sock:" << iConnectRtn << endl;
        if (iConnectRtn == -1)
        {
            close(sockClient);
            return;
        }
        int iSendRtn = send(sockClient, szMsg, strlen(szMsg), 0);
        cout << "[" << szMsg << "] is send " << iSendRtn << "/" << strlen(szMsg) << endl;
        int iRecvRtn = recv(sockClient, szRecv, MAX_PATH, 0);
        cout << "[" << szRecv << "] is recv " << iRecvRtn << "/" << MAX_PATH << endl;
        getchar();
        close(sockClient);
    }
    
    // 简单socket网络服务器
    void func_simpleserver()
    {
        typedef int SOCKET;
#define MAX_PATH 260
        struct sockaddr_in addrServer = {};
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(12345);
        addrServer.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr("127.0.0.1");
        struct sockaddr_in addrClient = {};
        socklen_t iAddrLen = sizeof(addrClient);
        char szRecv[MAX_PATH] = {};
        char szSend[MAX_PATH] = "hello linux clinet";

        SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
        cout << "sockServer:" << sockServer << endl;
        int iBindRth = bind(sockServer, (sockaddr *)&addrServer, sizeof(addrServer));
        cout << "iBindRth:" << iBindRth << endl;
        int iListenRtn = listen(sockServer, SOMAXCONN);
        cout << "iListenRtn:" << iListenRtn << endl;
        SOCKET sockClient = accept(sockServer, (sockaddr *)&addrClient, &iAddrLen);
        cout << "sockClient:" << sockClient << " form " << inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port << endl;

        int iRecvRth = recv(sockClient, szRecv, MAX_PATH, 0);
        cout << "iRecvRth:" << iRecvRth << " msg:" << szRecv << endl;
        int iSend = send(sockClient, szSend, strlen(szSend), 0);
        cout << "iSend:" << iSend << " msg:" << szSend << endl;

        close(sockServer); // closesocket(sockServer);
    }
    
    // select 网络服务器模型
    void func_selectTCPServer(){
        typedef int SOCKET;
#define MAX_PATH 260
        struct sockaddr_in addrServer = {};
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(12345);
        addrServer.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
        struct sockaddr_in addrClient = {};
        socklen_t iAddrLen = sizeof(addrClient);
        char szRecv[MAX_PATH] = {};
        char szSend[MAX_PATH] = "hello linux clinet";

        SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
        cout << "sockServer:" << sockServer << endl;
        int iBindRth = bind(sockServer, (sockaddr *)&addrServer, sizeof(addrServer));
        cout << "iBindRth:" << iBindRth << endl;
        int iListenRtn = listen(sockServer, SOMAXCONN);
        cout << "iListenRtn:" << iListenRtn << endl;

        // 单线程select网络模型
        for (;;)
        {
            // 用于保存连接ID和地址信息的结构体
            struct SockInfo
            {
                SOCKET sock = 0;
                sockaddr_in addr = {};
                SockInfo(SOCKET sockClient = 0, sockaddr_in addrClient = {})
                    : sock(sockClient), addr(addrClient) { ; }
            };
            std::vector<SockInfo> vecClient; // 客户端链接
            fd_set readfds;                  // 服务端仅关注可读事件
            while (true)
            {
                // 初始化队列
                FD_ZERO(&readfds);
                // 加入服务端socket，以便有客户端连接时触发accept
                FD_SET(sockServer, &readfds);
                // 加入客户端socket，以便客户端发来消息时触发
                for (auto var : vecClient)
                {
                    FD_SET(var.sock, &readfds);
                }
                // 服务端socket的select只需关注是否有可读事件到来，因为接入的客户端总是可写的
                // 客户端的connect和send会触发服务端的可读事件，以便服务端调用accept和recv
                // select的timeval参数: null:阻塞; 0:立即返回; >0:设置超时时间
                // select的timeval取值含义与WSAPoll不同
                int iSelectRtn = select(sockServer + 1, &readfds, NULL, NULL, NULL); // &tv);
                if (iSelectRtn == 0)
                {
                    continue;
                } // 超时
                else if (iSelectRtn < 0)
                {
                    break;
                } // 错误
                // 检查服务端socket是否可读  // 新客户端接入时会触发
                if (FD_ISSET(sockServer, &readfds))
                {
                    FD_CLR(sockServer, &readfds);
                    SOCKET sockClient = accept(sockServer, (sockaddr *)&addrClient, &iAddrLen);
                    std::cout << "new client socket(" << sockClient << ") form "
                              << inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port << std::endl;
                    if (sockClient <= 0)
                    {
                        continue;
                    } // while(true){continue;}
                    vecClient.push_back(SockInfo(sockClient, addrClient));
                }
                // 检查客户端连接是否可读  // 客户端发来消息时会触发可读事件
                for (auto var : vecClient)
                {
                    if (FD_ISSET(var.sock, &readfds))
                    {
                        FD_CLR(var.sock, &readfds);
                        memset(szRecv, 0, MAX_PATH);
                        int iRecvRtn = recv(var.sock, szRecv, MAX_PATH, 0);
                        std::cout << "socket(" << var.sock << ")"
                                  << " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
                                  << " recv msg(" << strlen(szRecv) << "/" << MAX_PATH << "):[" << szRecv << "]" << std::endl;
                        if (iRecvRtn <= 0)
                        { // 客户端断开连接
                            close(var.sock);
                            std::cout << "socket(" << var.sock << ")"
                                      << " form " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
                                      << " 已断开连接" << std::endl;
                            // 删除已断开客户端的连接信息
                            vector<SockInfo>::iterator ite = vecClient.begin();
                            for (; ite != vecClient.end(); ++ite)
                            {
                                if (ite->sock = var.sock)
                                {
                                    vecClient.erase(ite);
                                    break; // 终止iterator的for循环
                                }
                            }
                            break; // 终止FD_ISSET的for循环
                        }
                        // 客户端总是可写的，因此可以在服务端处理完发来的消息后立即返回
                        sprintf(szRecv, "hello! client socket %d form %s:%d",
                                var.sock, inet_ntoa(var.addr.sin_addr), var.addr.sin_port);
                        int iSendRtn = send(var.sock, szRecv, strlen(szRecv), 0);
                        std::cout << "socket(" << var.sock << ")"
                                  << " to   " << inet_ntoa(var.addr.sin_addr) << ":" << var.addr.sin_port
                                  << " send msg(" << strlen(szRecv) << "/" << iSendRtn << "):[" << szRecv << "]" << std::endl;
                    }
                }
            }
        }
    }

    // poll 网络服务器模型
    void func_pollTCPServer(){
//typedef unsigned long int nfds_t;
//struct pollfd {
//    int fd;			/* File descriptor to poll.  */
//    short int events;		/* Types of events poller cares about.  */
//    short int revents;		/* Types of events that actually occurred.  */
//  };
//
//extern int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);
//#ifdef __USE_GNU
//extern int ppoll (struct pollfd *__fds, nfds_t __nfds,
//		  const struct timespec *__timeout,
//		  const __sigset_t *__ss);
//#endif
        typedef int SOCKET;
#define MAX_PATH 260
        struct sockaddr_in addrServer = { };
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(12345);
        addrServer.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
        struct sockaddr_in addrClient = { };
        socklen_t iAddrLen = sizeof(addrClient);
        char szRecv[MAX_PATH] = {};
        char szSend[MAX_PATH] = "hello linux clinet";

        SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
        cout << "sockServer:" << sockServer << endl;
        int iBindRth = bind(sockServer, (sockaddr*)&addrServer, sizeof(addrServer));
        cout << "iBindRth:" << iBindRth << endl;
        int iListenRtn = listen(sockServer, SOMAXCONN);
        cout << "iListenRtn:" << iListenRtn << endl;

        // 经典socket
        for (; 0;)
        {
            SOCKET sockClient = accept(sockServer, (sockaddr *)&addrClient, &iAddrLen);
            cout << "sockClient:" << sockClient << " form " << inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port << endl;

            int iRecvRth = recv(sockClient, szRecv, MAX_PATH, 0);    cout << "iRecvRth:" << iRecvRth << " msg:" << szRecv << endl;
            int iSend = send(sockClient, szSend, strlen(szSend), 0); cout << "iSend:" << iSend << " msg:" << szSend << endl;
            close(sockServer); return;
        }

        // 测试
        #define POLL_SIZE 10
        nfds_t nCount = 0;
        pollfd fds[POLL_SIZE] = {};
        // 加入服务器
        fds[0].fd = sockServer;
        fds[0].events = POLLIN;
        nCount++;
        // 检查
        while (true)
        {
            int iPollRtn = poll(fds, nCount, -1);
            cout << "pollRth:" << iPollRtn << " count:" << nCount << endl;
            if (iPollRtn == 0) { continue; } else if (iPollRtn == -1) { break; }
            // 接收到消息
            for (size_t i = 0; i < nCount; i++)
            {
                cout << "fds[" << i+1 << "/" << nCount << "]"
                     << " fd:" << fds[i].fd
                     << " event:" << (fds[i].events == POLLIN ? "POLLIN" : "")
                     << (fds[i].events == POLLHUP ? "POLLHUP" : "")
                     << (fds[i].events == POLLERR ? "POLLERR" : "")
                     << " revents:" << fds[i].revents << endl;
                // 刚加入的客户端事件
                if(fds[i].revents == 0) { continue; }
                // 非传入事件暂不处理
                if(!(fds[i].revents & POLLIN)){ continue; };
                // 服务器
                if (fds[i].fd == sockServer)
                {
                    int sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen);
                    cout << "accept sock " << sockClient
                         << " form " << inet_ntoa(addrClient.sin_addr)
                         << ":" << addrClient.sin_port << endl;
                    fds[nCount].fd = sockClient;
                    fds[nCount].events = POLLIN;
                    nCount++;
                    continue;
                }
                // 客户端有传入的(可读或断开等)消息
                memset(szRecv, 0, MAX_PATH);
                memset(szSend, 0, MAX_PATH);
                int iReadRtn = read(fds[i].fd, szRecv, MAX_PATH);
                cout << "recv sock " << fds[i].fd
                     << " Msg(" << iReadRtn << "/" << MAX_PATH << "):"
                     << "[" << szRecv << "]" << endl;
                // 断开连接暂不修改计数，待循环完毕扫描修改
                if(iReadRtn <= 0){
                    close(fds[i].fd);
                    //nCount--;
                    bzero(&fds[i], sizeof(fds[i]));
                    continue;
                }
                sprintf(szSend, "server recv form %d msg:[%s]", fds[i].fd, szRecv);
                int iSendRtn = send(fds[i].fd, szSend, strlen(szSend), 0);
                cout << "send sock " << fds[i].fd
                     << " Msg(" << iSendRtn << "/" << strlen(szSend) << "):"
                     << "[" << szSend << "]" << endl;
            }
            // 整理有效连接信息
            for (size_t i = 0; i < POLL_SIZE; i++)
            {
                // 有效连接不处理
                if(fds[i].fd && fds[i].events) { continue; }
                // 无效连接时，从后面找出有效连接进行替换
                for (size_t j = i + 1; j < POLL_SIZE; j++)
                {
                    if(fds[j].fd && fds[j].events)
                    {
                        memcpy(&fds[i], &fds[j], sizeof(fds[i]));
                        memset(&fds[j], 0, sizeof(fds[j]));
                        break;
                    }
                }
                // 后面均无有效连接时，即可跳出循环  // 未找到有效连接则不会进行交换
                if(fds[i].fd == 0 && fds[i].events == 0){
                    break;
                };
            }
            // 修改有效连接计数
            for (size_t i = POLL_SIZE - 1; i >= 0; --i) {
                if(fds[i].fd && fds[i].events){
                    nCount = i + 1;
                    cout << "after socket nCount:" << nCount << endl;
                    break;
                }
            }
            // 没有客户端连接就退出服务器吧
            if (nCount == 1 && fds[0].fd == sockServer)
            {
                close(sockServer);
                sockServer = 0;
                cout << "socket server is exit!" << endl;
                break;//while(true)
            }
        }
        cout << "function is run over!" << endl;
    }

    // epoll 网络服务器模型
    void func_epollTCPServer(){
//#define EPOLL_CTL_ADD 1	/* Add a file descriptor to the interface.  */
//#define EPOLL_CTL_DEL 2	/* Remove a file descriptor from the interface.  */
//#define EPOLL_CTL_MOD 3	/* Change file descriptor epoll_event structure.  */
//
//typedef union epoll_data
//{
//  void *ptr;
//  int fd;
//  uint32_t u32;
//  uint64_t u64;
//} epoll_data_t;
//
//struct epoll_event
//{
//  uint32_t events;	/* Epoll events */
//  epoll_data_t data;	/* User data variable */
//} __EPOLL_PACKED;
//
//extern int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event *__event) __THROW;
//
//extern int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
        typedef int SOCKET;
#define MAX_PATH 260
        struct sockaddr_in addrServer = {};
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(12345);
        addrServer.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
        struct sockaddr_in addrClient = {};
        socklen_t iAddrLen = sizeof(addrClient);
        char szRecv[MAX_PATH] = {};
        char szSend[MAX_PATH] = "hello linux clinet";

        SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
        cout << "sockServer:" << sockServer << endl;
        int iBindRth = bind(sockServer, (sockaddr *)&addrServer, sizeof(addrServer));
        cout << "iBindRth:" << iBindRth << endl;
        int iListenRtn = listen(sockServer, SOMAXCONN);
        cout << "iListenRtn:" << iListenRtn << endl;

        for (;;){
#define MAX_EVENTS 10
            struct epoll_event ev;
            struct epoll_event events[MAX_EVENTS];
            int epollfd = epoll_create1(0);
            //
            ev.events = EPOLLIN;
            ev.data.fd = sockServer;
            int iCtlRtn = epoll_ctl(epollfd, EPOLL_CTL_ADD, sockServer, &ev);
            //
            for (;;)
            {
                int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
                for (int n = 0; n < nfds; ++n) {
                    // 服务器消息  // 新客户端传入
                    if (events[n].data.fd == sockServer)
                    {
                        SOCKET sockClient = accept(sockServer, (struct sockaddr *)&addrClient, &iAddrLen);
                        cout << "accept sock " << sockClient
                             << " form " << inet_ntoa(addrClient.sin_addr)
                             << ":" << addrClient.sin_port << endl;
                        fcntl(sockClient, F_SETFL, fcntl(sockClient, F_GETFD, 0) | O_NONBLOCK); // setnonblocking(sockClinet);
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = sockClient;
                        epoll_ctl(epollfd, EPOLL_CTL_ADD, sockClient, &ev);
                        continue;
                    }
                    // 客户端消息
                    // do_use_fd(events[n].data.fd);
                    memset(szRecv, 0, MAX_PATH);
                    memset(szSend, 0, MAX_PATH);
                    // 有传入的(可读或断开等)消息
                    int iReadRtn = read(events[n].data.fd, szRecv, MAX_PATH);
                    cout << "recv sock " << events[n].data.fd
                         << " Msg(" << iReadRtn << "/" << MAX_PATH << "):"
                         << "[" << szRecv << "]" << endl;
                    // 接收到断开连接消息
                    if (iReadRtn == 0)
                    {
                        close(events[n].data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
                        // 计数推出服务器
                        continue;
                    }
                    // 正常连接状态发送数据
                    sprintf(szSend, "server recv form %d msg:[%s]", events[n].data.fd, szRecv);
                    int iSendRtn = send(events[n].data.fd, szSend, strlen(szSend), 0);
                    cout << "send sock " << events[n].data.fd
                         << " Msg(" << iSendRtn << "/" << strlen(szSend) << "):"
                         << "[" << szSend << "]" << endl;
                }
            }
        }
        close(sockServer);
    }
    
    // 名字空间入口
    int main(){
        //func_simpleClient();
        //func_simpleserver();
        func_selectTCPServer();
        //func_pollTCPServer();
        //func_epollTCPServer();
        return 0;
    }
}

int main(){
    ns_socket::main();
    getchar();
    return 0;
}