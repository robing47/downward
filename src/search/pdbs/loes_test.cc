#include "loes.h"
#include "pattern_database.h"
#include <vector>
#include <iostream>


int main(){
    int num_bits = 3;
    pdbs::LOES loes(num_bits);
    std::vector<bool> states;
    states.push_back(false);
    states.push_back(false);
    states.push_back(false);
    loes.add_state(states, 2);

    std::vector<bool> states2;
    states2.push_back(false);
    states2.push_back(true);
    states2.push_back(false);
    loes.add_state(states2, 3);

    std::vector<bool> states3;
    states3.push_back(false);
    states3.push_back(true);
    states3.push_back(true);
    loes.add_state(states3, 6);

    loes.finalize_LOES();
    loes.get_loes();
    int value = loes.get_value(states3, 3);
    std::cout<<"Wert von state3: " << value <<std::endl;
    int id = loes.member_index(states);
    std::cout<<"The id: " << id << std::endl;


    std::vector<bool> states4;
    states4.push_back(true);
    states4.push_back(true);
    states4.push_back(false);
    int value2 = loes.get_value(states4, 4);
    std::cout<<"The value of state4: " << value2 << std::endl;
    return 0;
}