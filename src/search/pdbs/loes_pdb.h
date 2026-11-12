#ifndef DOWNWARD_LOES_PDB_H
#define DOWNWARD_LOES_PDB_H

#include "loes.h"
#include "pattern_database.h"
#include "types.h"
#include <vector>
#include <memory>

namespace pdbs {
class LOESPatternDatabase {
    Projection projection;
    int num_bits;
    int default_value;
    std::vector<int> needed_bits; //bits per pattern variable
    std::unique_ptr<LOES> loes;

    public:

    LOESPatternDatabase(Projection &&projection, const std::vector<int> &distances);

    std::vector<bool> index_to_bits(int index) const;

    std::vector<bool> state_to_bits(const std::vector<int> &state) const;

    int get_value(const std::vector<int> &state) const;

    void build_loes(const std::vector<int> &distances);

    void calculate_bits();


    const Pattern &get_pattern() const {
        return projection.get_pattern();
    }

    int get_size() const {
        return projection.get_num_abstract_states();
    }

    int get_default_value(const std::vector<int> &distances) const;

    int get_multiplier(int var) {
        return projection.get_multiplier(var);
    }

    int get_domain_size(int var) const{
        return projection.get_domain_size(var);
    }


};
}

#endif // DOWNWARD_LOES_PDB_H
