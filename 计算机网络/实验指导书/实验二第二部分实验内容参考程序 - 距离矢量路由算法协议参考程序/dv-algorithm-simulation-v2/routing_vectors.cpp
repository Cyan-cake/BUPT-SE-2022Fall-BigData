// Created by abc on Aug. 29, 2022

// This 

#include <iostream>
#include "routing_vectors.h"

Routing_Vector::Routing_Vector(){};
Routing_Vector::~Routing_Vector(){};

Routing_Vectors_Base::Routing_Vectors_Base(){};
Routing_Vectors_Base::~Routing_Vectors_Base(){};

int Routing_Vectors_Base::insert_a_vector(std::string from_node_id, Routing_Vector *routing_vector) {
    auto a_vector_pr = std::make_pair(from_node_id, *routing_vector);
    auto ret = vectors_base.insert(a_vector_pr);

    return 1;
}

int Routing_Vectors_Base::remove_a_vector(std::string node_id) {
 
    if (vectors_base.erase(node_id)){
//        std::cout << " node_id was removed." << std::endl;
    }
    else{
//       std::cout << "node_id was not found." << std::endl;
    }

    return 1;
}
