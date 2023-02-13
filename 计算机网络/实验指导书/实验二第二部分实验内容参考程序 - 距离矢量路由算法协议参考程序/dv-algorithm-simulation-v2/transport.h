//
// Created by abc on 9/25/21.
//

// This file declares a class named TransportLayer.
// The data member src_port is to store the UDP port of the process which simulating a network node.

// The constructor function of TransportLayer class is to:
//  1) create a UDP socket bounded to the local IP address and the UDP port used by the process which simulating a network node.
//  2) set the UDP socket into non-blocking mode.
//  3) for the UDP port, initialize the epoll multiplexing mechanism for receiving data or sending data via the UDP socket.

// The member function Transport::get_msg() is to receive an UDP message from the network.
// The Transport::get_msg() function calls the epoll_wait() function to detect if there is a readable UDP message,
// If there is a received UDP message from the network, the Transport::get_msg() will read the UDP message from the UDP socket
// and then return the UDP message to the caller function.

// The member function Transport::send_msg() is to send an UDP message into the network from the UDP socket.


#ifndef DV_ROUTING_TRANSPORT_H
#define DV_ROUTING_TRANSPORT_H
//
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

class TransportLayer{
    int16_t src_port;
    int sock_fd;
    int flags;
    int epollfd, nfds;
    struct epoll_event ev, events[MAX_EVENTS];

    int16_t dest_port;
    struct sockaddr_in src_addr, dest_addr, peer_addr;
    int len;
    int peer_addr_len;

    int recv_num;
    int send_num;

public:

    TransportLayer(std::string & src_IP, int16_t src_port);
    ~TransportLayer();

    // The parameter recv_buf is the initial address of the receive buffer provided the caller function;
    // The parameter bun_len is the length of the receive buffer provided by the caller function;
    // The parameter to is the timeout value to be used by the get_msg() in the epoll_wait() call.
    int get_msg(char *recv_buf, int16_t buf_len, int to);

    // The parameter dest_IP and parameter dest_port is to indicate the destination that the message is to sent to.
    // The parameter send_buf is the initial address of the send buffer which contains a message to be sent out.
    // parameter msh_len is the length of the length of the message in the send buffer.
    int send_msg(std::string & dest_IP, int16_t dest_port, char *send_buf, int16_t msg_len);
};

#endif //DV_ROUTING_TRANSPORT_H
