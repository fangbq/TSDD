#include "manager.h"

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <libgen.h>
#include <chrono>
#include "tsdd.h"


extern std::map<int, int> get_index_by_var;  // (1, 1), (variable number, index), not (value, index)

using namespace std::chrono;

namespace tsdd {

Manager::Manager() {
    // std::cout << "Manager bad initialization !!!" << std::endl;
}

Manager::Manager(const Vtree& v, const unsigned int cache_size) :
    cache_table_(cache_size) {
    initial_constants();
    vtree = new Vtree(v);
    initial_node_table_and_zsdd_trues();
};

void Manager::initial_constants() {
    TsddNode false_node(0);
    uniq_table_.make_or_find(false_node);
    for (int i = 1; i <= vtree->size; ++i) {
        TsddNode true_node(1, i);
        uniq_table_.make_or_find(true_node);
        Tsdd true_(i, i);
        trues_.emplace(i, true_);
        if (vtree->is_leaf(i)) continue;
        Tsdd left_true(vtree->left_child(i), vtree->left_child(i)), right_true(vtree->right_child(i), vtree->right_child(i));
        left_trues_.emplace(i, left_true);
        right_trues_.emplace(i, right_true);
    }
}

void Manager::initial_node_table_and_zsdd_trues() {
    // alread: 0 - false, 1 - true;
    // here: 1*2 - x_1, 1*2+1 - -x_1, ..., 
    for (int i = 1; i <= (vtree->size+1)/2; ++i) {
        TsddNode pos_literal(i*2, get_index_by_var[abs(i)]), neg_literal(i*2+1, get_index_by_var[abs(i)]);
        uniq_table_.make_or_find(pos_literal);
        addr_t neg = uniq_table_.make_or_find(neg_literal);
        // add leaf epsl_ (map below)
        Tsdd epsl(get_index_by_var[i], neg);
        epsl_.emplace(get_index_by_var[i], epsl);  // 'i' here is variable, not vnode
    }
// int node_count = 0;
// for (auto a:uniq_table_.tsdd_nodes_) {
//     std::cout << node_count++ << " @@@@ " << std::endl;
//     print(a);
// }
    var_no_ = (vtree->size+1)/2;
    /// inital this before apply operation (do apply soon below)
    initial_depths_by_index(vtree);

vtree->print();
// vtree->save_dot_file("tree18.dot");
// vtree->save_vtree_file("tree18.vtree");
    // create a map for vtree_node and zsdd epsilon(cpmps)
    for (int i = 1; i <= (vtree->size); ++i) {
        if (vtree->is_leaf(i)) continue;  // leaf'left child added in the above loop
        int vtree_i = vtree->leftmost_index(i);
        TsddNode epsl_node((vtree->leftmost_var(i)*2+1), vtree_i);
        addr_t epsl = uniq_table_.make_or_find(epsl_node);
        Tsdd epsl_tsdd(i, epsl);
        epsl_.emplace(i, epsl_tsdd);
    }
    for (int i = 1; i <= (vtree->size); ++i) {
        Tsdd true_tsdd(i, i);
        Tsdd epsl_tsdd_comp = apply(true_tsdd, epsl_.at(i), XOR);
        epsl_comp_.emplace(i, epsl_tsdd_comp);
    }
// for (auto a:epsl_comp_) {
//     std::cout << a.first << " @@@@ " << std::endl;
//     print(a.second);
// }
std::cout << " init done @@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << std::endl;
// print_unique_table();
    // initial lca_table_
    std::vector<int> v;
    lca_table_.push_back(v);
    for (int i = 1; i <= vtree->size; ++i) {
        std::vector<int> table;
        table.push_back(0);
        for (int j = 1; j <= vtree->size; ++j) {
            table.push_back(vtree->get_lca(i, j));
        }
        lca_table_.push_back(table);
    }

}

void Manager::initial_depths_by_index(Vtree* v) {
    depths_by_index[v->index] = v->depth;
    if (v->lt) initial_depths_by_index(v->lt);
    if (v->rt) initial_depths_by_index(v->rt);
}

Manager::~Manager() {
    if (vtree != NULL) { delete vtree; vtree = NULL; }
};

Tsdd Manager::tsddVar(const int tmp_var) {
    // in the case of the initial tsdd_nodes_[] is settel
    if (tmp_var < 0) {
        Tsdd tsdd_var(get_index_by_var[(-tmp_var)], 2*var_no_-2+ (-tmp_var)*2+1);
        return tsdd_var;
    } else {
        Tsdd tsdd_var(get_index_by_var[tmp_var], 2*var_no_-2+ tmp_var*2);
        return tsdd_var;
    }
}

unsigned long long Manager::size(const Tsdd& tsdd) const {
// std::cout << "size ????????????????" << std::endl;
// print(tsdd);
    if (is_terminal(tsdd)) return 0;

// std::cout << "not terminal >>>>>>>>>>>>>>> " << std::endl;
    std::unordered_set<Tsdd> tsdd_ids;
    std::stack<Tsdd> unexpanded;
    unexpanded.push(tsdd);

    while (!unexpanded.empty()) {
        Tsdd t = unexpanded.top();
        unexpanded.pop();

        tsdd_ids.insert(t);
        const TsddNode& n = uniq_table_.get_node_at(t.addr_);
        if (n.value < 0) {
            for (const Element e : n.elements) {
                if (tsdd_ids.find(e.first) == tsdd_ids.end()) {
                    tsdd_ids.insert(e.first);
                    unexpanded.push(e.first);
                }
                if (tsdd_ids.find(e.second) == tsdd_ids.end()) {
                    tsdd_ids.insert(e.second);
                    unexpanded.push(e.second);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto& i : tsdd_ids) {
        const TsddNode& n = uniq_table_.get_node_at(i.addr_);
        if (n.value < 0) {
            size += n.elements.size();
        }
    }
// std::cout << "size >>>>>>>>>>>>>>> " << size << std::endl;
    return size;
}

Tsdd Manager::reduced(const TsddNode& tsdd_node) {
// std::cout << "reducing..." << std::endl;
    if (tsdd_node.is_terminal()) return *new Tsdd(tsdd_node.vtree_index, uniq_table_.make_or_find(tsdd_node));

    bool sameSub = true;
    Tsdd first_sub = tsdd_node.elements.front().second;
    for (const auto& e : tsdd_node.elements) {
        if (e.second != first_sub)
        {
            sameSub = false;
            break;
        }
    }

    TsddNode result_node;
    Tsdd result_tsdd;
    // 1.2 return false if all elements' subs are false
    if (sameSub) {
        // Trimming: one element, SDD Rule 2, {(1, a)} -> a
        return first_sub;
    }
// print_unique_table();

    // 2 compressing
    result_node = tsdd_node;
    for (auto e1 = result_node.elements.begin(); \
    e1 != result_node.elements.end(); ) {
        bool is_delete = false;
        for (auto e2 = result_node.elements.begin(); \
        e2 != result_node.elements.end(); ++e2) {
// std::cout << "_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+_+" << std::endl;
// std::cout << e1->second.tag_ << " " << e1->second.addr_ << " " << e2->second.tag_ << " " << e2->second.addr_ << std::endl;
// print(e1->second);
// print(e1->second);
// std::cout << (e1 != e2) << " " << (e1->second==e2->second) << std::endl;
            if (e1 != e2 && ((is_true(e1->second) && is_true(e2->second)) || e1->second==e2->second)) {
// std::cout << "they are equal" << std::endl;
                is_delete = true;
                e2->first = apply(e1->first, e2->first, OR);
                e1 = result_node.elements.erase(e1);
                break;
            }
        }
        if (!is_delete)
            ++e1;
    }

    //3 Trimming
// std::cout << "trimming..." << std::endl;
// print(result_node);

    result_tsdd.tag_ = result_node.vtree_index;

    if (result_node.elements.size() == 2) {
        if (!is_false(result_node.elements[0].second)) {
            // swap first
            Element tmp_e = result_node.elements[0];
            result_node.elements[0] = result_node.elements[1];
            result_node.elements[1] = tmp_e;
        }

        if (is_false(result_node.elements[0].second)) {
            // SDD Rule 1: {(¬a, 0), (a, 1)} -> a
// std::cout<<"reduced 1 ---------------------------------- " << is_true(result_node.elements[1].second) << std::endl;
// print(result_node);
            if (is_true(result_node.elements[1].second)) {
                return result_node.elements[1].first;
            }
            int tmp_pri_tag = result_node.elements[1].first.tag_;
            // int tmp_sub_tag = result_node.elements[1].second.tag_;
            TsddNode tmp_pri_ = uniq_table_.tsdd_nodes_.at(result_node.elements[1].first.addr_);
            TsddNode tmp_sub_ = uniq_table_.tsdd_nodes_.at(result_node.elements[1].second.addr_);
// std::cout<<"reduced 2 --------------------------------------------" << std::endl;


            if (result_node.elements[1].first == epsl_.at(vtree->left_child(result_tsdd.tag_))) {
                if (tmp_pri_tag == vtree->left_child(result_tsdd.tag_)) {
// std::cout<<"reduced 2.1 --------------------------------------------" << std::endl;
                    // ZSDD Rule 2a: {(¬𝜀, 0), (𝜀, a)} -> a, t=v^l
                    result_tsdd = result_node.elements[1].second;
                    result_tsdd.tag_ = result_node.vtree_index;
                    return result_tsdd;
                } else {
                    // ZSDD Rule 2b: {(¬𝜀, 0), (𝜀, a)} -> a, t≠v^l
                    result_node.vtree_index = vtree->right_child(result_tsdd.tag_);
                    result_node.elements[1].first = left_trues_.at(result_tsdd.tag_);
                    result_node.elements.erase(result_node.elements.begin());
                }
            } else if (result_node.elements[1].second == epsl_.at(vtree->right_child(result_tsdd.tag_))) {
// std::cout<<"reduced 3 --------------------------------------------" << std::endl;
                if (tmp_pri_tag == vtree->left_child(result_tsdd.tag_)) {
                    // ZSDD Rule 1a: {(¬a, 0), (a, 𝜀)} -> a, t=v^l
                    result_tsdd = result_node.elements[1].first;
                    result_tsdd.tag_ = result_node.vtree_index;
                    return result_tsdd;
                } else {
                    // ZSDD Rule 1b: {(¬a, 0), (a, 𝜀)} -> a, t≠v^l
                    result_node.vtree_index = vtree->left_child(result_tsdd.tag_);
                    result_node.elements[1].second = right_trues_.at(result_tsdd.tag_);
                }
            }
        }
    }

    result_tsdd.addr_ = uniq_table_.make_or_find(result_node);
    return result_tsdd;
}

TsddNode Manager::cofactors(const Tsdd& tsdd, int lca) {
// cout << "cofactors..." << endl;
    if (is_true(tsdd)) {
        TsddNode new_node;
        new_node.vtree_index = lca;
        Element e1;
        e1.first = left_trues_.at(lca);
        e1.second = right_trues_.at(lca);
        new_node.elements.push_back(e1);
        return new_node;
    }
    TsddNode tsdd_node = uniq_table_.tsdd_nodes_.at(tsdd.addr_), new_node;
    if (lca == tsdd_node.vtree_index) return tsdd_node;  // ??? right?
    int tsdd_tag = tsdd.tag_;
    new_node.vtree_index = lca;
// std::cout<<"cofactors 1 --------------------------------------------" << std::endl;
// std::cout<< depths_by_index[lca] << " " << depths_by_index[tsdd_tag] << std::endl;
// set "vtree_index" in apply algorithms, but no here
// print(tsdd);

    if (depths_by_index[lca] < depths_by_index[tsdd_tag]) {
// std::cout<<"cofactors 1.1 --------------------------------------------" << std::endl;
        if (tsdd_tag < lca) {
            // SDD Rule 1
            Element e1;
            e1.first = tsdd;
            // int true_tag = vtree->right_child(lca);
            e1.second = right_trues_.at(lca);
            new_node.elements.push_back(e1);
            if (!is_true(e1.first)) {
                Element e2;
                e2.first = apply(trues_.at(tsdd.tag_), e1.first, XOR);
                e2.second = false_;
                new_node.elements.push_back(e2);
            }
        } else if (tsdd_tag > lca) {
            // SDD Rule 2
            Element e;
            e.first = left_trues_.at(lca);
            e.second = tsdd;
            new_node.elements.push_back(e);
        }
    } else if (tsdd_node.is_one()) {
// std::cout<<"cofactors 1.2 --------------------------------------------" << std::endl;
        Element e1;
        e1.first = epsl_.at(vtree->left_child(lca));
        e1.second = epsl_.at(vtree->right_child(lca));
        new_node.elements.push_back(e1);
        if (is_true(e1.first)) return new_node;
        Element e2;
        e2.first = apply(left_trues_.at(lca), e1.first, XOR);
        e2.second = false_;
        new_node.elements.push_back(e2); 
    } else if (tsdd_node.vtree_index < lca) {
// std::cout<<"cofactors 2 --------------------------------------------" << std::endl;
        if (tsdd_node.vtree_index == vtree->left_child(lca) \
        && tsdd_node.elements.size() == 2) {
// std::cout<<"cofactors 2.3 --------------------------------------------" << std::endl;
            // ZSDD Rule 1b
            new_node.elements = tsdd_node.elements;
            if (is_true(new_node.elements[0].second) \
            && is_false(new_node.elements[1].second)) {
                new_node.elements[0].second = epsl_.at(vtree->right_child(lca));
                return new_node;
            } else if (is_true(new_node.elements[1].second) \
            && is_false(new_node.elements[0].second)) {
                new_node.elements[1].second = epsl_.at(vtree->right_child(lca));
                return new_node;
            }
        }
// std::cout<<"cofactors 3 --------------------------------------------" << std::endl;
        // ZSDD Rule 1a
        Element e1, e2;
        e1.first = tsdd;
        e1.first.tag_ = vtree->left_child(lca);
// std::cout<<"cofactors 3.3 -------------------------------------------- " << vtree->right_child(lca) << std::endl;
//     for (auto a : epsl_) {
//         std::cout<< a.first << std::endl;
//     }
        e1.second = epsl_.at(vtree->right_child(lca));
        new_node.elements.push_back(e1);
        e2.first = apply(left_trues_.at(lca), e1.first, XOR);
        e2.second = false_;
        new_node.elements.push_back(e2);
        
// std::cout<<"cofactors 4 --------------------------------------------" << std::endl;
    } else if (tsdd_node.vtree_index > lca) {
// std::cout<<"cofactors 5 --------------------------------------------" << std::endl;
        if (tsdd_node.vtree_index == vtree->right_child(lca) \
        && tsdd_node.elements.size() == 1) {
            // ZSDD Rule 2b
            Element e1, e2;
            e1.first = epsl_.at(vtree->left_child(lca));
            e1.second = tsdd_node.elements[0].second;
            new_node.elements.push_back(e1);

            e2.first = epsl_comp_.at(vtree->left_child(lca));
            e2.second = false_;
            new_node.elements.push_back(e2);
        } else {
// std::cout<<"cofactors 6 --------------------------------------------" << std::endl;
            // ZSDD Rule 2a
            Element e1, e2;
            e1.first = epsl_.at(vtree->left_child(lca));
// std::cout<<"cofactors 6.1 --------------------------------------------" << std::endl;
            e1.second = tsdd;
            e1.second.tag_ = vtree->right_child(lca);
// std::cout<<"cofactors 6.2 --------------------------------------------" << std::endl;
            new_node.elements.push_back(e1);

            e2.first = epsl_comp_.at(vtree->left_child(lca));
// std::cout<<"cofactors 6.3 --------------------------------------------" << std::endl;
            e2.second = false_;
            new_node.elements.push_back(e2);
        }
    } else {
        std::cerr << "[MyError] cofactors error" << std::endl;
    }
    return new_node;
}

Tsdd Manager::apply(const Tsdd& lhs_tsdd, const Tsdd& rhs_tsdd, OPERATOR_TYPE op, int indent) {
// cout << "apply..." << endl;
    if (rhs_tsdd < lhs_tsdd) {
        return apply(rhs_tsdd, lhs_tsdd, op, indent);
    }

    int lhs_tag = lhs_tsdd.tag_;
    int rhs_tag = rhs_tsdd.tag_;
    addr_t lhs = lhs_tsdd.addr_;
    addr_t rhs = rhs_tsdd.addr_;


    // trivial(constant) case
    switch (op) {
        case AND:
            if (is_false(lhs_tsdd)) return lhs_tsdd;
            if (is_false(rhs_tsdd)) return rhs_tsdd;
            if (is_true(lhs_tsdd)) return rhs_tsdd;
            if (is_true(rhs_tsdd)) return lhs_tsdd;
            if (lhs_tsdd == rhs_tsdd) return lhs_tsdd;
            break;
        case OR:
            if (is_false(lhs_tsdd)) return rhs_tsdd;
            if (is_false(rhs_tsdd)) return lhs_tsdd;
            if (is_true(lhs_tsdd)) return lhs_tsdd;
            if (is_true(rhs_tsdd)) return rhs_tsdd;
            if (lhs_tsdd == rhs_tsdd) return lhs_tsdd;
            break;
        case XOR:
            if (is_false(lhs_tsdd)) return rhs_tsdd;
            if (is_false(rhs_tsdd)) return lhs_tsdd;
            if (lhs_tsdd == rhs_tsdd) return false_;
            break;
        default:
            std::cerr << "[MyError] apply error 1" << std::endl;
            return false_;
    }

    Tsdd cache = cache_table_.read_cache(op, lhs_tsdd, rhs_tsdd);
    if (cache != empty_)
        return cache;

for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout<<"apply ========================================== " << op << std::endl;
print(lhs_tsdd, indent);
for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout<<"apply with --------------------------------------------" << std::endl;
print(rhs_tsdd, indent);

    TsddNode new_node;
    TsddNode normalized_node1 = uniq_table_.tsdd_nodes_[lhs], normalized_node2 = uniq_table_.tsdd_nodes_[rhs];
    
    int min_tag = vtree->get_lca(lhs_tag, rhs_tag), lca;
    if (lhs_tag == rhs_tag)
        lca = vtree->get_lca(normalized_node1.vtree_index, normalized_node2.vtree_index);
    else
        lca = min_tag;
// std::cout<<"lca: " << lca << std::endl;
    
    if (normalized_node1.vtree_index != lca || normalized_node2.vtree_index != lca \
        || !normalized_node1.is_terminal() || !normalized_node2.is_terminal()) {
        // when not computable, both normalization
        normalized_node1 = cofactors(lhs_tsdd, lca);
for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout<<"normalized_tsdd 1 --------------------------------------------" << std::endl;
print(normalized_node1, indent);
        normalized_node2 = cofactors(rhs_tsdd, lca);
for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout<<"normalized_tsdd 2 --------------------------------------------" << std::endl;
print(normalized_node2, indent);
    }

    new_node.vtree_index = lca;

    if (normalized_node1.is_terminal() && normalized_node2.is_terminal()) {
    // how to deal with tags for terminal, terminal should be processed otherwhere???
        switch (op) {
            case AND:
                if (normalized_node1.is_zero() || normalized_node2.is_zero()) new_node.value=0;
                else if (normalized_node1.is_one()) new_node.value=normalized_node2.value;
                else if (normalized_node2.is_one()) new_node.value=normalized_node1.value;
                else if (normalized_node1.value==normalized_node2.value) new_node.value=normalized_node1.value;
                else new_node.value=0;
                break;
            case OR:
                if (normalized_node1.is_zero()) new_node.value=normalized_node2.value;
                else if (normalized_node2.is_zero()) new_node.value=normalized_node1.value;
                else if (normalized_node1.is_one() || normalized_node2.is_one() || (normalized_node1.value != normalized_node2.value)) new_node.value=1;
                else new_node.value=normalized_node1.value;
                break;
            case XOR:
                if (normalized_node1.is_zero()) new_node.value=normalized_node2.value;
                else if (normalized_node2.is_zero()) new_node.value=normalized_node1.value;
                else if (normalized_node1.value == normalized_node2.value) new_node.value=0;
                else if (normalized_node1.is_one()) new_node.value=normalized_node2.value^1;
                else if (normalized_node2.is_one()) new_node.value=normalized_node1.value^1;
                else new_node.value=1;
                break;
            default:
                std::cerr << "[MyError] apply error 2" << std::endl;
        }
    } else {
        for (std::vector<Element>::const_iterator e1 = normalized_node1.elements.begin();
        e1 != normalized_node1.elements.end(); ++e1) {
            for (std::vector<Element>::const_iterator e2 = normalized_node2.elements.begin();
            e2 != normalized_node2.elements.end(); ++e2) {
                Element new_e;
                new_e.first = apply(e1->first, e2->first, AND, indent+4);
                if (!is_false(new_e.first)) {
                    new_e.second = apply(e1->second, e2->second, op, indent+4);
                    new_node.elements.push_back(new_e);
                }
            }
        }
    }

for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout << "before reduce got node ---------------" << std::endl;
print(new_node, indent);

    // for false node, because <(t, false)> = false.
    if (new_node.is_zero()) return false_;

    Tsdd new_tsdd = reduced(new_node);

for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout << "after reduce got node ---------------" << std::endl;
print(new_tsdd, indent);

    // extra case, <(\alpha, false), (\beta, false)> = false.
    if (new_tsdd.addr_==0) return false_;

// std::cout << min_tag << " " << lca << std::endl;

    if (min_tag != lca) {
        new_tsdd.tag_ = min_tag;
    }

for (int i = 0; i < indent; ++i) std::cout << " ";
std::cout << "new tsdd with new tag ------------" << std::endl;
print(new_tsdd, indent);

    cache_table_.write_cache(op, lhs_tsdd, rhs_tsdd, new_tsdd);
    return new_tsdd;
}

void Manager::print(const Tsdd& tsdd, int indent) const {
    TsddNode tsdd_node = uniq_table_.get_node_at(tsdd.addr_);
    int tsdd_tag = tsdd.tag_;
    if (tsdd_node.elements.empty()) {
        for (int i = 0; i < indent; ++i) std::cout << " ";
        if (tsdd_node.value < 2) {
            // std::cout << tsdd_node.value << std::endl;
            std::cout << "-" << tsdd_tag << "- " << tsdd_node.value << " ~ " << tsdd_node.vtree_index << std::endl;
        } else {
            std::cout << "-" << tsdd_tag << "- ";
            if (tsdd_node.is_negative()) std::cout << "-";
            // std::cout << "x" << tsdd_node.value/2 << std::endl;
            std::cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << std::endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "-" << tsdd_tag << "- Dec " << tsdd_node.vtree_index << ":" << std::endl;
    int counter = 1;
    indent++;
    for (const auto e : tsdd_node.elements) {
        for (int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "E" << counter << "p:" << std::endl;
        print(e.first, indent+1);
        for (int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "E" << counter++ << "s:" << std::endl;
        print(e.second, indent+1);
    }
    return;
}

void Manager::print(const TsddNode& tsdd_node, int indent) const {
    if (tsdd_node.elements.empty()) {
        for (int i = 0; i < indent; ++i) std::cout << " ";
        if (tsdd_node.value < 2) {
            // std::cout << tsdd_node.value << std::endl;
            std::cout << "-no-tag- " << tsdd_node.value << " ~ " << tsdd_node.vtree_index << std::endl;
        } else {
            std::cout << "-no-tag- ";
            if (tsdd_node.is_negative()) std::cout << "-";
            // std::cout << "x" << tsdd_node.value/2 << std::endl;
            std::cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << std::endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "-no-tag- Dec " << tsdd_node.vtree_index << ":" << std::endl;
    int counter = 1;
    indent++;
    for (const auto& e : tsdd_node.elements) {
        for (int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "E" << counter << "p:" << std::endl;
        print(e.first, indent+1);
        for (int i = 0; i < indent; ++i) std::cout << " ";
        std::cout << "E" << counter++ << "s:" << std::endl;
        print(e.second, indent+1);
    }
    return;
}

void Manager::print_tsdd_nodes() const {
    std::cout << "tsdd_nodes_:-------------------------------" << std::endl;
    int i = 0;
    for (auto& tsdd_node: uniq_table_.tsdd_nodes_) {
        std::cout << "Node " << i++ << ":" << std::endl;
        print(tsdd_node);
        std::cout << std::endl;
    }
}

void Manager::print_unique_table() const {
    std::cout << "unique_table:-------------------------------" << std::endl;
    for (auto& x: uniq_table_.uniq_table_) {
        std::cout << "Node addr_t: " << x.second << std::endl;
        print(x.first);
        std::cout << std::endl;
    }
}
int factorial(int n)
{
    if(n > 1)
        return n * factorial(n - 1);
    else
        return 1;
}
Tsdd Manager::cnf_to_tsdd(const std::string cnf_file, const std::string vtree_file) {
    // read *.cnf
    std::ifstream infile(cnf_file, std::ios::in);
    std::string file_name = static_cast<std::string>(cnf_file);
    std::cout << file_name.substr(file_name.find_last_of("/")+1, file_name.length()-file_name.find_last_of("/")) << "\t";
    if(!infile)
    {
        std::cerr << "open infile error!" << std::endl;
        exit(1);
    }
    int var_no = 0, col_no = 0;  // Number of variables and number of clauses
    std::string line;
    while (!infile.eof()) {
        std::getline(infile, line);

        if (line.length() == 0 || line[0] == 'c')
            ; //cout << "IGNORE LINE\n";
        else {
            var_no = stoi(line.substr(6, line.find_last_of(' ')-6));  //  p cnf a b; this line can't end with ' '
            col_no = stoi(line.substr(line.find_last_of(' ')+1, line.length()-line.find_last_of(' ')));
            break;
        }
    }
    std::cout << var_no << "  " << col_no << "    ";
    std::ifstream vtreefile(vtree_file, std::ios::in);
    if (!vtreefile || vtree_file.empty()) {
        std::vector<int> vars_order;
        for (int i = 1; i <= var_no; ++i) vars_order.push_back(i);
        vtree = new Vtree(1, var_no*2-1, vars_order, RANDOM_TREE);
    } else {
        vtree = new Vtree(vtree_file);
    }

    VTREE_SIZE = vtree->size;
    initial_constants();
    initial_node_table_and_zsdd_trues();

    // v.save_vtree_file("s27_ balanced.vtree");

    // make tsdd literal by literal
    Tsdd fml = trues_.at(1);
    int  clause_counter = 1;
    std::vector<Tsdd> clause_tsdds;
    for(int line = 0; line < col_no; ++line)  //read every line number, and save as a clause
    {
        Tsdd clause = false_;
        while (true) {
            int var;
            infile >> var;
            if (var == 0) break;
            Tsdd tmp_var = tsddVar(var);
// std::cout << "var : " << var << " starting... " << size(fml) << std::endl;
            clause = apply(clause, tmp_var, OR);
// print(clause);
// std::cout << "var : " << var << " done; " << size(fml) << std::endl;
        }
// std::cout << "clause : " << clause_counter << " starting... " << size(fml) << std::endl;

// std::cout<<"apply AND ============================" << std::endl;
// print(fml);
// std::cout<<"fml apply with clause ----------------" << std::endl;
// print(clause);
        fml = apply(fml, clause, AND);
// std::cout<<"get new fml --------------------------" << std::endl;
// print(fml);

        clause_tsdds.push_back(clause);
std::cout << "clause : " << clause_counter++ << " done; " << size(fml) << std::endl;
    }



// // print_unique_table();

// sort(clause_tsdds.begin(), clause_tsdds.end());
// int i = 0;
// do {
//     Tsdd tmp_fml = trues_.at(1);
//     for (auto a : clause_tsdds) {
// // std::cout<<"apply AND ============================" << std::endl;
// // print(tmp_fml);
// // std::cout<<"fml apply with clause ----------------" << std::endl;
// // print(a);

// // std::cout << "clause : " << a.tag_ << " " << a.addr_ << std::endl;
//         tmp_fml = apply(fml, a, AND);
// // std::cout<<"get new fml --------------------------" << std::endl;
// // print(fml);
//     }
// std::cout << "permutation : \t" << i++ << "; size: " << size(tmp_fml) << std::endl;
// } while (std::next_permutation(clause_tsdds.begin(), clause_tsdds.end()));





print(fml);
// print_unique_table();


    return fml;

    while (clause_tsdds.size() > 1) {
        std::vector<Tsdd> tmp_tsdds;
        for (int i = 0; i < (int)(clause_tsdds.size() + 1) / 2; i++) {
            if (2 * i + 1 >= (int)clause_tsdds.size()) {
                tmp_tsdds.push_back(clause_tsdds[2*i]);
            }
            else {
                tmp_tsdds.push_back(apply(clause_tsdds[2*i], clause_tsdds[2*i+1], AND));
            }
        }
        clause_tsdds = tmp_tsdds;
    }
    return clause_tsdds[0];
}

} // namespace tsdd
