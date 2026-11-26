#include "loes.h"
#include <cassert>
#include <vector>
#include <iostream>

namespace pdbs {

LOES::LOES(int num_bits) : num_bits(num_bits), treelevels(num_bits), has_states(false) {
  assert(num_bits>0);
}

//The path_offset function (algo 1) navigates through the LOES and checks if state represents a valid path.
//It returns the offset if its a valid path and -1 if not
int LOES::path_offset(const std::vector<bool> &state) const{
  if (finished_LOES.empty()) {
    return -1;
  }
  int offset = 0;
  for (int depth = 0; depth <= num_bits-1; depth++) {
    if (state[depth]) {
      offset++;
    }
    if (finished_LOES[offset]) {
      if (depth == num_bits-1) {
        return offset;
      }else {
        offset = 2 * compute_rank(offset);
      }
    }else {
      return -1;
    }
  }
  return -1;
}
//Returns the id of the given state or -1 if the state's not contained (algo. 2)
int LOES::member_index(const std::vector<bool> &state) const{
  int o = path_offset(state); //checks if the path to the state exists, if not returns -1
  if (o == -1) {
    return -1;
  }
  int a = compute_rank(o);
  int b = compute_rank(level_offsets[num_bits-1] - 1);
  return a-b-1;
}

//Adds a new state to the LOES (Algo 3 to double check). States must be added in ascending order
void LOES::add_state(const std::vector<bool> &state, int value) {
  add_state(state);
  loes_values.push_back(value);
  }


//counts the 1's in the finished LOES from position 0 up to and including the offset
int LOES::simple_rank(int offset) const{
  int rank = 0;
  for (int i = 0; i <= offset; i++){
    if (finished_LOES[i] == 1) {
      rank++;
    }
  }
  return rank;
}

//This method concatenates the bits in treelevels into one consecutive bit string. The result is the finished LOES. It also records where each level starts (in level_offsets)
void LOES::finalize_LOES() {
  for (int i = 0; i < num_bits; i++) {
    level_offsets.push_back(finished_LOES.size()); //builds the level_offsets vector which is needed for the member_index function
    //finished_LOES.size() is exactly the position where the next level will start
    finished_LOES.insert(finished_LOES.end(), treelevels[i].begin(), treelevels[i].end()); //concatenates the bit string level by level
  }
    treelevels.clear();
    rank_index();
}

//returns the finished_LOES for testing
std::vector<bool> LOES::get_loes(){
    for (bool b: finished_LOES){
    std::cout << b<<std::endl;}
	return finished_LOES;}

int LOES::get_value(const std::vector<bool> &state, int default_value) const{
	int id = member_index(state);
	if (id == -1) {
		return default_value;}
	int value = loes_values[id];
	return value;
}

//prepares the two tables which compute_rank later needs. The function loops through
//the finished loes and marks at certain positions how many 1 were counted up till that point.
//the loes will be split into blocks. each block consists of 128 subblocks(a subblock is 512 bits)
void LOES::rank_index() {
    block_index.clear();
    subblock_index.clear();

    const int bits_per_block = 1<<16;
    const int bits_per_subblock = 512;

    uint64_t total = 0;
    uint64_t start_block = 0;

    for (int i = 0; i < finished_LOES.size(); i++) {
        if (i % bits_per_block == 0) {
            block_index.push_back(total);
            start_block = total;
        }
        if (i % bits_per_subblock == 0) {
            uint16_t difference = total - start_block;
            subblock_index.push_back(difference);
        }
        if (finished_LOES[i]) {
            total++;
        }
    }
    /*
    for (int i = 0; i < finished_LOES.size(); i++) {
        assert(compute_rank(i) == simple_rank(i));
    }
    */
}
//computes the rank from position to/with offset.
int LOES::compute_rank(int offset) const {
    //std::cout<<"enters function"<<std::endl;
    const int bits_per_block = 1 << 16;
    const int bits_per_subblock = 512;

    int nr_subblock = offset/bits_per_subblock;
    int nr_block = offset/bits_per_block;

    uint64_t rank = block_index[nr_block] + subblock_index[nr_subblock];
    int start_loop = nr_subblock * bits_per_subblock;

    for (int i = start_loop; i <= offset; i++) {
        if (finished_LOES[i]) {
            rank++;
        }
    }
    return rank;
}

bool LOES::contains_state(const std::vector<bool> &state) const {
	bool result = path_offset(state);
	return result != -1;
}

//adds a new state to the loes (algo 3). states must be added in ascending order
void LOES::add_state(const std::vector<bool> &state){
  int depth;
  if (!has_states) {
    depth = -1;
  } else{
  if (last_state == state) {
    return;
  }
    depth = 0;
    while (state[depth] == last_state[depth]) {
      ++depth;
    }
    treelevels[depth].back() = true;
  }
    for (int i = depth + 1; i <= num_bits - 1; i++) {
      if (state[i]) {
        treelevels[i].push_back(false);
        treelevels[i].push_back(true);
      }else {
        treelevels[i].push_back(true);
        treelevels[i].push_back(false);
      }
    }
    last_state = state;
    has_states = true;
  }

}


