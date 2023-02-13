//
// Created by abc on 9/25/21.
//

#include <sys/time.h>
#include <iostream>
#include <signal.h>

using namespace std;

#include "dv_algorithm.h"

extern class Network_Info net_info; // a object of Network_info, which is used to nodes and link costs of the network
extern class TransportLayer *p_transport_layer;
extern class DV_Routing dv_routing;

extern std::string src_IP_address;
extern std::string dest_IP_address;

static volatile sig_atomic_t gotAlarm = 0;

static void sigalrmHandler(int sig){
    gotAlarm = 1;


   // Todo: to check which neighbor node does not reply PING_MSG. 
   // Todo: If the link between this neigbor is dwon, 
   // Todo: then the Distance-Vector algorithm will re-calculate the distance-table using the routing information in the routing_vectors_base.
   //

    // to construct ping message
    DV_Msg dv_msg;
    dv_routing.construct_dv_msg(&net_info,PING_MSG, & dv_msg);
    dv_routing.flood_ping_dv_msg(p_transport_layer, &net_info, & dv_msg);
}

DV_Routing::DV_Routing(int interval){

    // to set timer interval of ping operation
    struct sigaction sa;
    struct itimerval itv;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;
    if (sigaction(SIGALRM, &sa, NULL) == -1){
        std::cout << "sigaction(SIGALRM, &sa, NULL) " << std::endl;
        exit(1);
    }

    itv.it_interval.tv_sec = interval; // 3;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = interval; // 3;
    itv.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &itv, 0) == -1) {
        std::cout << "setitimer()" << std::endl;
        exit(1);
    }
}



DV_Routing::~DV_Routing(){

}

int DV_Routing::construct_dv_msg(Network_Info * p_net_info, int msg_type, DV_Msg *p_dv_msg){

    switch(msg_type) {
        case PING_MSG:
            // to construct ping message
            p_dv_msg->insert_from_node_id(PING_MSG, p_net_info->node_id);
            break;

        case PING_MSG_REPLY:
            p_dv_msg->insert_from_node_id(PING_MSG_REPLY, p_net_info->node_id);
            break;

        case PATH_DISTANCE_MSG:
            Path_Cost path_cost;
            p_dv_msg->insert_from_node_id(PATH_DISTANCE_MSG, p_net_info->node_id);
            for (const auto &element : p_net_info->distance_table) {
                path_cost.to_node_id = element.first;
                path_cost.cost = element.second.second;
                p_dv_msg->insert_a_path_cost(&path_cost);
            }
            break;
    }

}

int DV_Routing::flood_ping_dv_msg(TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg * p_dv_msg){

    // to encode the ping message
    std::string encoded_msg_str;
    p_dv_msg->encode(encoded_msg_str);

    // to send the dv_msg to its neighbors.
    for (const auto& element : p_net_info->neighbour) {
        if ( element.second != 1 ) continue;

        if (element.first == p_net_info->node_id) continue;

        int16_t peer_port_no = p_net_info->node_addr[element.first];  // to find neighbor's port number from neigh_addr;

        p_transport_layer->send_msg( dest_IP_address, peer_port_no, (char *) encoded_msg_str.c_str(), encoded_msg_str.length());
    }
}

int DV_Routing::flood_dv_msg(TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg *p_dv_msg){
    
//   cout << "Entering flood_dv_msg(): " << endl;

    // to encode the ping message
    std::string encoded_msg_str;

    p_dv_msg->encode(encoded_msg_str);

    // to send the dv_msg to its neighbors.
    for (const auto& element : p_net_info->neighbour) {
        if ( element.second != 1 ) continue;

        if (element.first == p_net_info->node_id) continue;

        int16_t peer_port_no = p_net_info->node_addr[element.first];  // to find neighbor's port number from neigh_addr;

//        cout << "flood_dv_msg: " << encoded_msg_str << " to node, whose UDP port is: " << peer_port_no << endl;
        
        p_transport_layer->send_msg(dest_IP_address, peer_port_no, (char *) encoded_msg_str.c_str(), encoded_msg_str.length());
    }
}

int DV_Routing::send_dv_msg(std::string to_node_id, TransportLayer *p_t_l, Network_Info * p_net_info, DV_Msg * p_dv_msg){

    // to encode the ping message
    std::string encoded_msg_str;
    p_dv_msg->encode(encoded_msg_str);

    auto peer_port_no = p_net_info->node_addr[to_node_id];  // to find neighbor's port no from neigh_addr;

    p_transport_layer->send_msg(dest_IP_address, peer_port_no, (char *) encoded_msg_str.c_str(), encoded_msg_str.length());

    return 1;
}

int DV_Routing::decode_dv_msg(std::string received_msg, DV_Msg *p_dv_msg){
    p_dv_msg->decode(received_msg);
}

