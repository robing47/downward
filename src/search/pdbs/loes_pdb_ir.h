#ifndef DOWNWARD_LOES_PDB_IR_H
#define DOWNWARD_LOES_PDB_IR_H

#include "loes.h"
#include "pattern_database.h"
#include "loes_pdb.h"
#include "types.h"
#include <vector>

class IRLOESPatternDatabase : PatternDatabase {
    std::vector<int> needed_bits;
    int num_bits;
    int default_value;
    LOES loes;

public:
    IRLOESPatternDatabase(Projection &&projection, const std::vector<int> &distances);
    std::vector<bool> index_to_bits(int index) const;
    std::vector<bool> state_to_bits(const std::vector<int> &state) const;
    int get_value(const std::vector<int> &state) const override;

#endif // DOWNWARD_LOES_PDB_IR_H
