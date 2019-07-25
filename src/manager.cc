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
using namespace std;

namespace tsdd {

Manager::Manager() {
    // cout << "Manager bad initialization !!!" << endl;
}

Manager::Manager(const Vtree& v, const unsigned int cache_size) :
    cache_table_(cache_size) {
    initial_constants();
    vtree = new Vtree(v);
    initial_node_table_and_zsdd_trues();
};

void Manager::initial_constants() {
    TsddNode false_node(0, 0);
    TsddNode true_node(1, 0);
    uniq_table_.make_or_find(false_node);
    uniq_table_.make_or_find(true_node);
}

void Manager::initial_node_table_and_zsdd_trues() {
    var_no_ = (vtree->size+1)/2;
    // alread: 0 - false, 1 - true;
    // here: 1*2 - x_1, 1*2+1 - -x_1, ..., 
    for (int i = 1; i <= var_no_; ++i) {
        // no need starting from leftest variable
        TsddNode pos_literal(i*2, get_index_by_var[i]), neg_literal(i*2+1, get_index_by_var[i]);
        uniq_table_.make_or_find(pos_literal);
        addr_t neg = uniq_table_.make_or_find(neg_literal);
        // add leaf epsl_ (map below)
        Tsdd epsl(get_index_by_var[i], neg);
        epsl_.emplace(get_index_by_var[i], epsl);  // 'i' here is variable, not vnode
    }
// int node_count = 0;
// for (auto a:uniq_table_.tsdd_nodes_) {
//     cout << node_count++ << " @@@@ " << endl;
//     print(a);
// }
    /// inital this before apply operation (do apply soon below)
    initial_depths_by_index(vtree);

// cout << " init done 1 @@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << endl;
// print_tsdd_nodes();

// vtree->print();
// vtree->save_dot_file("tree3.dot");
// vtree->save_vtree_file("tree3.vtree");
    // create a map for vtree_node and zsdd epsilon(cpmps)
    for (int i = 1; i <= (vtree->size); ++i) {
        // initial true_nodes_
        TsddNode true_node(1, i);
        addr_t addr_ = uniq_table_.make_or_find(true_node);
        true_nodes_[i] = addr_;

        if (vtree->is_leaf(i)) continue;  // leaf'left child added in the above loop
        int vtree_i = vtree->leftmost_index(i);
        int vtree_var = vtree->leftmost_var(i);
        left_most_vars_.insert(vtree_var);
        TsddNode epsl_node(vtree_var*2+1, vtree_i);
// cout << "before add  >>>>>>>>>>>>>>> " << endl;
// print_tsdd_nodes();
        addr_t epsl = uniq_table_.make_or_find(epsl_node);
// cout << "seems adding |||||||||||||| " << epsl << endl;
// print(epsl_node);
// print_tsdd_nodes();
// cout << "add done    <<<<<<<<<<<<<<< " << endl;
        Tsdd epsl_tsdd(i, epsl);
        epsl_.emplace(i, epsl_tsdd);
    }
    for (int i = 1; i <= (vtree->size); ++i) {
// cout << "before add  >>>>>>>>>>>>>>> : " << i << endl;
        Tsdd epsl_tsdd_comp = negation(epsl_.at(i));
        epsl_comp_.emplace(i, epsl_tsdd_comp);
    }
// for (auto a:epsl_) {
//     cout << a.first << " @@@@ " << endl;
//     print(a.second); 
// }
// cout << " init done 2 @@@@@@@@@@@@@@@@@@@@@@@@@@@@ " << endl;
// print_tsdd_nodes();
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

// cout << endl << "[initial_node_table_and_zsdd_trues] init done." << endl;
}

void Manager::initial_depths_by_index(Vtree* v) {
    depths_by_index[v->index] = v->depth;
    if (v->lt) {
        initial_depths_by_index(v->lt);
        if (!v->rt) cout << "[initial_depths_by_index]: impossible!!! " << endl;
        initial_depths_by_index(v->rt);
        sibling_of[v->lt->index] = v->rt->index;
        sibling_of[v->rt->index] = v->lt->index;
    }
}

Manager::~Manager() {
    if (vtree != NULL) { delete vtree; vtree = NULL; }
};

Tsdd Manager::tsddVar(const int tmp_var) {
    // in the case of the initial tsdd_nodes_[] is settle
    Tsdd tsdd_var(get_index_by_var[abs(tmp_var)], tmp_var < 0 ? (-tmp_var)*2+1 : tmp_var*2);
    return tsdd_var;
}

unsigned long long Manager::size(const Tsdd& tsdd) const {
// cout << "size ????????????????" << endl;
// print(tsdd);
    if (is_terminal(tsdd)) return 0;

// cout << "not terminal >>>>>>>>>>>>>>> " << endl;
    std::unordered_set<addr_t> node_ids;
    std::stack<addr_t> unexpanded;
    unexpanded.push(tsdd.addr_);

    while (!unexpanded.empty()) {
        addr_t t = unexpanded.top();
        unexpanded.pop();

        node_ids.insert(t);
        const TsddNode& n = get_tsddnode_at(t);
        if (n.value < 0) {
            for (const Element e : n.elements) {
                if (node_ids.find(e.first.addr_) == node_ids.end()) {
                    node_ids.insert(e.first.addr_);
                    unexpanded.push(e.first.addr_);
                }
                if (node_ids.find(e.second.addr_) == node_ids.end()) {
                    node_ids.insert(e.second.addr_);
                    unexpanded.push(e.second.addr_);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto& i : node_ids) {
        const TsddNode& n = get_tsddnode_at(i);
        if (n.value < 0) {
            size += n.elements.size();
        }
    }
// cout << "size >>>>>>>>>>>>>>> " << size << endl;
    return size;
}

unsigned long long Manager::size_implicit(const Tsdd& tsdd) const {
    if (is_terminal(tsdd)) return 0;

// cout << "not terminal >>>>>>>>>>>>>>> " << endl;
    std::unordered_set<addr_t> node_ids;
    std::stack<addr_t> unexpanded;
    unexpanded.push(tsdd.addr_);

    while (!unexpanded.empty()) {
        addr_t t = unexpanded.top();
        unexpanded.pop();

        node_ids.insert(t);
        const TsddNode& n = get_tsddnode_at(t);
        if (n.value < 0) {
            for (const Element e : n.elements) {
                if (e.second == false_) continue;
                if (node_ids.find(e.first.addr_) == node_ids.end()) {
                    node_ids.insert(e.first.addr_);
                    unexpanded.push(e.first.addr_);
                }
                if (node_ids.find(e.second.addr_) == node_ids.end()) {
                    node_ids.insert(e.second.addr_);
                    unexpanded.push(e.second.addr_);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto& i : node_ids) {
        const TsddNode& n = get_tsddnode_at(i);
        if (n.value < 0) {
            bool exist_false = false;
            for (const auto& e : n.elements) {
                if (e.second == false_) exist_false = true;
            }
            size += n.elements.size();
            if (exist_false) size--;
        }
    }
// cout << "size >>>>>>>>>>>>>>> " << size << endl;
    return size;
}

Tsdd Manager::reduced(const TsddNode& tsdd_node, int new_tag) {
// cout << "reducing..." << endl;
// print(tsdd_node);
    if (tsdd_node.is_terminal()) {
// cout << "before add  >>>>>>>>>>>>>>> " << endl;
// print_tsdd_nodes();
        if (tsdd_node.value>1 && tsdd_node.value%2==1) {
            return tsddVar(-(tsdd_node.value/2));
        }
        Tsdd tmp_tsdd(tsdd_node.vtree_index, uniq_table_.make_or_find(tsdd_node));
        return tmp_tsdd;
    }

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
// cout << "reducing 1..." << endl;
        if (first_sub == false_) return false_;
        if (is_true(first_sub)) {
            Tsdd true_tsdd(tsdd_node.vtree_index, true_nodes_.at(tsdd_node.vtree_index));
            return true_tsdd;
        }
        if (tsdd_node.vtree_index != new_tag) {
        // for separation when tag != vtree_index, and return a lower tsdd by SDD rule
        // not trimmed but need compressed
            if (first_sub == epsl_.at(vtree->right_child(tsdd_node.vtree_index))) {
                // for jumping standalization
                Tsdd true_tsdd(tsdd_node.vtree_index, true_nodes_.at(vtree->left_child(tsdd_node.vtree_index)));
                return true_tsdd;
            }
            result_node.vtree_index = tsdd_node.vtree_index;
            Element e;
            e.first = true_;
            e.second = first_sub;
            result_node.elements.push_back(e);
            result_tsdd.tag_ = new_tag;
            result_tsdd.addr_ = uniq_table_.make_or_find(result_node);
            return result_tsdd;
        }
        return first_sub;
    }
// print_unique_table();

    // 2 compressing
    result_node = tsdd_node;
// cout << "before compressing ---------------" << endl;
// print(result_node);
    for (auto e1 = result_node.elements.begin(); \
    e1 != result_node.elements.end(); ) {
        bool is_delete = false;
        for (auto e2 = result_node.elements.begin(); \
        e2 != result_node.elements.end(); ++e2) {
            if (e1 != e2 && e1->second==e2->second) {
// cout << "they are equal" << endl;
                is_delete = true;
// cout << "[compressing] prime1 or with ---------------" << endl;
// print(e1->first);
// cout << "[compressing] prime2 ---------------" << endl;
// print(e2->first);
                e2->first = apply(e1->first, e2->first, OR);
// cout << "[compressing] got ---------------" << endl;
// print(e2->first);
                e1 = result_node.elements.erase(e1);
                break;
            }
        }
        if (!is_delete)
            ++e1;
    }
// cout << "after compressing ---------------" << endl;
// print(result_node);

    //3 Trimming
// cout << "trimming..." << endl;
// print(result_node);

    result_tsdd.tag_ = result_node.vtree_index;

    if (result_node.elements.size() == 2) {
        if (!is_false(result_node.elements[0].second)) {
            // swap firstly, so that {( ,0), (, )}
            Element tmp_e = result_node.elements[0];
            result_node.elements[0] = result_node.elements[1];
            result_node.elements[1] = tmp_e;
        }

        if (is_false(result_node.elements[0].second)) {
// cout<<"reduced 1 ---------------------------------- " << is_true(result_node.elements[1].second) << endl;
// print(result_node);
            if (is_true(result_node.elements[1].second)) {
            // SDD Rule 1: {(¬a, 0), (a, 1)} -> a
// cout << "SDD Rule 1  >>>>>>>>>>>>>>> " << endl;
// cout << result_node.vtree_index << " " << new_tag << endl;
                if (result_node.vtree_index != new_tag) {
                // for separation when tag != vtree_index, and return a lower tsdd by SDD rule
                // compressed but not trimmed
                    if (result_node.elements[1].first == epsl_.at(vtree->left_child(tsdd_node.vtree_index))) {
                        // for jumping standalization
                        Tsdd true_tsdd(tsdd_node.vtree_index, true_nodes_.at(vtree->right_child(tsdd_node.vtree_index)));
                        return true_tsdd;
                    }
                    result_tsdd.tag_ = new_tag;
                    result_tsdd.addr_ = uniq_table_.make_or_find(result_node);
                    return result_tsdd;
                }
// cout << "no seperation  >>>>>>>>>>>>>>> " << endl;
                return result_node.elements[1].first;
            }

// cout<<"reduced 2 --------------------------------------------" << endl;

            if (result_node.elements[1].second == epsl_.at(vtree->right_child(result_node.vtree_index))) {
                int tmp_pri_tag = result_node.elements[1].first.tag_;
                if (tmp_pri_tag == vtree->left_child(result_node.vtree_index)) {
                // ZS-DD Rule 1a: {(¬a, 0), (a, 𝜀)} -> a, t=v^l
                    result_tsdd = result_node.elements[1].first;
                    result_tsdd.tag_ = result_node.vtree_index;
                    return result_tsdd;
                } else if (tmp_pri_tag < vtree->left_child(result_node.vtree_index)) {
                // ZS-DD Rule 1b-l: {(¬a, 0), (a, 𝜀)} -> a, t<v^l
// cout << "ZS-DD Rule 1b-l >>>>>>>>>>>>>>> " << endl;
                    int origin_vtree_index = result_node.vtree_index;
                    result_node.vtree_index = vtree->left_child(result_node.vtree_index);  // v' <- v^l

                    Tsdd t_alpha = result_node.elements[1].first;
                    if (t_alpha == epsl_.at(vtree->left_child(result_node.vtree_index))) {
                        // standalization for epsl(aka. non-terminal cases)
                        Tsdd std_epsl(origin_vtree_index, \
                            true_nodes_.at(vtree->right_child(result_node.vtree_index)));  // -v'- 1 ~ v^lr
// cout << "ZS-DD Rule 1b-l standalization >>>>>>>>>>>>>>> " << endl;
// print(std_epsl);
                        return std_epsl;
                    }

                    result_node.elements[1].second = true_;
                } else {
                // ZS-DD Rule 1b-r: {(¬a, 0), (a, 𝜀)} -> a, t>v^l
// cout << "ZS-DD Rule 1b-r >>>>>>>>>>>>>>> " << endl;
                    int origin_vtree_index = result_node.vtree_index;
                    result_node.vtree_index = vtree->left_child(result_node.vtree_index);  // v' <- v^l

                    Tsdd t_alpha = result_node.elements[1].first;
                    if (t_alpha == epsl_.at(vtree->right_child(result_node.vtree_index))) {
                        // standalization for epsl(aka. non-terminal cases)
                        Tsdd std_epsl(origin_vtree_index, \
                            true_nodes_.at(vtree->left_child(result_node.vtree_index)));  // -v'- 1 ~ v^ll
// cout << "ZS-DD Rule 1b-r standalization >>>>>>>>>>>>>>> " << endl;
// print(std_epsl);
                        return std_epsl;
                    }

                    result_node.elements[1].second = t_alpha;
                    result_node.elements[1].first = true_;
                    result_node.elements.erase(result_node.elements.begin());
                }
            } else if (result_node.elements[1].first == epsl_.at(vtree->left_child(result_node.vtree_index))) {
                int tmp_sub_tag = result_node.elements[1].second.tag_;
                if (tmp_sub_tag == vtree->right_child(result_node.vtree_index)) {
// cout<<"reduced 2.1 --------------------------------------------" << endl;
                // ZS-DD Rule 2a: {(¬𝜀, 0), (𝜀, a)} -> a, t=v^r
                    result_tsdd = result_node.elements[1].second;
                    result_tsdd.tag_ = result_node.vtree_index;
                    return result_tsdd;
                } else if (tmp_sub_tag < vtree->right_child(result_node.vtree_index)) {
                // ZS-DD Rule 2b-l: {(¬𝜀, 0), (𝜀, a)} -> a, t<v^r
// cout << "ZS-DD Rule 2b-l >>>>>>>>>>>>>>> " << endl;
                    int origin_vtree_index = result_node.vtree_index;
                    result_node.vtree_index = vtree->right_child(result_node.vtree_index);

                    Tsdd t_alpha = result_node.elements[1].second;
                    if (t_alpha == epsl_.at(vtree->left_child(result_node.vtree_index))) {
                        // standalization for epsl(aka. non-terminal cases)
                        Tsdd std_epsl(origin_vtree_index, \
                            true_nodes_.at(vtree->right_child(result_node.vtree_index)));  // -v'- 1 ~ v^rr
// cout << "ZS-DD Rule 2b-l standalization >>>>>>>>>>>>>>> " << endl;
// print(std_epsl);
                        return std_epsl;
                    }

                    result_node.elements[1].first = result_node.elements[1].second;
                    result_node.elements[1].second = true_;
// cout<<"xor1 in reduce -------------------------------------------" << endl;
                    result_node.elements[0].first = negation(result_node.elements[1].first);
                } else {
                // ZS-DD Rule 2b-r: {(¬𝜀, 0), (𝜀, a)} -> a, t>v^r
// cout << "ZS-DD Rule 2b-r >>>>>>>>>>>>>>> " << endl;
                    int origin_vtree_index = result_node.vtree_index;
                    result_node.vtree_index = vtree->right_child(result_node.vtree_index);

                    Tsdd t_alpha = result_node.elements[1].second;
                    if (t_alpha == epsl_.at(vtree->right_child(result_node.vtree_index))) {
                        // standalization for epsl(aka. non-terminal cases)
                        Tsdd std_epsl(origin_vtree_index, \
                            true_nodes_.at(vtree->left_child(result_node.vtree_index)));  // -v'- 1 ~ v^rl
// cout << "ZS-DD Rule 2b-r standalization >>>>>>>>>>>>>>> " << endl;
// print(std_epsl);
                        return std_epsl;
                    }

                    result_node.elements[1].first = true_;
                    result_node.elements.erase(result_node.elements.begin());
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
        e1.first = true_;
        e1.second = true_;
        new_node.elements.push_back(e1);
        return new_node;
    }
    TsddNode tsdd_node = get_tsddnode_at(tsdd.addr_), new_node;
    if (lca == tsdd_node.vtree_index) {
// cout << "would come to here???" << lca << " " << tsdd_node.vtree_index << endl;
// print(tsdd_node);
        return tsdd_node;  // ??? right?
    }
    int tsdd_tag = tsdd.tag_;
    new_node.vtree_index = lca;
// cout<< lca << " -index- " << tsdd_tag << endl;
// cout<< depths_by_index[lca] << " -depth- " << depths_by_index[tsdd_tag] << endl;
// set "vtree_index" in apply algorithms, but no here
// print(tsdd);

    if (depths_by_index[lca] < depths_by_index[tsdd_tag]) {
// cout<<"cofactors 1.1 --------------------------------------------" << endl;
        if (tsdd_tag < lca) {
        // SDD Rule 1
            Element e1;
            e1.first = tsdd;
            // int true_tag = vtree->right_child(lca);
            e1.second = true_;
            new_node.elements.push_back(e1);
            if (!is_true(e1.first)) {
                Element e2;
// cout<<"xor1 in cofac -------------------------------------------" << endl;
                e2.first = negation(e1.first);
                e2.second = false_;
                new_node.elements.push_back(e2);
                return new_node;
            }
cout << "[test true] - 1" << endl;
print(e1.first);
        } else if (tsdd_tag > lca) {
        // SDD Rule 2
            Element e;
            e.first = true_;
            e.second = tsdd;
            new_node.elements.push_back(e);
        } else {
            cout << "cofactors: shouldn't be here." << endl;
        }
    } else if (tsdd_node.vtree_index < lca) {
// cout<<"cofactors 2 --------------------------------------------" << endl;
        if (tsdd_node.is_one()) {
        // special case: true node
// cout<<"cofactors 1.2 ----------- lca: " << lca << endl;
// print(tsdd_node);
            if (tsdd_node.vtree_index==lca) {
cout<<"fangbq 1" << endl;
            }
            Element e1;
            e1.first = tsdd;
            e1.first.tag_ = vtree->left_child(lca);
            // standalization e1.first
            if (e1.first.tag_ == tsdd_node.vtree_index) {
                // '-29- 1 ~ 29' -> '-0- 1 ~ 0'
                e1.first = true_;
            } else if (vtree->left_child(e1.first.tag_)==tsdd_node.vtree_index ||
                vtree->right_child(e1.first.tag_)==tsdd_node.vtree_index) {
                // '-30- 1 ~ 29' -> '-31- -x16 ~ 31'
                e1.first = epsl_.at(sibling_of[tsdd_node.vtree_index]);
            }

            e1.second = epsl_.at(vtree->right_child(lca));
            if (is_true(e1.first)) {
                e1.first = true_;
                new_node.elements.push_back(e1);
                // cout << "[cofactors] possible 1!" << endl;
                return new_node;
            }
            new_node.elements.push_back(e1);
            Element e2;
    // cout<<"xor1 in cofac 2-------------------------------------------" << endl;
            e2.first = negation(e1.first);
            e2.second = false_;
            new_node.elements.push_back(e2);
            return new_node;
        } 
        if (tsdd_node.vtree_index == vtree->left_child(lca)) {
            if (tsdd_node.elements.size() == 1) {
            // ZS-DD Rule 1b-1: 1 element case
                Element e1;
                e1.first = tsdd_node.elements[0].second;
                e1.second = epsl_.at(vtree->right_child(lca));
                if (is_true(e1.first)) {
                    e1.first = true_;
                    new_node.elements.push_back(e1);
                    return new_node;
                }
                new_node.elements.push_back(e1);
                Element e2;
                e2.first = negation(e1.first);
                e2.second = false_;
                new_node.elements.push_back(e2);
                return new_node;
            } else if (tsdd_node.elements.size() == 2) {
// cout<<"cofactors 2.3 --------------------------------------------" << endl;
            // ZS-DD Rule 1b-2: 2 element case
                if (is_true(tsdd_node.elements[0].second) \
                && is_false(tsdd_node.elements[1].second)) {
                    new_node.elements = tsdd_node.elements;
                    new_node.elements[0].second = epsl_.at(vtree->right_child(lca));
                    return new_node;
                } else if (is_true(tsdd_node.elements[1].second) \
                && is_false(tsdd_node.elements[0].second)) {
                    new_node.elements = tsdd_node.elements;
                    new_node.elements[1].second = epsl_.at(vtree->right_child(lca));
                    return new_node;
                }
            }
        }
// cout<<"cofactors 2.31 --------------------------------------------" << endl;
        // ZS-DD Rule 1a
        Element e1;
        e1.first = tsdd;
        e1.first.tag_ = vtree->left_child(lca);
        e1.second = epsl_.at(vtree->right_child(lca));
        new_node.elements.push_back(e1);
        if (is_true(e1.first)) {
cout << "[test true] - 3.5" << endl;
print(e1.first);
            return new_node;
        }
        Element e2;
// cout<<"xor1 in cofac 2-------------------------------------------" << endl;
        e2.first = negation(e1.first);
        e2.second = false_;
        new_node.elements.push_back(e2);
// print(new_node);
    } else if (tsdd_node.vtree_index > lca) {
// cout<<"cofactors 5 --------------------------------------------" << endl;
        if (tsdd_node.is_one()) {
        // special case: true node
// cout<<"cofactors 1.22 ----------- lca: " << lca << endl;
// print(tsdd_node);
            if (tsdd_node.vtree_index==lca) {
cout<<"fangbq 2" << endl;
            }
            Element e1;
            e1.first = epsl_.at(vtree->left_child(lca));
            e1.second = tsdd;
            e1.second.tag_ = vtree->right_child(lca);
            // standalization e1.second
            if (e1.second.tag_ == tsdd_node.vtree_index) {
                // '-29- 1 ~ 29' -> '-0- 1 ~ 0'
                e1.second = true_;
            } else if (vtree->left_child(e1.second.tag_)==tsdd_node.vtree_index || \
                vtree->right_child(e1.second.tag_)==tsdd_node.vtree_index) {
                e1.second = epsl_.at(sibling_of[tsdd_node.vtree_index]);
            }
            new_node.elements.push_back(e1);
            Element e2;
            e2.first = epsl_comp_.at(vtree->left_child(lca));
            e2.second = false_;
            new_node.elements.push_back(e2);
            return new_node;
        } 
        if (tsdd_node.vtree_index == vtree->right_child(lca)) {
            if (tsdd_node.elements.size() == 1) {
            // ZS-DD Rule 2b-1: 1 element case
                Element e1, e2;
                e1.first = epsl_.at(vtree->left_child(lca));
                e1.second = tsdd_node.elements[0].second;
                new_node.elements.push_back(e1);

                e2.first = epsl_comp_.at(vtree->left_child(lca));
                e2.second = false_;
                new_node.elements.push_back(e2);
                return new_node;
            } else if (tsdd_node.elements.size() == 2) {
            // ZS-DD Rule 2b-2: 2 element case
                if (is_true(tsdd_node.elements[0].second) \
                && is_false(tsdd_node.elements[1].second)) {
                    new_node.elements = tsdd_node.elements;
                    new_node.elements[0].second = new_node.elements[0].first;
                    new_node.elements[0].first = epsl_.at(vtree->left_child(lca));
                    new_node.elements[1].first = epsl_comp_.at(vtree->left_child(lca));
                    return new_node;
                } else if (is_true(tsdd_node.elements[1].second) \
                && is_false(tsdd_node.elements[0].second)) {
                    new_node.elements = tsdd_node.elements;
                    new_node.elements[1].second = new_node.elements[1].first;
                    new_node.elements[1].first = epsl_.at(vtree->left_child(lca));
                    new_node.elements[0].first = epsl_comp_.at(vtree->left_child(lca));
                    return new_node;
                }
            }
        }
// cout<<"cofactors 6 --------------------------------------------" << endl;
        // ZS-DD Rule 2a
        Element e1, e2;
        e1.first = epsl_.at(vtree->left_child(lca));
        e1.second = tsdd;
        e1.second.tag_ = vtree->right_child(lca);
        new_node.elements.push_back(e1);
        e2.first = epsl_comp_.at(vtree->left_child(lca));
        e2.second = false_;
        new_node.elements.push_back(e2);
    } else {
        std::cerr << "[MyError] cofactors error" << endl;
    }
    return new_node;
}

Tsdd Manager::negation(const Tsdd& tsdd, int indent) {
    // true xor tsdd
    // tsdd maybe a true or a false tsdd because calling below in this function
    if (is_true(tsdd)) return false_;
    if (is_false(tsdd)) return true_;

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "negation ========================================== " << endl;
// print(tsdd, indent);

    Tsdd cache_ = cache_table_.read_cache(XOR, true_, tsdd);
    if (cache_ != empty_)
        return cache_;

// cout<<"negationing 1 --------------------------------------------" << endl;
    TsddNode tsdd_node = get_tsddnode_at(tsdd.addr_);
    
    if (tsdd.tag_ == tsdd_node.vtree_index) {
// cout<<"negationing 2 ---------------------" << tsdd.tag_ << " " << tsdd_node.vtree_index << endl;
        if (tsdd_node.value%2==0) return tsddVar(-tsdd_node.value/2);
        else if (tsdd_node.value%2==1) return tsddVar(tsdd_node.value/2);
        // else cout << "[negation] shouldn't get here!" << endl;
    }


    // if (tsdd_node.is_terminal())
// cout << "1. tsdd.tag_: " << tsdd.tag_ << endl;
    tsdd_node = cofactors(tsdd, tsdd.tag_);
    TsddNode new_node;
    new_node.vtree_index = tsdd_node.vtree_index;
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"normalized_tsdd --------------------------------------------" << endl;
// print(tsdd_node, indent);
    for (std::vector<Element>::const_iterator e = tsdd_node.elements.begin();
    e != tsdd_node.elements.end(); ++e) {
        Element new_e;
        new_e.first = e->first;
        if (!is_false(new_e.first)) {
            new_e.second = negation(e->second, indent+4);
            new_node.elements.push_back(new_e);
        }
    }

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "before reduce got node ---------------" << endl;
// print(new_node, indent);
// cout << "tsdd.tag_: " << tsdd.tag_ << endl;

    Tsdd new_tsdd = reduced(new_node, tsdd.tag_);
    new_node = get_tsddnode_at(new_tsdd.addr_);  // update new_node

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "after reduce got node ---------------" << endl;
// print(new_tsdd, indent);

    // extra case, <(\alpha, false), (\beta, false)> = false.
    if (new_tsdd.addr_==0) return false_;

    // new_tsdd.tag_ = tsdd.tag_;  // can't update tag here!

    // standalization:
    if (new_node.is_one()) {
// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "[negation] standalization: ------------" << endl;
// print(new_tsdd, indent);
        if (new_tsdd.tag_ == new_node.vtree_index) {
            // '-29- 1 ~ 29' -> '-0- 1 ~ 0'
            return true_;
        } else if (vtree->left_child(new_tsdd.tag_)==new_node.vtree_index || \
            vtree->right_child(new_tsdd.tag_)==new_node.vtree_index) {
            return epsl_.at(sibling_of[new_node.vtree_index]);
        }
    } else if (new_node.is_negative()) {
        return epsl_.at(new_tsdd.tag_);
    }

    cache_table_.write_cache(XOR, true_, tsdd, new_tsdd);
    return new_tsdd;
}

Tsdd Manager::apply(const Tsdd& lhs_tsdd, const Tsdd& rhs_tsdd, OPERATOR_TYPE op, int indent) {
// cout << "apply..." << endl;
    if (rhs_tsdd < lhs_tsdd) {
        return apply(rhs_tsdd, lhs_tsdd, op, indent);
    }

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
        default:
            std::cerr << "[MyError] apply error 1" << endl;
            return false_;
    }

    Tsdd cache_ = cache_table_.read_cache(op, lhs_tsdd, rhs_tsdd);
    if (cache_ != empty_)
        return cache_;

// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"apply ========================================== " << op << endl;
// print(lhs_tsdd, indent);
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"apply with --------------------------------------------" << endl;
// print(rhs_tsdd, indent);

    int lhs_tag = lhs_tsdd.tag_;
    int rhs_tag = rhs_tsdd.tag_;
    addr_t lhs = lhs_tsdd.addr_;
    addr_t rhs = rhs_tsdd.addr_;

    TsddNode new_node;
    TsddNode normalized_node1 = get_tsddnode_at(lhs), normalized_node2 = get_tsddnode_at(rhs);
    
    int min_tag = vtree->get_lca(lhs_tag, rhs_tag), lca;
    if (lhs_tag == rhs_tag)
        lca = vtree->get_lca(normalized_node1.vtree_index, normalized_node2.vtree_index);
    else
        lca = min_tag;
// cout<<"lca: " << lca << endl;
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"haha 1" << endl;

    // only if lca is not leaf, then we normalize them
    if (!vtree->is_leaf(lca)) {
    // !!!can we compute '-n- xi ~ m' disjunction with '-n- -xi ~ m' even if `m` is not a leaf
        if (normalized_node1.vtree_index != lca || normalized_node2.vtree_index != lca \
        || !normalized_node1.is_terminal() || !normalized_node2.is_terminal()) {
            // when not computable, both normalization, can we normalize only one ???
// cout<<"normalizing -------------------------------" << endl;
            if (normalized_node1.vtree_index != lca)
                normalized_node1 = cofactors(lhs_tsdd, lca);
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"normalized_tsdd 1 --------------------------------------------" << endl;
// print(normalized_node1, indent);
            if (normalized_node2.vtree_index != lca)
                normalized_node2 = cofactors(rhs_tsdd, lca);
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"normalized_tsdd 2 --------------------------------------------" << endl;
// print(normalized_node2, indent);
        }
    }
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"haha 2" << endl;

    new_node.vtree_index = lca;

    if (normalized_node1.is_one()) {
        switch (op) {
            case AND:
                new_node = normalized_node2;  // could be optimized before!!!
                break;
            case OR:
                new_node = normalized_node1;  // could be optimized before!!!
                break;
            default:
                cerr << "[MyError] apply error 1.1" << endl;
        }
    } else if (normalized_node2.is_one()) {
        switch (op) {
            case AND:
                new_node = normalized_node1;  // could be optimized before!!!
                break;
            case OR:
                new_node = normalized_node2;  // could be optimized before!!!
                break;
            default:
                cerr << "[MyError] apply error 1.2" << endl;
        }
    } else if (normalized_node1.is_terminal() && normalized_node2.is_terminal()) {
// cout<<"normalized terminals --------------------------------------------" << endl;
    // after normalization, they still terminal, means same tag and vtree index
    // there terminals are syntax terminal, not semantics terminals, but ok!!!
    // how to deal with tags for terminal, terminal should be processed otherwhere???
        switch (op) {
// for (int i = 0; i < indent; ++i) cout << " ";
// cout<<"haha 3" << endl;
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
            default:
                cerr << "[MyError] apply error 2" << endl;
        }
    } else {
        for (std::vector<Element>::const_iterator e1 = normalized_node1.elements.begin();
        e1 != normalized_node1.elements.end(); ++e1) {
            for (std::vector<Element>::const_iterator e2 = normalized_node2.elements.begin();
            e2 != normalized_node2.elements.end(); ++e2) {
                Element new_e;
                new_e.first = apply(e1->first, e2->first, AND, indent+4);
                if (!is_false(new_e.first)) {
// cout << "[apply] prime 1 AND with ---------------" << endl;
// print(e1->first);
// cout << "[apply] prime 2 ---------------" << endl;
// print(e2->first);
// cout << "[apply] new prime ---------------" << endl;
// print(new_e.first);

// cout << "[apply] sub 1 op with ---------------" << endl;
// print(e1->second);
// cout << "[apply] sub 2 ---------------" << endl;
// print(e2->second);
                    new_e.second = apply(e1->second, e2->second, op, indent+4);
// cout << "[apply] new sub ---------------" << endl;
// print(new_e.second);
                    new_node.elements.push_back(new_e);
                }
            }
        }
    }

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "before reduce got node ---------------" << endl;
// print(new_node, indent);

    // for false node, because <(t, false)> = false.
    if (new_node.is_zero()) return false_;

    Tsdd new_tsdd = reduced(new_node, min_tag);

    new_node = get_tsddnode_at(new_tsdd.addr_);  // update new_node

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "after reduce got node ---------------" << endl;
// print(new_tsdd, indent);

    // extra case, <(\alpha, false), (\beta, false)> = false.
    if (new_tsdd.addr_==0) return false_;

// cout << min_tag << " " << lca << endl;

    if (min_tag != lca) {
        new_tsdd.tag_ = min_tag;
    }

    // standalization:
    if (new_node.is_one()) {
// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "standalization: ------------" << endl;
// print(new_tsdd, indent);
        if (new_tsdd.tag_ == new_node.vtree_index) {
            // '-29- 1 ~ 29' -> '-0- 1 ~ 0'
            return true_;
        } else if (vtree->left_child(new_tsdd.tag_)==new_node.vtree_index ||
            vtree->right_child(new_tsdd.tag_)==new_node.vtree_index) {
            return epsl_.at(sibling_of[new_node.vtree_index]);
        }
    } else if (new_node.is_negative()) {
        return epsl_.at(new_tsdd.tag_);
    }

// for (int i = 0; i < indent; ++i) cout << " ";
// cout << "new tsdd with new tag ------------" << endl;
// print(new_tsdd, indent);

    cache_table_.write_cache(op, lhs_tsdd, rhs_tsdd, new_tsdd);
    return new_tsdd;
}

void Manager::print(const Tsdd& tsdd, int indent) const {
    TsddNode tsdd_node = get_tsddnode_at(tsdd.addr_);
    int tsdd_tag = tsdd.tag_;
    if (tsdd_node.elements.empty()) {
        for (int i = 0; i < indent; ++i) cout << " ";
        if (tsdd_node.value < 2) {
            // cout << tsdd_node.value << endl;
            cout << "-" << tsdd_tag << "- " << tsdd_node.value << " ~ " << tsdd_node.vtree_index << endl;
        } else {
            cout << "-" << tsdd_tag << "- ";
            if (tsdd_node.is_negative()) cout << "-";
            // cout << "x" << tsdd_node.value/2 << endl;
            cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) cout << " ";
    cout << "-" << tsdd_tag << "- Dec " << tsdd_node.vtree_index << ":" << endl;
    int counter = 1;
    indent++;
    for (const auto e : tsdd_node.elements) {
        for (int i = 0; i < indent; ++i) cout << " ";
        cout << "E" << counter << "p:" << endl;
        print(e.first, indent+1);
        for (int i = 0; i < indent; ++i) cout << " ";
        cout << "E" << counter++ << "s:" << endl;
        print(e.second, indent+1);
    }
    return;
}

void Manager::print(const TsddNode& tsdd_node, int indent) const {
    if (tsdd_node.elements.empty()) {
        for (int i = 0; i < indent; ++i) cout << " ";
        if (tsdd_node.value < 2) {
            // cout << tsdd_node.value << endl;
            cout << "-no-tag- " << tsdd_node.value << " ~ " << tsdd_node.vtree_index << endl;
        } else {
            cout << "-no-tag- ";
            if (tsdd_node.is_negative()) cout << "-";
            // cout << "x" << tsdd_node.value/2 << endl;
            cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) cout << " ";
    cout << "-no-tag- Dec " << tsdd_node.vtree_index << ":" << endl;
    int counter = 1;
    indent++;
    for (const auto& e : tsdd_node.elements) {
        for (int i = 0; i < indent; ++i) cout << " ";
        cout << "E" << counter << "p:" << endl;
        print(e.first, indent+1);
        for (int i = 0; i < indent; ++i) cout << " ";
        cout << "E" << counter++ << "s:" << endl;
        print(e.second, indent+1);
    }
    return;
}

void Manager::print_tsdd_nodes() const {
    cout << "tsdd_nodes_:-------------------------------" << endl;
    int i = 0;
    for (auto& tsdd_node: uniq_table_.tsdd_nodes_) {
        cout << "Node " << i++ << ":" << endl;
        print(tsdd_node);
        cout << endl;
    }
}

void Manager::print_unique_table() const {
    cout << "unique_table:-------------------------------" << endl;
    for (auto& x: uniq_table_.uniq_table_) {
        cout << "Node addr_t: " << x.second << endl;
        print(x.first);
        cout << endl;
    }
}

Tsdd Manager::cnf_to_tsdd(const std::string cnf_file, const std::string vtree_file) {
    // read *.cnf
    std::ifstream infile(cnf_file, std::ios::in);
    std::string file_name = static_cast<std::string>(cnf_file);
    cout << file_name.substr(file_name.find_last_of("/")+1, file_name.length()-file_name.find_last_of("/")) << "\t";
    if(!infile)
    {
        std::cerr << "open infile error!" << endl; 
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
    cout << var_no << "  " << col_no << "    ";
    std::ifstream vtreefile(vtree_file, std::ios::in);
    if (!vtreefile || vtree_file.empty()) {
        std::vector<int> vars_order;
        for (int i = 1; i <= var_no; ++i) vars_order.push_back(i);
        // vtree = new Vtree(1, var_no*2-1, vars_order, RANDOM_TREE);
        vtree = new Vtree(1, var_no*2-1, vars_order, TRIVIAL_TREE);
    } else {
        vtree = new Vtree(vtree_file);
    }

    VTREE_SIZE = vtree->size;
    initial_constants();
    initial_node_table_and_zsdd_trues();

    string output_vtree_file = vtree_file;
    // std::cout << output_vtree_file << std::endl;
    output_vtree_file.replace(0, 23, "../benchmarks/all-vtree-linear");
    // std::cout << output_vtree_file << std::endl;
    vtree->save_vtree_file(output_vtree_file);

    // make tsdd literal by literal
    Tsdd fml = true_;
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
            clause = apply(tmp_var, clause, OR);
        }
// cout << "clause : " << clause_counter << " starting... " << size(fml) << endl;

// cout<<"apply AND ============================" << endl;
// print(fml);
// cout<<"fml apply with clause ----------------" << endl;
// print(clause);
        // fml = apply(fml, clause, AND);
// cout<<"get new fml --------------------------" << endl;
// print(fml);

        clause_tsdds.push_back(clause);
// cout << "clause : " << clause_counter++ << " done; " << size(clause) << endl;
// print(clause);
    }


// permutation checking
// int first_size = -1;
// Tsdd first_tsdd;
// int i = 0;
// do {
//     Tsdd tmp_fml = true_;
//     for (auto a : clause_tsdds) {
//         tmp_fml = apply(tmp_fml, a, AND);
//     }
//     int tmp_size = 0;
// cout << "permutation : \t" << i++ << ";\tsize: " << (tmp_size=size(tmp_fml)) \
//     << "\t node size: " << uniq_table_.node_size() << "\t table size: " << uniq_table_.table_size() << endl;
//     if (first_size<0) {
//         first_size = tmp_size;
//         first_tsdd = tmp_fml;
// cout << "[permutation] First tsdd:" << endl;
// print(first_tsdd);
//     }
//     else if (first_size != tmp_size) {
// cout << "[permutation] Two size error!" << endl;
// print(tmp_fml);
//         return fml;
//     } else if (first_tsdd != tmp_fml) {
// cout << "[permutation] Two tsdd error!" << endl;
// print(tmp_fml);
//         return fml;
//     }
// } while (std::next_permutation(clause_tsdds.begin(), clause_tsdds.end()));


// shuffle checking
    // obtain a time-based seed:
// unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
// int first_size = -1;
// Tsdd first_tsdd;
// int i = 0;
// while (1) {
//     shuffle (clause_tsdds.begin(), clause_tsdds.end(), std::default_random_engine(seed));
//     Tsdd tmp_fml = true_;
//     for (auto a : clause_tsdds) {
//         tmp_fml = apply(tmp_fml, a, AND);
//     }
//     int tmp_size = 0;
// cout << "shuffle : \t" << i++ << ";\tsize: " << (tmp_size=size(tmp_fml)) \
//     << "\t node size: " << uniq_table_.node_size() << "\t table size: " << uniq_table_.table_size() << endl;
//     if (first_size<0) {
//         first_size = tmp_size;
//         first_tsdd = tmp_fml;
// cout << "[shuffle] First tsdd size: " << first_size << endl;
// print(first_tsdd);
//     }
//     else if (first_size != tmp_size) {
// cout << "[shuffle] Two size error: " << tmp_size << endl;
// print(tmp_fml);
//         return fml;
//     } else if (first_tsdd != tmp_fml) {
// cout << "[shuffle] Two tsdd error!" << endl;
// print(tmp_fml);
//         return fml;
//     }
// }


    // return fml;


    // shuffle (clause_tsdds.begin(), clause_tsdds.end(), std::default_random_engine(seed));


    // divide and conquer computing process
    while (clause_tsdds.size() > 1) {
        std::vector<Tsdd> tmp_tsdds;
        for (int i = 0; i < (int)(clause_tsdds.size() + 1) / 2; i++) {
            if (2*i+1 >= (int)clause_tsdds.size()) {
                tmp_tsdds.push_back(clause_tsdds[2*i]);
            }
            else {
                tmp_tsdds.push_back(apply(clause_tsdds[2*i], clause_tsdds[2*i+1], AND));
            }
        }
// cout << clause_tsdds.size() << endl;
        clause_tsdds = tmp_tsdds;
    }
    // print(clause_tsdds[0]);
    return clause_tsdds[0];
}

void Manager::export_tsdd_dot(const addr_t tsdd_addr, std::ostream& os) const {
    const std::string SYMBOL_TRUE = "&#8868;";
    const std::string SYMBOL_FALSE = "&#8869;";
    auto  lit2symb =  [](const int literal) -> std::string {
        std::ostringstream oss;
        if (literal < 0) {
            oss << "&#177;";
        }
        if (abs(literal) <= 'Z' - 'A' + 1) {
            oss << static_cast<char>(abs(literal) + 'A' - 1);
        } else {
            oss << abs(literal);
        }
        return oss.str();
    };

    const TsddNode& node = get_tsddnode_at(tsdd_addr);
    if (node.is_terminal()) {
        std::string symbol;
        int lit_value = node.value;
        if (lit_value == 1) {
            symbol = SYMBOL_TRUE;
        } else if (lit_value == 0) {
            symbol = SYMBOL_FALSE;
        } else {
            int var = lit_value%2 ? -lit_value/2 : lit_value/2;
            std::ostringstream oss;
            symbol = lit2symb(var);
        }
        os << "digraph tsdd {\noverlap=false\n"
           << "n1 [label= \"" <<  symbol << "\",\n"
           << "shape=record,\n" 
           << "fontsize=20,\n"
           << "fontname=\"Times-Italic\",\n"
           << "fillcolor=white,\n"
           << "style=filled,\n"
           << "fixedsize=true,\n"
           << "height=.30,\n"
           << "width=.45];\n"
           << "}\n";
        
        return;
    }

    std::unordered_map<int, std::vector<addr_t> > same_level_nodes;
    {
        std::unordered_set<addr_t> visited;
        std::stack<addr_t> stk;
        stk.push(tsdd_addr);
        visited.insert(tsdd_addr);
        while (!stk.empty()) {
            auto addr = stk.top();
            stk.pop();
            if (!node.is_terminal()) {
                if (same_level_nodes.find(node.vtree_index) == same_level_nodes.end()) {
                    same_level_nodes[node.vtree_index] = std::vector<addr_t>();
                }
                same_level_nodes[node.vtree_index].push_back(addr);
                
                for (const auto e : node.elements) {
                    if (e.first.addr_ >= 0 && visited.find(e.first.addr_) == visited.end())  {
                        stk.push(e.first.addr_);
                        visited.insert(e.first.addr_);
                    }
                    if (e.second.addr_ >= 0 && visited.find(e.second.addr_) == visited.end())  {
                        stk.push(e.second.addr_);
                        visited.insert(e.second.addr_);
                    }
                }
            }
        }
    }

    os << "digraph tsdd {\noverlap=false\n";
    for (auto& pair : same_level_nodes) {
        os << "{rank=same;";
        for (auto& id : pair.second) {
            os << " n" << id;
        }
        os << "}\n";
    }

    std::unordered_set<addr_t> visited;
    std::stack<addr_t> stk;
    stk.push(tsdd_addr);
    visited.insert(tsdd_addr);
    while (!stk.empty()) {
        auto addr = stk.top();
        stk.pop();
        if (!node.is_terminal()) {
            os << "n" << addr
               << "[label= \"" << node.vtree_index 
               << "\",style=filled,fillcolor=gray95,shape=circle,height=.25,width=.25]; \n";
            
            for (size_t i = 0; i < node.elements.size(); i++) {
                const auto& elem = node.elements.at(i);
                
                std::string  p_s;
                std::string  s_s;
                
                const auto& prime = get_tsddnode_at(elem.first.addr_);
                if (prime.value == 1) {
                    p_s = SYMBOL_TRUE;
                } else if (prime.value == 0) {
                    p_s =  SYMBOL_FALSE;
                } else {
                    if (prime.is_terminal()) {
                        int var = prime.value%2 ? -prime.value/2 : prime.value/2;
                        p_s = lit2symb(var);
                    } else {
                        if (visited.find(elem.first.addr_) == visited.end()) {
                            visited.insert(elem.first.addr_);
                            stk.push(elem.first.addr_);
                        }
                    }
                }

                const auto& sub = get_tsddnode_at(elem.second.addr_);
                if (sub.value == 1) {
                    s_s = SYMBOL_TRUE;
                } else if (sub.value == 0) {
                    s_s =  SYMBOL_FALSE;
                } else {
                    if (sub.is_terminal()) {
                        int var = sub.value%2 ? -sub.value/2 : sub.value/2;
                        s_s = lit2symb(var);
                    } else {
                        if (visited.find(elem.second.addr_) == visited.end()) {
                            visited.insert(elem.second.addr_);
                            stk.push(elem.second.addr_);
                        }
                    }
                }
                std::ostringstream nid_oss;
                nid_oss << "n" << addr << "e" << i;
                auto nid = nid_oss.str();
                os << nid
                   << " [label= \"<L>" <<  p_s 
                   << "|<R>" << s_s << "\",\n"
                   << "shape=record,\n"
                    "fontsize=20,\n"
                    "fontname=\"Times-Italic\",\n"
                    "fillcolor=white,\n"
                    "style=filled,\n"
                    "fixedsize=true,\n"
                    "height=.30,\n"
                    "width=.65];\n";
                os << "n" << addr << "->" << nid << " [arraysize=.50];";
                if (p_s == "") {
                    os << nid <<  ":L:c->n" 
                       << elem.first.addr_ <<  "[arrowsize=.50,tailclip=false,arrowtail=dot,dir=both];\n";
                }
                if (s_s == "") {
                    os << nid <<  ":R:c->n" 
                       << elem.second.addr_ <<  "[arrowsize=.50,tailclip=false,arrowtail=dot,dir=both];\n";
                }
            }
        }
 
    }
    os << "}\n";   
}

} // namespace tsdd
