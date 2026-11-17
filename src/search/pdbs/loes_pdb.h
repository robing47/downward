#ifndef DOWNWARD_LOES_PDB_H
#define DOWNWARD_LOES_PDB_H

#include "loes.h"
#include "pattern_database.h"
#include "types.h"
#include <vector>

namespace pdbs {
class LOESPatternDatabase : public PatternDatabase {
    std::vector<int> needed_bits;
    int num_bits;
    int default_value;
    LOES loes;

    public:
    LOESPatternDatabase(Projection &&projection, const std::vector<int> &distances);

    std::vector<bool> index_to_bits(int index) const;

    std::vector<bool> state_to_bits(const std::vector<int> &state) const;

    int get_value(const std::vector<int> &state) const override;

    void build_loes(const std::vector<int> &distances);

    std::vector<int> calculate_bits(const Projection &projection);

    int sum_bits(const std::vector<int> &needed_bits);

    int get_default_value(const std::vector<int> &distances) const;

};
}

#endif // DOWNWARD_LOES_PDB_H
