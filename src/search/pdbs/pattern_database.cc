#include "pattern_database.h"

#include "../task_utils/task_properties.h"
#include "../utils/logging.h"
#include "../utils/math.h"
#include "loes.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>

using namespace std;

namespace pdbs {
Projection::Projection(const TaskProxy &task_proxy, const Pattern &pattern)
    : pattern(pattern) {
    task_properties::verify_no_axioms(task_proxy);
    task_properties::verify_no_conditional_effects(task_proxy);
    assert(utils::is_sorted_unique(pattern));

    domain_sizes.reserve(pattern.size());
    hash_multipliers.reserve(pattern.size());
    num_abstract_states = 1;
    for (int pattern_var_id : pattern) {
        hash_multipliers.push_back(num_abstract_states);
        VariableProxy var = task_proxy.get_variables()[pattern_var_id];
        int domain_size = var.get_domain_size();
        domain_sizes.push_back(domain_size);
        if (utils::is_product_within_limit(
                num_abstract_states, domain_size, numeric_limits<int>::max())) {
            num_abstract_states *= domain_size;
        } else {
            cerr << "Given pattern is too large! (Overflow occured): " << endl;
            cerr << pattern << endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }
}

int Projection::rank(const vector<int> &state) const {
    size_t index = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        index += hash_multipliers[i] * state[pattern[i]];
    }
    return index;
}

int Projection::unrank(int index, int var) const {
    int temp = index / hash_multipliers[var];
    return temp % domain_sizes[var];
}

std::vector<bool> PatternDatabase::index_to_binary(int index){
    std::vector<bool> bits;
    for (int b = num_bits - 1; b >= 0; b--) {
        bool bit = (index >> b) & 1;
		bits.push_back(bit);
    }
    return bits;
}


//For now ill leave the implementation in the constructor. Later on ill write a own dedicated funtion or class in order to minimize the overhead
PatternDatabase::PatternDatabase(
    Projection &&projection, vector<int> &&distances)
    : projection(move(projection)), distances(move(distances)) {

    int num_abstract_states = this->projection.get_num_abstract_states();

    //This is used to determine how often each h-value appears in distances
    std::unordered_map<int, int> count;
    for (int h : this->distances){
        if (count.contains(h)){
        count[h] = count[h] + 1;}
        else{
        count[h] = 1;}
    }

    //Search for the h-value which appears most
    int mostFrequent = this->distances[0];
    int max_count = 0;
    for (auto [h, c]: count){
        if (max_count < c){
            max_count = c;
            mostFrequent = h;}}

	default_value = mostFrequent;

	//find the smallest num_bits with 2^num_bits >= num_abstract_states
	num_bits = 1;
	while ((1 << num_bits) < num_abstract_states) {
    	num_bits++;
	}

	LOES loes(num_bits);

	for (int i = 0; i < num_abstract_states; i++){
		//ignore the most occurring entries
		if (this->distances[i] == mostFrequent){
			continue;
		}
		std::vector<bool> bits = index_to_binary(i);
		loes.add_state(bits, this->distances[i]);
	}
	loes.finalize_LOES();

    for (int i = 0; i < num_abstract_states; i++) {
    assert(loes.get_value(index_to_binary(i), default_value) == this->distances[i]);
}
}


int PatternDatabase::get_value(const vector<int> &state) const {
    return distances[projection.rank(state)];
}

double PatternDatabase::compute_mean_finite_h() const {
    double sum = 0;
    int size = 0;
    for (size_t i = 0; i < distances.size(); ++i) {
        if (distances[i] != numeric_limits<int>::max()) {
            sum += distances[i];
            ++size;
        }
    }
    if (size == 0) { // All states are dead ends.
        return numeric_limits<double>::infinity();
    } else {
        return sum / size;
    }
}
}
