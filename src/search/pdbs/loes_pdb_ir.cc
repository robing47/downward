#include "loes_pdb_ir.h"
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
#include <iostream>
#include <functional>
#include <algorithm>


namespace pdbs{
IRLOESPatternDatabase::IRLOESPatternDatabase(Projection &&projection, const std::vector<int> &distances)
    : PatternDatabase(std::move(projection), std::vector<int>()),
    needed_bits(calculate_bits(get_projection())),
    num_bits(sum_bits(needed_bits)),
    default_value(get_default_value(distances)){
	build_sets(distances);}

//index and state to bits are the same as in loes_pdb
std::vector<bool> IRLOESPatternDatabase::state_to_bits(const std::vector<int> &state) const{
	std::vector<bool> bits;
	const Projection &proj = get_projection();
	const Pattern &pattern = proj.get_pattern();
	int k = pattern.size();
	for (int i = k - 1; i >= 0; i--){
		int value = state[pattern[i]];
		for (int b = needed_bits[i] - 1; b >= 0; b--){
			bool bit = (value >> b) & 1;
			bits.push_back(bit);}}
		return bits;}

std::vector<bool> IRLOESPatternDatabase::index_to_bits(int index) const{
    std::vector<bool> bits;
    const Projection &proj = get_projection();
    int k = proj.get_pattern().size();
    for(int j = k-1; j >= 0; j--){
        int value = proj.unrank(index, j);
        for (int b = needed_bits[j] - 1; b>=0; b--){
            bool bit = (value >> b) & 1;
            bits.push_back(bit);}}
    return bits;
}

//builds the loes in sets. h-values are counted and listed in descending order based on their count (for optimization reasons)
void IRLOESPatternDatabase::build_sets(const std::vector<int> &distances){
    std::unordered_map<int, int> count;
    for (int h : distances){
        if (count.contains(h)){
            count[h] = count[h] + 1;}
        else{
            count[h] = 1;}
	}
	//order the sets, large to small (default left out)
	std::vector<std::pair<int,int>> order;
	for (auto &[h,c]: count){
	if (h != default_value){
	order.push_back(std::make_pair(c,h));}}
	std::sort(order.begin(), order.end(), std::greater<>());

	//h_to_loes_sets maps each h-value to the position in the sets/set_values array
	std::unordered_map<int, int> h_to_loes_sets;

	//create empty loes for every h-value and store the h-value in set_values
	//store mapping h-value->pos in h_to_loes_sets
	for (size_t i = 0; i < order.size(); i++){
		int h = order[i].second;
		set_values.push_back(h);
		sets.push_back(LOES(num_bits));
		h_to_loes_sets[h] = i;
	}

	//add the states to the loes (default values are skipped)
	for (size_t i = 0; i < distances.size(); i++){
		int h = distances[i];
		if (h != default_value){
		int index = h_to_loes_sets[h];
		sets[index].add_state(index_to_bits(i));
		}
	}

	//build every loes in sets
	for (LOES &loes: sets){
		loes.finalize_LOES();
	}}

int IRLOESPatternDatabase::get_value(const std::vector<int> &state) const{
	std::vector<bool> state_bits = state_to_bits(state);
	for (size_t i = 0; i < sets.size(); i++){
	if (sets[i].contains_state(state_bits)){
		return set_values[i];}}
	return default_value;}}

