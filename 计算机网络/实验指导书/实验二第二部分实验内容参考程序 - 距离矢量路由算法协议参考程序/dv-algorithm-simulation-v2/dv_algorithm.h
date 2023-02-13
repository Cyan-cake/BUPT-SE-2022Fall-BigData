//
// Created by abc on 9/25/21.
//

// This file declares the class DV_Routing, which have several member functions for sending Distance-vector message
// The constructor function DV_Routing::DV_Routing(int interval) set up a Linux timer which do the timing in a period.
// The time period of the timer is equal to the parameter interval.
// After the timer is timeout, a SIGALRM signal is fired, which will trigger the signal handler function to execute.
// In the constructor function DV_Routing::DV_Routing(int interval), the SIGALRM signal handler function is set before the timer is set up.
// The SIGALRM signal handler will flood distance-vector algorithm's PING message to the neighbor nodes in the network.

// The flood_dv_msg() will call the function provided by a TransportLayer instance, and send a PATH_DISTANCE_MSG message to the all the neighbors of this nodes.
// The flood_ping_dv_msg() will call the function provided by a TransportLayer instance, and send a PING_MSG message to the all the neighbors of this nodes.
// The send_dv_msg() will send a PATH_DISTANCE_MSG message to a neighbor node indicated by the parameter to_node_id

// The construct_dv_msg() is to construct a distance-vector algorithm message, which if of type PING_MSG or PATH_DISTANCE_MSG
// If it is to construct a PATH_DISTANCE_MSG type message, the PATH_DISTANCE_MSG type message contains the PATH cost which is extracted from the routing table.

// The function decode_dv_msg() is to decoded a received message into a distance-vector message.


#ifndef DV_ROUTING_DV_ALGORITHM_H
#define DV_ROUTING_DV_ALGORITHM_H

#include <string.h>
#include "dv_msg.h"
#include "network_info.h"
#include "transport.h"

static void timer_signal_handler(int sig);

class DV_Routing {

public:
    DV_Routing(int interval);
    ~DV_Routing();

    int construct_dv_msg(Network_Info * p_net_info, int msg_type, DV_Msg * p_dv_msg);
    int flood_dv_msg(TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg * p_dv_msg);
    int flood_ping_dv_msg(TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg * p_dv_msg);
    int send_dv_msg(std::string to_node_id, TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg * p_dv_msg);

    int decode_dv_msg(std::string received_msg, DV_Msg *p_dv_msg);
};


#endif //DV_ROUTING_DV_ALGORITHM_H
