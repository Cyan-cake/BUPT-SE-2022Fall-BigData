// Created by abc on Aug. 29, 2022

// This file defines the variable received_routing_vectors, which is used to store the latest routing vector from neighbors.


#ifndef ROUTING_VECTORS_H
#define ROUTING_VECTORS_H

#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "dv_msg.h"


class Routing_Vector{
public:
    std::string from_node_id;
    std::vector<Path_Cost> to_nodes;

    Routing_Vector();
    ~Routing_Vector();
};

class Routing_Vectors_Base{
public:

    std::map<std::string, Routing_Vector> vectors_base;
    
    Routing_Vectors_Base();
    ~Routing_Vectors_Base();

    int insert_a_vector(std::string from_node_id, Routing_Vector *routing_vector);
    int remove_a_vector(std::string node_id);

};




#endif //ROUTING_VECTORS_H