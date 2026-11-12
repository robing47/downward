#include "loes_pdb.h"
#include "loes.h"
#include "pattern_database.h"
#include "types.h"

#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pdbs{

LOESPatternDatabase::LOESPatternDatabase(
Projection &&projection, const std::vector<int> &distances)
: projection(std::move(projection)){
calculate_bits();
default_value = get_default_value(distances);
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
    return most_frequent;
}


//This function finds out how many bits are needed for each pattern variable and sets the members needed_bits and num_bits, which are later needed to build the loes
void LOESPatternDatabase::calculate_bits() {
	int k = projection.get_pattern().size();
	num_bits = 0;
	needed_bits.resize(k);

	for(int i = 0; i < k; i++){
	int b = 1;
	int domain_size = projection.get_domain_size(i);
	while((1<<b) < domain_size){
	b++;}
	needed_bits[i] = b;
	num_bits += b;
}}

void LOESPatternDatabase::build_loes(const std::vector<int> &distances){
	int num_abstract_states = projection.get_num_abstract_states();
	loes = std::make_unique<LOES>(num_bits);

	for (int i = 0; i < num_abstract_states; i++){
	if (distances[i] == default_value){
	continue;
	}
	std::vector<bool> bits = index_to_bits(i);
	loes->add_state(bits, distances[i]);
	}
	loes->finalize_LOES();
	for (int i = 0; i < num_abstract_states; i++) {
    assert(loes->get_value(index_to_bits(i), default_value) == distances[i]);
}}

//implemented the same way as index_to_bits. "Translates" the real state into a bit encodng
std::vector<bool> LOESPatternDatabase::state_to_bits(const std::vector<int> &state) const{
	std::vector<bool> bits;
	const Pattern &pattern = projection.get_pattern();
	for (int i = pattern.size() - 1; i >= 0; i--){
		int value = state[pattern[i]];
		for (int b = needed_bits[i] - 1; b >= 0; b--){
			bool bit = (value >> b) & 1;
			bits.push_back(bit);}}
		return bits;}

int LOESPatternDatabase::get_value(const std::vector<int> &state) const {
    return loes->get_value(state_to_bits(state), default_value);
}}



