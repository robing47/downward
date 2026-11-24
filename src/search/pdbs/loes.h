#ifndef LOES_H
#define LOES_H
#include <vector>
#include <cstdint>

namespace pdbs{
class LOES{
  	int num_bits;   //length of the bit sequence
  	std::vector<std::vector<bool>> treelevels; //bits per level
  	std::vector<bool> last_state; //the last added state
  	std::vector<bool> finished_LOES;
  	std::vector<int> level_offsets;  //startposition of every level
  	bool has_states; //check if there are states
  	std::vector<int> loes_values;
  	std::vector<uint64_t> block_index;
  	std::vector<uint16_t> subblock_index;
  	void rank_index();

public:
  	int path_offset(const std::vector<bool> &state) const;
  	int member_index(const std::vector<bool> &state) const;
  	void add_state(const std::vector<bool> &state, int value);
  	int simple_rank(int offset) const;
  	void finalize_LOES();
  	std::vector<bool> get_loes();
  	LOES(int num_bits);
  	int get_value(const std::vector<bool> &state, int default_value) const;

	int get_rank_index(){
	return (8*block_index.size()) + (2*subblock_index.size());}

  	//these two getter methods are needed for benchmarking
  	int get_finished_loes_size() const{return finished_LOES.size();}
  	int get_loes_values_size() const{return loes_values.size();}
};}



#endif //LOES_H
