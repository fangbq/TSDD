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
#include "readVerilog.h"
#include "cache_table.h"
#include "unique_table.h"


namespace tsdd {

class TSDD;

class Manager {
public:
    addr_t true_ = -1;
    addr_t false_ = -1;
    Vtree* vtree = NULL;
    addr_t max_addr_for_lit_ = 0;

    // for zsdd trues and complementary
    std::unordered_map<int, addr_t> zsdd_trues_;
    std::unordered_map<int, addr_t> zsdd_falses_;

    // get vtree node depth by index
    std::unordered_map<int, int> depths_by_index;

    // fot getting lca quickly
    std::vector<std::vector<int> > lca_table_;

    UniqueTable uniq_table_;
    CacheTable cache_table_;
public:
    Manager();
    Manager(const Vtree& v, const unsigned int cache_size = 1U << 16);
    void initial_node_table_and_zsdd_trues();
    void initial_depths_by_index(Vtree* v);
    ~Manager();
    addr_t tsddVar(const int var);

    // size of tsdd
    unsigned long long size(const addr_t tsdd_id) const;
    unsigned long long size(const std::unordered_set<addr_t> tsdd_ids) const;

    // operations
    addr_t reduced(const TsddNode& tsdd_node);  // reducing
    TsddNode cofactors(const addr_t tsdd_id, int lca);
    // addr_t generate_bigoplus_piterms(const Vtree& v);

    // inline addr_t get_compl_tmn(const addr_t addr_) const { return addr_^1; }
    inline bool is_terminal(const addr_t addr_) const { return addr_>1 && addr_ <= max_addr_for_lit_; }
    // inline bool is_negative(const addr_t addr_) const { return (addr_ & 1) ==1; }
    // inline bool is_positive(const addr_t addr_) const { return (addr_ & 1) ==0; }
    inline bool is_true(const addr_t addr_) const { return addr_==0; }
    inline bool is_false(const addr_t addr_) const { return addr_==1; }

    addr_t apply(const addr_t lhs, const addr_t rhs, OPERATOR_TYPE op);

    // print node
    void print(const addr_t addr_, int indent = 0) const;
    void print(const TsddNode& tsdd_node, int indent = 0) const;
    void print_tsdd_nodes() const;
    void print_unique_table() const;

    // read cnf file
    addr_t cnf_to_tsdd(const std::string cnf_file, const std::string vtree_file = "");
    std::unordered_set<addr_t> verilog_to_tsdds(char*, const std::string vtree_file = "");
    void output_one_tsdd(logicVar *var);
};

} // namespace tsdd

#endif
