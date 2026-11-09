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
    loes.add_state(states2, 1);

    std::vector<bool> states3;
    states3.push_back(true);
    states3.push_back(false);
    states3.push_back(false);
    loes.add_state(states3, 0);

    std::vector<bool> states4;
    states4.push_back(true);
    states4.push_back(false);
    states4.push_back(true);
    loes.add_state(states4, 0);

    loes.finalize_LOES();
    loes.get_loes();
    std::vector<bool> fake_state;
    fake_state.push_back(true);
    return 0;
}