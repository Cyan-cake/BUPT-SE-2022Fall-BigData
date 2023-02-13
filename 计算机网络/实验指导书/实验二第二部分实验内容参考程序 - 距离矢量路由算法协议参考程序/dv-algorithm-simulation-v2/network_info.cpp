//
// Created by abc on 9/26/21.
//

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "network_info.h"

using namespace std;

Network_Info::Network_Info() {

}

Network_Info::~Network_Info() {

}
int Network_Info::set_node_id(std::string self_node_id) {
    node_id = self_node_id;
    return 1;
}

int Network_Info::init_node_addr(std::string neigh_nodes_file_in) {

    std::ifstream neigh_nodes_in {neigh_nodes_file_in};

    if(!neigh_nodes_in)
    {
        std::cerr << neigh_nodes_file_in << " not open." << std::endl;
        exit(1);
    }

    string key;
    string value;
    while (neigh_nodes_in >> key && getline(neigh_nodes_in, value)) {
        node_addr[key] = stoi(value);
    }

    return 1;
}

int Network_Info::display_node_addr() {
    // to display port number used by nodes, including this nodes itself
    std::cout << "To display port number used by nodes, including this nodes itself:" << std::endl;
    for (const auto& [node, addr] : node_addr)
        std::cout << node << "'s port : " << addr << std::endl;
}

int Network_Info::init_cost_to_neigh(std::string cost_to_neigh_file_in){
    std::ifstream neigh_cost_in {cost_to_neigh_file_in};
    if(!neigh_cost_in)
    {
        std::cerr << cost_to_neigh_file_in << " not open." << std::endl;
        exit(1);
    }

    string from_node, to_node, value;
    while (neigh_cost_in >> to_node &&  neigh_cost_in >> from_node && getline(neigh_cost_in, value)){
        if ( to_node == node_id ) neigh_cost[from_node] = stoi(value);
        if ( from_node == node_id ) neigh_cost[to_node] = stoi(value);

        if  (to_node == node_id) {
            // In the initialization phase, the neigh_connected is set to 0, indicating the link to the neighbor is not established.
            // Only after the initialization phase, and having received a PING_MSG from a neighbor,
            // the element of neigh_connected corresponding to this neighbor is set to 1
            neigh_connected[from_node] = 0;

            neighbour[from_node] = 1;
        }
        if ( from_node == node_id ){
            // In the initialization phase, the neigh_connected is set to 0, indicating the link to the neighbor is not established.
            // Only after the initialization phase, and having received a PING_MSG from a neighbor,
            // the element of neigh_connected corresponding to this neighbor is set to 1
            neigh_connected[to_node] = 0;

            neighbour[to_node] = 1;
        }
    }
}

int Network_Info::display_cost_to_neigh(){

    // to display cost of link to the neighbors
    std::cout << "The cost of link to the neighbors:" << std::endl;
    for (const auto& [node, cost] : neigh_cost)
        std::cout <<  "The link cost to " << node << " is: " << cost << std::endl;

}


int Network_Info::init_routing_table(){
/*
    // This function is of no use. So, the body of this functioin is commented.
    // to initialize distance table

    map<string, int16_t>::iterator iter;
    iter = neigh_cost.begin();
    while (iter != neigh_cost.end()){
        auto pr = std::make_pair(iter->first, iter->second);
        auto table_pr = std::make_pair(iter->first, pr);
        auto ret_table_pr = distance_table.insert(table_pr);
        iter++;
    }
*/
    return 1;
}

int Network_Info::is_path_to_neighbor_existed(std::string neighbor_node_id){
    if (distance_table.find(neighbor_node_id)!= distance_table.end())
        return 1;
    else
        return 0;
}

int Network_Info::insert_a_new_path(std::string neighbor_node_id, int16_t link_cost){
    auto pr = std::make_pair(neighbor_node_id, link_cost);
    auto table_pr = std::make_pair(neighbor_node_id, pr);     
    auto ret_table_pr = distance_table.insert(table_pr);

    return 1;
}

int Network_Info::display_routing_table(){
    cout << endl;
    cout << "The routing table: " << endl;

    map<string, pair<string, int16_t>>::iterator distance_table_iter;
    distance_table_iter = distance_table.begin();
    while (distance_table_iter != distance_table.end()){
        auto table_pr = distance_table_iter->second;
        std::cout << "The distance from this node(" <<  node_id << ") to node(" << distance_table_iter->first << ") is:" << table_pr.second << "  via node(" << table_pr.first << ")" << std::endl;
        distance_table_iter++;
    }
}