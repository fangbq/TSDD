#ifndef MANAGER_H
#define MANAGER_H

#include "define.h"
#include "tsdd_node.h"
#include "tsdd_vtree.h"
#include <unordered_map>
#include <vector>
#include <stack>
#include <functional>
#include <map>
#include <set>
#include "cache_table.h"
#include "unique_table.h"
// #include "tsdd.h"

using namespace std;

namespace tsdd {

class Manager {
public:
    Tsdd empty_ = *new Tsdd(-1, -1);  // for missing in read_cache
    Tsdd false_ = *new Tsdd(0, 0);  // false_(0, 0); this kind of initialization ok?!!!
    Tsdd true_ = *new Tsdd(0, 1);  // true_(0, 1); this kind of initialization ok?!!!
    Vtree* vtree = NULL;
    int var_no_ = 0;

    std::set<int> left_most_vars_;

    // for standalization in ::reduce
    std::unordered_map<int, addr_t> true_nodes_;

    // for zsdd epsilons and complementary
    std::unordered_map<int, Tsdd> epsl_;
    std::unordered_map<int, Tsdd> epsl_comp_;

    // get vtree node depth by index
    std::unordered_map<int, int> depths_by_index;
    std::unordered_map<int, int> sibling_of;

    // fot getting lca quickly
    std::vector<std::vector<int> > lca_table_;

    UniqueTable uniq_table_;
    CacheTable cache_table_;
public:
    Manager();
    Manager(const Vtree& v, const unsigned int cache_size = 1U << 16);
    void initial_constants();
    void initial_node_table_and_zsdd_trues();
    void initial_depths_by_index(Vtree* v);
    ~Manager();
    Tsdd tsddVar(const int var);

    // size of tsdd
    unsigned long long size(const Tsdd& tsdd) const;
    unsigned long long size_implicit(const Tsdd& tsdd) const;
    // unsigned long long size(const std::unordered_set<Tsdd> tsdd_ids) const;

    Tsdd reduced(const TsddNode& tsdd_node, int new_tag);  // reducing
    TsddNode cofactors(const Tsdd& tsdd, int lca);
    // addr_t generate_bigoplus_piterms(const Vtree& v);

    // inline addr_t get_compl_tmn(const addr_t addr_) const { return addr_^1; }
    inline bool is_terminal(const Tsdd& tsdd) const { return is_constant(tsdd)||tsdd.tag_%2==1; }
    inline bool is_constant(const Tsdd& tsdd) const { return is_true(tsdd)||is_false(tsdd); }
    inline bool is_true(const Tsdd& tsdd) const {
        if (tsdd == true_) return true;
        const TsddNode& n_ = get_tsddnode_at(tsdd.addr_);
        // need optimization!!!
        if (n_.value==1 && n_.vtree_index==tsdd.tag_) {
            // cout << "there are other true!!!" << endl;
            return true;
        }
        // if (tsdd.tag_ == true_nodes_.at(tsdd.tag_)) {
        //     cout << "there are other true!!!" << endl;
        //     print(tsdd);
        //     return true;
        // }
        return false;
    }
    inline bool is_false(const Tsdd& tsdd) const { return tsdd==false_; }

    // operations
    Tsdd negation(const Tsdd& tsdd, int indent=0);
    Tsdd apply(const Tsdd& lhs_tsdd, const Tsdd& rhs_tsdd, OPERATOR_TYPE op, int indent=0);

    // print node
    void print(const Tsdd& tsdd, int indent = 0) const;
    void print(const TsddNode& tsdd_node, int indent = 0) const;
    void print_tsdd_nodes() const;
    void print_unique_table() const;
    void export_tsdd_dot(const addr_t tsdd_addr, std::ostream& os) const;

    // read cnf file
    Tsdd cnf_to_tsdd(const std::string cnf_file, const std::string vtree_file = "");

    const TsddNode& get_tsddnode_at(const addr_t idx) const {
        return uniq_table_.get_node_at(idx);
    }
};

} // namespace tsdd

#endif
