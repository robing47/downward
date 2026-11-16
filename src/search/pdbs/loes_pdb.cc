#include "loes_pdb.h"
#include "loes.h"
#include "pattern_database.h"
#include "types.h"
#include "../utils/logging.h"

#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pdbs{

LOESPatternDatabase::LOESPatternDatabase(
Projection &&projection, const std::vector<int> &distances)
: projection(projection),
needed_bits(calculate_bits(this->projection)),
num_bits(sum_bits(needed_bits)),
default_value(get_default_value(distances)),
loes(num_bits){
build_loes(distances);
}

//Returns the bitsequence of the abstract state
std::vector<bool> LOESPatternDatabase::index_to_bits(int index) const{
    std::vector<bool> bits;
    int k = projection.get_pattern().size();
    for(int j = k-1; j >= 0; j--){
        int value = projection.unrank(index, j);
        for (int b = needed_bits[j] - 1; b>=0; b--){
            bool bit = (value >> b) & 1;
            bits.push_back(bit);}}
    return bits;
}

//This function counts how often an h-value appears in the distances array and returns the default value
int LOESPatternDatabase::get_default_value(const std::vector<int> &distances) const{
    std::unordered_map<int, int> count;
    for (int h : distances){
        if (count.contains(h)){
            count[h] = count[h] + 1;}
        else{
            count[h] = 1;}
    }

    int most_frequent = distances[0];
    int max_count = 0;

    for (auto [h,c]: count) {
        if (max_count < c) {
            max_count = c;
            most_frequent = h;
        }
    }
	//std::cout << "Default value found: " << most_frequent << std::endl;
    return most_frequent;
}


//This function finds out how many bits are needed for each pattern variable and returns the needed_bits array
std::vector<int> LOESPatternDatabase::calculate_bits(const Projection &projection){
	int k = projection.get_pattern().size();
	std::vector<int> needed_bits(k);

	for(int i = 0; i < k; i++){
	int b = 1;
	int domain_size = projection.get_domain_size(i);
	while((1<<b) < domain_size){
	b++;}
	needed_bits[i] = b;
	}
	return needed_bits;
}

int LOESPatternDatabase::sum_bits(const std::vector<int> &needed_bits){
	int num_bits = 0;
	for(int b: needed_bits){
	num_bits += b;}
	return num_bits;
}

void LOESPatternDatabase::build_loes(const std::vector<int> &distances){
	int num_abstract_states = projection.get_num_abstract_states();

	for (int i = 0; i < num_abstract_states; i++){
	if (distances[i] == default_value){
	continue;
	}
	std::vector<bool> bits = index_to_bits(i);
	loes.add_state(bits, distances[i]);
	}
	loes.finalize_LOES();
	for (int i = 0; i < num_abstract_states; i++) {
    assert(loes.get_value(index_to_bits(i), default_value) == distances[i]);
}
	/*
    //This is needed for benchmarking
    long array_bytes = 4L * num_abstract_states; //Memory for the original pdb. One int is 4 bytes per abstract state in distances array
    long loes_bytes = ((loes.get_finished_loes_size() + 7)/8) + (4L * loes.get_loes_values_size()); //Memory for loes pdb. One bit per entry (rounded up to bytes)
    //plus one int per stored value (states with default value are not stored)

    utils::g_log << "loes pdb: states = "<<num_abstract_states << " stored =  "<<loes.get_loes_values_size()<< " num_bits = "<<num_bits
    << " array_bytes = " << array_bytes<< " loes_bytes = "<< loes_bytes << std::endl;
	*/
    }

//implemented the same way as index_to_bits. "Translates" the real state into a bit encodng
std::vector<bool> LOESPatternDatabase::state_to_bits(const std::vector<int> &state) const{
	std::vector<bool> bits;
	const Pattern &pattern = projection.get_pattern();
	int k = pattern.size();
	for (int i = k - 1; i >= 0; i--){
		//pattern[i] gives nmr of i-th pattern variable in pattern
		//state[] gives the value of the i-th pattern variable in the real state
		int value = state[pattern[i]];
		for (int b = needed_bits[i] - 1; b >= 0; b--){
			bool bit = (value >> b) & 1;
			bits.push_back(bit);}}
		return bits;}

int LOESPatternDatabase::get_value(const std::vector<int> &state) const {
    return loes.get_value(state_to_bits(state), default_value);
}}



