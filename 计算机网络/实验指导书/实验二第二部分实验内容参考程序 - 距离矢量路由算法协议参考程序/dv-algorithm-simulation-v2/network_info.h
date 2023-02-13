//
// Created by abc on 9/26/21.
//

// This file declares the class Network_Info which is used to store the information of the nodes and links of the simulated network.
// Each nodes of the simulated network is represented by a process, which is created by run this simulation program from the command line of a shell terminal.
// Each process representing a node uses localhost IP address 127.0.0.1 and a UDP port, and uses UDP protocol to communicate with other nodes' processes.
// The data member node_id is to store the node ID of the process itself
// The data member node_addr is map-container type variable, each element of the variable is a pair <node ID, UDP port used by the node process>.
// The data member neighbor is a map-container type variable, each element of the variable is a pair <a node ID, the flag to indicate the node is a neighbor>.
//     The ID of the node which is not a neighbor of this node is not contained in the neighbor variable.
// The data member neigh_cost is a map-container type variable, each element of the variable is a pair <neighbor node ID, the cost of link to the neighbor node>
// The data member neigh_connected is a map-container type variable, each element of the variable is a pair <neighbor node ID, the flag to indicate whether the neighbor is well connected to this node>
// The data member distance_table is a map-container type variable, each element of the variable is a pair <destination node ID, second item>,
//     The second item of each element of the distance_table is also a pair, which is <the next hop node ID from this node, the cost of the path from this node to destination node>

// The member function set_node_id() is to store the ID of this node itself.
// The member function init_node_addr() is to get the UDP port number from configuration file, and store the UDP port numbers in the node_addr variable.
// The member function init_cost_to_neigh() is to get link cost of this node from the configuration file, and store the link cost in the neigh_cost variable.
//      The member function init_cost_to_neigh() also initialize the data member neigh_connected, and data member neighbour;
// The member function init_routing_table() is to initialize the routing table by learning cost of links already stored in the neigh_cost variable


#ifndef DV_ROUTING_NETWORK_INFO_H
#define DV_ROUTING_NETWORK_INFO_H

# include <map>

#include "dv_msg.h"

class Network_Info{

public:
    std::string node_id;    // the id of the node, which is provided via the command parameter of the program

    // The variable node_addr is used to store the UDP port used by nodes of the network
    // The first item of a element of the node_addr variable: is the nodeID of the node
    // The second item of a element of the node_addr variable: is the UDP port number used by the node
    std::map<std::string, std::int16_t> node_addr;

    // The variable neighbour is used to store neighbor.
    // The first item of a element of the neigh_cost is: the nodeID of a neighbor of the node
    // The second item of a element of the neigh_cost is to indicate if the node is the neighbor
    std::map<std::string, std::int16_t> neighbour;

    // The variable neigh_cost is used to store the cost of links of a node.
    // The first item of a element of the neigh_cost is: the nodeID of a neighbor of the node
    // The second item of a element of the neigh_cost is: the cost of the link between the node and the neighbor
    std::map<std::string, std::int16_t> neigh_cost;

    // The variable neigh_connected is to store the states of links of a node
    // The first item of a element of the neigh_connected is: the nodeID of a neighbor of the node
    // The second item of a element of the neigh_connected is: the state of the link between the node and the neighbor
    std::map<std::string, std::int16_t> neigh_connected;

    // The variable distance_table is used to store the distance table of a node
    // The first item of a element of the distance_table is: the nodeID of the destination node
    // The second item of a element of the distance_table is: the cost of the path to the destination node
    // The second item of a element of the distance_table is a pair of <via_node, cost_of_the_path>
    std::map<std::string, std::pair<std::string, int16_t>> distance_table;


    Network_Info();
    ~Network_Info();

    int set_node_id(std::string node_id);


    int init_node_addr(std::string neigh_nodes_file_in);   // used to set the ID of the
    int display_node_addr();

    int init_cost_to_neigh(std::string cost_to_neigh_file_in);
    int display_cost_to_neigh();

    int init_routing_table();
    int display_routing_table();

    int is_path_to_neighbor_existed(std::string neighbor_node_id);
    int insert_a_new_path(std::string neighbor_node_id, int16_t link_cost);
};


#endif //DV_ROUTING_NETWORKROUTING_H
