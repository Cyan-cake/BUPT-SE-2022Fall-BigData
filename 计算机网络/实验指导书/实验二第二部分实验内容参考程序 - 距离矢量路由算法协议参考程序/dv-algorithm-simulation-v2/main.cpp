#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

#include <sys/time.h>
#include <signal.h>


#include "transport.h"
#include "dv_msg.h"
#include "network_info.h"
#include "user_cmd.h"
#include "dv_algorithm.h"
#include "routing_vectors.h"

using namespace std;


const int MAX_CMD_LEN = 100;
const int POLLING_INTERVAL = 3;

static volatile sig_atomic_t gotAlarm = 0;
class Network_Info net_info; // a object of class Network_info, net_info object is used to store nodes, link and distance table information of the network
class TransportLayer *p_transport_layer; // a pointer to a TransportLayer object, which is used for communicate with other neighbor nodes of this node.
class DV_Routing dv_routing(POLLING_INTERVAL);  // an object instance of class DV_Routing, the parameter of the constructor function is used to set the value of the timer.
class Routing_Vectors_Base routing_vector_base;  // an object instance of class Routing_Vectors_base, routing_vector_base object instance is used to store the new routing vectors from neoghbors.

std::string src_IP_address {"127.0.0.1"};
std::string dest_IP_address {"127.0.0.1"};



