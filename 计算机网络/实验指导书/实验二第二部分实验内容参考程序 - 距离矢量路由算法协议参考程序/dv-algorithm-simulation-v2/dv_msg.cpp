//
// Created by abc on 9/23/21.
//
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <regex>
using namespace std;

#include "dv_msg.h"


DV_Msg::DV_Msg(){};
DV_Msg::~DV_Msg(){};

int DV_Msg::insert_from_node_id(int msg_kind, std::string node_id) {
    msg_type = msg_kind;
    from_node_id = node_id;
    return 1;
}

int DV_Msg::insert_a_path_cost(Path_Cost *path_cost) {
    to_nodes.insert(to_nodes.begin()+to_nodes.size(), *path_cost);
    return 1;
}

int DV_Msg::encode(std::string &encoded_string) {
    char tmp_str[100];

    sprintf(tmp_str, "%d", msg_type);

    encoded_string = (std::string) tmp_str + '#' + from_node_id;

    // using '#' to indicate a new path, using '/' to indicate '/' to separate to_node_id from cost

    switch (msg_type) {
        case PATH_DISTANCE_MSG:
            for (auto iter{to_nodes.begin()}; iter < to_nodes.begin() + to_nodes.size(); ++iter) {
                sprintf(tmp_str, "%d", iter->cost);
                encoded_string = encoded_string + '#' + iter->to_node_id + '/' + (std::string) tmp_str;
            }
            encoded_string = encoded_string + '#';

            break;
        case PING_MSG_REPLY:
            encoded_string = encoded_string + '#';
            break;
        case PING_MSG:
            encoded_string = encoded_string + '#';
            break;
        default:
            break;
    }

    return 1;  // indicate success
}

int DV_Msg::decode(std::string string_to_be_decoded) {
    std::string to_decode_string;
    std::string path_cost_field;
    int field_count = 0;
    Path_Cost p_c;
    std::string field_string;

    regex ex_field("#");
    regex ex_path_cost("/|#");

    to_decode_string = string_to_be_decoded;

    std::sregex_token_iterator pos(to_decode_string.begin(), to_decode_string.end(), ex_field, -1);
    decltype(pos) end;
    for (; pos != end; ++pos){
        field_count++;

        if ( field_count == 1 ){
            field_string = pos->str();
            std::string tempString;
            tempString = field_string.substr(0, field_string.length());
            msg_type = std::stoi(field_string,0,10);
            continue;
        };

        if ( field_count == 2 ) {
            field_string = pos->str();

            field_string = field_string.substr(0, field_string.length());
            from_node_id = field_string;

            continue;
        };

        field_string = pos->str();

        std::sregex_token_iterator field_pos(field_string.begin(), field_string.end(), ex_path_cost, -1);
        decltype(field_pos) field_end;

        field_string = field_pos->str();
        field_string = field_string.substr(0, field_string.length());
        p_c.to_node_id = field_string;

        field_pos++;
        field_string = field_pos->str();
        field_string = field_string.substr(0, field_string.length());
        p_c.cost = std::stoi(field_string,0,10);

        to_nodes.push_back(p_c);
    };


}
