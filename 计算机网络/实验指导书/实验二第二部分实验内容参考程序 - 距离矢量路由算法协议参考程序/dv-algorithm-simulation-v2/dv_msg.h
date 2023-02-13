//
// Created by abc on 9/23/21.
//

// This file declares the class Path_Cost and also declares the class DV_Msg.
// The Path_cost class is used to represent the cost of a PATH.

// The MV_Msg class is used to represent a distance-vector message,
// which contains the following fields:
//    1) message type,
//    2) the id of the node from which node the distance-vector message is sent out
//    3) the list of path_cost for the PATH_DISTANCE_MSG type message.
//       each element of the path_cost has two field: the destination node ID of the path, and the cost of the path.

// The member function insert_from_node_id() is to store the ID of the node from which the DV_msg is sent out.
// The member function insert_a_path_cost() is to insert a path_cost variable into the data member to_nodes, which is a list of Path_Cost elements.

// The member function encode() is to encode a DV_Msg object into a std::string variable,
// the seperator between the fields of the encoded message is '#',  and
// the seperator bewteen the items of the second item of the path cost is '/'.
// No '#' and '/' is used in the value of each field the MV_Msg obejct.

// The member function decode() is to decode a std::string variable into a MV_Msg object.
// The decode() function uses regular expression library regex to take out each field encoded in the std::string object.

#ifndef DV_ROUTING_DV_MSG_H
#define DV_ROUTING_DV_MSG_H

#include <string>
#include <cctype>
#include <vector>

const int PING_MSG = 0;
const int PING_MSG_REPLY = 1;
const int PATH_DISTANCE_MSG = 2;


class Path_Cost{
    public: std::string to_node_id;
    public: int16_t cost;
};

class DV_Msg{
public:
    int  msg_type;
    std::string from_node_id;
    std::vector<Path_Cost> to_nodes;

    DV_Msg();
    ~DV_Msg();

    int insert_from_node_id(int msg_kind, std::string node_id);
    int insert_a_path_cost(Path_Cost *path_cost);

    int to_form_a_dv_msg();

    int encode(std::string &encoded_string);
    int decode(std::string string_to_be_decoded);

};


#endif //DV_ROUTING_DV_MSG_H