int main(int argc, char* argv[]) {

    char user_cmd[MAX_CMD_LEN]; // user_cmd is used to store the command input by user from shell
    DV_Msg *p_dv_msg;
    DV_Msg *p_distance_dv_msg;
    DV_Msg *p_received_dv_msg;
   

    // to set the node ID of this process, the node ID is inputted as the first parameter of the command line parameters
    net_info.set_node_id(argv[1]);

    // to display the node ID of itself
    cout << "The ID of this router is: " << net_info.node_id << endl;

    // To store the UDP port number used by all the nodes in to the node_addr data member of net_info object.
    // The UDP port number used by each node is in a configuration file.
    // This configuration file name is inputted as the second parameter of the command line parameters.
    net_info.init_node_addr(argv[2]);
    net_info.display_node_addr();

    // To store store the cost of each link to of this node's neighbor into the data member neigh_cost of net_info object.
    // The cost of all the links are configured in a configuration file.
    // This configuration file name is inputted as the third parameter of the command line parameters.
    // The init_cost_cost_to_neigh() function also
    net_info.init_cost_to_neigh(argv[3]);
    net_info.display_cost_to_neigh();

    // to initialize the transport layer of this process which represent a node of the simulated network
    // As the process does not know the UDP port used by the node, whose node ID is inputted from command line parameter,
    // this process find the UDP port number stored in the data member node_addr of net_info object.
    int16_t self_port_no, peer_port_no;
    
    self_port_no = net_info.node_addr[net_info.node_id];
    p_transport_layer = new TransportLayer(src_IP_address, self_port_no);

    char recv_buf[1000];
    int byte_received;

    Path_Cost path_cost;
    std::string encoded_msg_str;

    while(1){
        // In the while loop, it is first to check if there is user command inputted from the shell by the user.
        // In this version, two kinds of user commands are to be executed by this process.
        // One user command is "exit", which will make the process exit.
        // Another user command is "disp_rt", which will let the process print the distance table at the time the "disp_rt" command is processed.
        if ( read_user_cmd(user_cmd, MAX_CMD_LEN) == 1 ) {
            if ( strcmp(user_cmd, "exit")  == 0    ){
                exit(0);
            }
            if ( strcmp(user_cmd, "disp_rt")  == 0  ){  // disp_rt cmd is used to display the routing table of this node.
                net_info.display_routing_table();
            }
        }

        // In the while loop, it is second to get the distance-vector message from the neighbors of this process.
        // If no message is received, break the current loop body and entering next loop by executing the continue sentence.
        // As a timer is initialized during the initialization of a DV_Routing object in neighbor node's process,
        // this process will at least a PING_MSG from a neighbor node process in one timing period, unless on neighbor node process is started.
        // If a distance-vector message is received, this process will process the distance-vector message, which is of PING_MSG type or PATH_DISTANCE_MSG type.
        // The received message is stored in the receive buffer recv_buf, which is of 1000 bytes long.
        byte_received = 0;
        byte_received = p_transport_layer->get_msg(recv_buf, 1000, 1000);

        if (byte_received <= 0 ) continue;

        recv_buf[byte_received] = '\0';

        std::string received_string;
        received_string = recv_buf;

        DV_Msg * p_received_dv_msg = new DV_Msg;
        p_received_dv_msg->decode(received_string);

        switch (p_received_dv_msg->msg_type){
            case PING_MSG:
                 {
                    // If the received message is of PING_MSG type, then send back a PING_MSG_REPLY message.
                    DV_Msg * p_ping_reply_msg = new DV_Msg;
                
                    dv_routing.construct_dv_msg(&net_info, PING_MSG_REPLY, p_ping_reply_msg);

                    // to send this PING_MSG_REPLY message to its neighbors.
                    dv_routing.send_dv_msg(p_received_dv_msg->from_node_id, p_transport_layer, &net_info, p_ping_reply_msg);
                    
                    delete p_ping_reply_msg;

                    // After sending back PING_MS_REPLY, chcking if this neighbor is already connected.
                    // If this neighbor node is connected already, then break from this switch case.
                    if (net_info.neigh_connected[p_received_dv_msg->from_node_id] == 1 )  break;
                    
                    // As this neighbor not is not connected before, then doing the following computation: 
                    // To judge if there is already a path to this neighbor node, which is sotred in the distance table.
                    if (! net_info.is_path_to_neighbor_existed(p_received_dv_msg->from_node_id)) {
                        // This branch is for the case that there is not path to this neighbor in the distance table, 
                        // then insert a new path to this neighbor in the distance table.
                        int16_t link_cost = net_info.neigh_cost[p_received_dv_msg->from_node_id];

                        net_info.insert_a_new_path(p_received_dv_msg->from_node_id, link_cost);
    
                        // to display the distance table
                        net_info.display_routing_table();

                    }else
                    { 
                        // THis branch is for the case that there is already a path to this neighbor.    
                        // Then, to check if the cost of this link is samller than the path cost already stored in the distance table.
                        // If the link cost is smaller, then using this link cost to replace the path cost to this neghbor.

                        int16_t link_cost = net_info.neigh_cost[p_received_dv_msg->from_node_id];

                        int16_t path_cost_already_got;

                        path_cost_already_got = net_info.distance_table[p_received_dv_msg->from_node_id].second;

                        if (  link_cost < path_cost_already_got  ){
                            // In the case of a new shorter path is discovered, updating this node's distance table;
                            
                            net_info.distance_table.erase(p_received_dv_msg->from_node_id);

                            auto out_port_and_cost_pr = std::make_pair(p_received_dv_msg->from_node_id, link_cost);
                            auto path_cost_pr = std::make_pair(p_received_dv_msg->from_node_id, out_port_and_cost_pr);
                            auto ret = net_info.distance_table.insert(path_cost_pr);
                       
                            // to display the distance table.
                            net_info.display_routing_table();
                        }
                    }
                }

                // 

                // After sending back PING_MSG_REPLY message, then checking if the neighbor node, from which the PING_MSG message is received, is already connecteded. 
                // If not connected, sending the distance_table to this new connected neighbor.
                if (net_info.neigh_connected[p_received_dv_msg->from_node_id] == 0 ){ // to send distance table to the new neighbor
                    // If the neighbor node is not connected to this node before, send the distance_table to this new connected neighbor

                    // to form dv_msg of PATH_DISTANCE_MSG type dv_msg.
                    DV_Msg * p_distance_dv_msg = new DV_Msg;
                    dv_routing.construct_dv_msg(&net_info, PATH_DISTANCE_MSG, p_distance_dv_msg);

                    dv_routing.flood_dv_msg(p_transport_layer, &net_info, p_distance_dv_msg);

                    delete p_distance_dv_msg;
                }

                // record this neighbor node is now connected to this process node.
                net_info.neigh_connected[p_received_dv_msg->from_node_id] = 1;

                break;
                
            case PING_MSG_REPLY:
                // If the received message is of PING_MSG_REPLY type, which indicates a neighbor node is alive, and is connected to this process node.
                if (net_info.neigh_connected[p_received_dv_msg->from_node_id] == 0 ){ // to send distance table to the new neighbor

                    // If there is not path to this neighbor in the distance table, then insert a new path to this neighbor in the distance table.
                    if (! net_info.is_path_to_neighbor_existed(p_received_dv_msg->from_node_id)) {
  
                        int16_t link_cost = net_info.neigh_cost[p_received_dv_msg->from_node_id];
                        net_info.insert_a_new_path(p_received_dv_msg->from_node_id, link_cost);

                        // to display the distance table
                        net_info.display_routing_table();
                    }
                    else
                    {
                        // This branch is for the case that this neighbor node is connected to this node before.
                        // Then checking if the link cost if the shortest path to this neighbor;
                        // If the link between this node and this neighbor node is shorter, then record this PATH cost to the distance table.
                        // Then, sending the distance_table to this new connected neighbor
                        int16_t link_cost = net_info.neigh_cost[p_received_dv_msg->from_node_id];

                        int16_t path_cost_already_got;

                        path_cost_already_got = net_info.distance_table[p_received_dv_msg->from_node_id].second;

                        if (  link_cost < path_cost_already_got  ){
                            // In the case of a new shorter path is discovered, updating this node's distance table;
                            // and form a new PATH_DISTANCE_MSG message, and send this PATH_DISTANCE_MSG message to the connected neighbor.
                            // net_info.distance_table[p_received_dv_msg->from_node_id].second = link_cost;
                            // net_info.distance_table[p_received_dv_msg->from_node_id].first = p_received_dv_msg->from_node_id;

                            net_info.distance_table.erase(p_received_dv_msg->from_node_id);

                            auto out_port_and_cost_pr = std::make_pair(p_received_dv_msg->from_node_id, link_cost);
                            auto path_cost_pr = std::make_pair(p_received_dv_msg->from_node_id, out_port_and_cost_pr);
                            auto ret = net_info.distance_table.insert(path_cost_pr);
                          
                            // to display the distance table
                            net_info.display_routing_table(); 
                        }
                    }

                    // to form dv_msg of PATH_DISTANCE_MSG type dv_msg.
                    DV_Msg * p_distance_dv_msg = new DV_Msg;
                    dv_routing.construct_dv_msg(&net_info, PATH_DISTANCE_MSG, p_distance_dv_msg);

                    dv_routing.flood_dv_msg(p_transport_layer, &net_info, p_distance_dv_msg);

                    delete p_distance_dv_msg;
                }

                // record this neighbor node is now connected to this process node.
                net_info.neigh_connected[p_received_dv_msg->from_node_id] = 1;

                break;

           case PATH_DISTANCE_MSG:
                
                // To check if there is already a PATH_DISTANCE_MSG distance vector from the message sending node in the routing_vectors_base,
                // If there is, then: first remove this distance vector, and then insert the new PATH_DISTANCE_MSG distance vector in the reouting_vectors_base.
                for (const auto& element_node :  routing_vector_base.vectors_base){
                    if ( p_received_dv_msg->from_node_id == element_node.first){   // element_node.first is the field from_node_id                  
                        routing_vector_base.remove_a_vector(p_received_dv_msg->from_node_id);
                        break;
                    }
                }
                
                { // To insert the new PATH_DISTANCE_MSG distance vector in the reouting_vectors_base.
                    Routing_Vector *p_a_vector = new Routing_Vector;

                    p_a_vector->from_node_id = p_received_dv_msg->from_node_id;
                    p_a_vector->to_nodes.assign(p_received_dv_msg->to_nodes.begin(), p_received_dv_msg->to_nodes.end());

                    routing_vector_base.insert_a_vector(p_received_dv_msg->from_node_id, p_a_vector);
                }

                // To process the received PATH_DISTANCE_MSG message
                for (const auto& element : p_received_dv_msg->to_nodes) {
                    // to process each path cost contained in the PATH_DISTANCE_MSG message
                    if ( element.to_node_id == net_info.node_id){
                        // If the destination node ID of this path is this process node itself, just ignore this path.
                        continue;
                    }

                    if ( 1 != net_info.distance_table.count(element.to_node_id) ) {
                        // To process the case that there is not a path to the destination node of the received path in this node's distance table.
                        // to insert a new entry in this node's distance table
                        auto pr = std::make_pair(p_received_dv_msg->from_node_id, element.cost + net_info.neigh_cost[p_received_dv_msg->from_node_id]);
                        auto table_pr = std::make_pair(element.to_node_id, pr);
                        auto ret_table_pr = net_info.distance_table.insert(table_pr);
                        net_info.display_routing_table();
                    
                        // To send distance_table by PATH_DISTANCE_MSG message to neighors.
                        // to form dv_msg of PATH_DISTANCE_MSG type dv_msg.
                        DV_Msg * p_distance_dv_msg = new DV_Msg;
                        dv_routing.construct_dv_msg(&net_info, PATH_DISTANCE_MSG, p_distance_dv_msg);

                        dv_routing.flood_dv_msg(p_transport_layer, &net_info, p_distance_dv_msg);

                        delete p_distance_dv_msg;
                    
                    }
                    else{
 
                        // To process the case that there is already a path to the destination node of the received path in this node's distance table
                        // to check if a shorter path is discovered.
                        // to calculate new path cost

                        int16_t cost_of_new_path = element.cost + net_info.neigh_cost[p_received_dv_msg->from_node_id];

                        int16_t cost_of_old_path;

                        cost_of_old_path = net_info.distance_table[element.to_node_id].second;

                        if (  cost_of_new_path < cost_of_old_path  ){
                            // In the case of a new shorter path is discovered, updating this node's distance table, and then sending the dv_msg to its neighbors.
 
                            // In the case of a new shorter path is discovered, updating this node's distance table;
                            // and form a new PATH_DISTANCE_MSG message, and send this PATH_DISTANCE_MSG message to the connected neighbor.
                            // net_info.distance_table[p_received_dv_msg->from_node_id].second = link_cost;
                            // net_info.distance_table[p_received_dv_msg->from_node_id].first = p_received_dv_msg->from_node_id;

                            net_info.distance_table.erase(element.to_node_id);

                            auto out_port_and_cost_pr = std::make_pair(p_received_dv_msg->from_node_id, cost_of_new_path);
                            auto path_cost_pr = std::make_pair(element.to_node_id, out_port_and_cost_pr);
                            auto ret = net_info.distance_table.insert(path_cost_pr);
                            net_info.display_routing_table();

                            // To send distance_table by PATH_DISTANCE_MSG message to neighors.
                            // to form dv_msg of PATH_DISTANCE_MSG type dv_msg.
                            DV_Msg * p_distance_dv_msg = new DV_Msg;
                            dv_routing.construct_dv_msg(&net_info, PATH_DISTANCE_MSG, p_distance_dv_msg);

                            // to send the dv_msg to all its neighbors.

                            dv_routing.flood_dv_msg(p_transport_layer, &net_info, p_distance_dv_msg);

                            delete p_distance_dv_msg;
                       }

                    }
                };

                break;
 
           default:
                break;
        }

        delete p_received_dv_msg;

    }


    return 0;


}
