#ifndef LOES_H
#define LOES_H
#include <vector>
namespace pdbs{
class LOES{
  int num_bits;   //length of the bit sequence
  std::vector<std::vector<bool>> treelevels; //bits per level
  std::vector<bool> last_state; //the last added state
  std::vector<bool> finished_LOES;
  std::vector<int> level_offsets;  //startposition of every level
  bool has_states; //check if there are states
  std::vector<int> loes_values;

public:
  int path_offset(const std::vector<bool> &state);
  int member_index(const std::vector<bool> &state);
  void add_state(const std::vector<bool> &state, int value);
  int compute_rank(int offset);
  void finalize_LOES();
  std::vector<bool> get_loes();
  LOES(int num_bits);
  int get_value(const std::vector<bool> &state, int default_value);
};}



#endif //LOES_H
