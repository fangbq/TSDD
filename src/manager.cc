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
#include "readVerilog.h"


extern std::map<int, int> get_index_by_var;

using namespace std::chrono;

namespace tsdd {

Manager::Manager() {
    TsddNode true_node(1), false_node(0);
    false_ = uniq_table_.make_tsdd(false_node);
    true_ = uniq_table_.make_tsdd(true_node);
    // std::cout << "Manager bad initialization !!!" << std::endl;
}

Manager::Manager(const Vtree& v, const unsigned int cache_size) :
    cache_table_(cache_size) {
    TsddNode true_node(1), false_node(0);
    false_ = uniq_table_.make_tsdd(false_node);
    true_ = uniq_table_.make_tsdd(true_node);
    vtree = new Vtree(v);
    initial_node_table_and_piterms_map();
};

void Manager::initial_node_table_and_piterms_map() {
    for (int i = 1; i <= (vtree->size+1)/2; ++i) {
        TsddNode pos_literal(i*2, get_index_by_var[abs(i)]), neg_literal(i*2+1, get_index_by_var[abs(i)]);
        uniq_table_.make_tsdd(pos_literal);
        uniq_table_.make_tsdd(neg_literal);
    }
    max_addr_for_lit_ = vtree->size+2;
    
    // create a map for vtree_node and bigoplus_piterms
    // generate_bigoplus_piterms(*vtree);

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

Manager::~Manager() {
    if (vtree != NULL) { delete vtree; vtree = NULL; }
};

addr_t Manager::tsddVar(const int tmp_var) {
    // in the case of the initial tsdd_nodes_[] is settel
    return (tmp_var < 0 ? (-tmp_var)*2+1 : tmp_var*2);
}

unsigned long long Manager::size(const addr_t tsdd_id) const {
    if (is_terminal(tsdd_id)) return 0;

    std::unordered_set<addr_t> node_ids;
    std::stack<addr_t> unexpanded;
    unexpanded.push(tsdd_id);

    while (!unexpanded.empty()) {
        addr_t e = unexpanded.top();
        unexpanded.pop();

        node_ids.insert(e);
        const TsddNode& n = uniq_table_.get_node_at(e);
        if (n.value < 0) {
            for (const Element e : n.elements) {
                if (node_ids.find(e.first) == node_ids.end()) {
                    node_ids.insert(e.first);
                    unexpanded.push(e.first);
                }
                if (node_ids.find(e.second) == node_ids.end()) {
                    node_ids.insert(e.second);
                    unexpanded.push(e.second);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto& i : node_ids) {
        const TsddNode& n = uniq_table_.get_node_at(i);
        if (n.value < 0) {
            size += n.elements.size();
        }
    }
    return size;
}

unsigned long long Manager::size(const std::unordered_set<addr_t> tsdd_ids) const {
    std::unordered_set<addr_t> node_ids;
    std::stack<addr_t> unexpanded;
    for (const auto& id : tsdd_ids) {
        if (is_terminal(id)) continue;
        unexpanded.push(id);
    }

    while (!unexpanded.empty()) {
        addr_t e = unexpanded.top();
        unexpanded.pop();

        node_ids.insert(e);
        const TsddNode& n = uniq_table_.get_node_at(e);
        if (n.value < 0) {
            for (const Element e : n.elements) {
                if (node_ids.find(e.first) == node_ids.end()) {
                    node_ids.insert(e.first);
                    unexpanded.push(e.first);
                }
                if (node_ids.find(e.second) == node_ids.end()) {
                    node_ids.insert(e.second);
                    unexpanded.push(e.second);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto& i : node_ids) {
        const TsddNode& n = uniq_table_.get_node_at(i);
        if (n.value < 0) {
            size += n.elements.size();
        }
    }
    return size;
}

addr_t Manager::reduced(const TsddNode& tsdd_node) {
// cout << "reduced..." << endl;
    bool sameSub = true;
    addr_t first_sub = tsdd_node.elements.front().second;
    for (const auto& e : tsdd_node.elements) {
        if (e.second != first_sub)
        {
            sameSub = false;
            break;
        }
    }

    TsddNode result_node;
    // 1.2 return false if all elements' subs are false
    if (sameSub) {
        // Trimming: one element, SDD rule 2, {(1, a)} -> a
        return first_sub;
    }

    // 2 compressing
    result_node = tsdd_node;
    for (auto e1 = result_node.elements.begin(); \
    e1 != result_node.elements.end(); ) {
        bool is_delete = false;
        for (auto e2 = result_node.elements.begin(); \
        e2 != result_node.elements.end(); ++e2) {
            // cout << "big equal..." << endl;
            if (e1 != e2 && e1->second==e2->second) {
                is_delete = true;
                e2->first = apply(e1->first, e2->first, OR);
                e1 = result_node.elements.erase(e1);
                break;
            }
        }
        if (!is_delete)
            ++e1;
    }

    //3 trimming
// cout << "trimming..." << endl;
    if (result_node.elements.size() == 2) {
        if (result_node.elements[0].second == false_) {
            // SDD Rule 1: {(a, 1), (¬a, 0)} -> a
            if (result_node.elements[1].second == true_) {
                return result_node.elements[1].first;
            }
            TsddNode tmp_pri_ = uniq_table_.tsdd_nodes_.at(result_node.elements[1].first);
            TsddNode tmp_sub_ = uniq_table_.tsdd_nodes_.at(result_node.elements[1].second);
            if (tmp_sub_.value == 1 && tmp_sub_.tag_ == vtree->subvtree(result_node.tag_).rt->index) {
                if (tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                    // ZSDD Rule 1a: {(a, 𝜀), (¬a, 0)} -> a, t=v^l
                    int tmp_tag = result_node.vtree_index;
                    result_node =  result_node.elements[1].first;
                    result_node.tag_ = tmp_tag;
                } else {
                    // ZSDD Rule 1b: {(a, 𝜀), (¬a, 0)} -> a, t≠v^l
                    result_node.tag_ = result_node.vtree_index;
                    result_node.vtree_index = vtree->subvtree(result_node.tag_).lt->index;
                    result_node.elements[1].second = true_;
                }
            } else if (tmp_pri_.value == 1 && tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                if (tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                    // ZSDD Rule 2a: {(𝜀, a), (¬𝜀, 0)} -> a, t=v^l
                    int tmp_tag = result_node.vtree_index;
                    result_node =  result_node.elements[1].second;
                    result_node.tag_ = tmp_tag;
                } else {
                    // ZSDD Rule 2b: {(𝜀, a), (¬𝜀, 0)} -> a, t≠v^l
                    result_node.tag_ = result_node.vtree_index;
                    result_node.vtree_index = vtree->subvtree(result_node.tag_).rt->index;
                    result_node.elements[1].first = true_;
                    result_node.elements.erase(result_node.elements.begin());
                }
            }
        }

        // opposite case:
        if (result_node.elements[1].second == false_) {
            // SDD Rule 1: {(a, 1), (¬a, 0)} -> a
            if (result_node.elements[0].second == true_) {
                return result_node.elements[0].first;
            }
            TsddNode tmp_pri_ = uniq_table_.tsdd_nodes_.at(result_node.elements[0].first);
            TsddNode tmp_sub_ = uniq_table_.tsdd_nodes_.at(result_node.elements[0].second);
            if (tmp_sub_.value == 1 && tmp_sub_.tag_ == vtree->subvtree(result_node.tag_).rt->index) {
                if (tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                    // ZSDD Rule 1a: {(a, 𝜀), (¬a, 0)} -> a, t=v^l
                    int tmp_tag = result_node.vtree_index;
                    result_node =  result_node.elements[0].first;
                    result_node.tag_ = tmp_tag;
                } else {
                    // ZSDD Rule 1b: {(a, 𝜀), (¬a, 0)} -> a, t≠v^l
                    result_node.tag_ = result_node.vtree_index;
                    result_node.vtree_index = vtree->subvtree(result_node.tag_).lt->index;
                    result_node.elements[0].second = true_;
                }
            } else if (tmp_pri_.value == 1 && tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                if (tmp_pri_.tag_ == vtree->subvtree(result_node.tag_).lt->index) {
                    // ZSDD Rule 2a: {(𝜀, a), (¬𝜀, 0)} -> a, t=v^l
                    int tmp_tag = result_node.vtree_index;
                    result_node =  result_node.elements[0].second;
                    result_node.tag_ = tmp_tag;
                } else {
                    // ZSDD Rule 2b: {(𝜀, a), (¬𝜀, 0)} -> a, t≠v^l
                    result_node.tag_ = result_node.vtree_index;
                    result_node.vtree_index = vtree->subvtree(result_node.tag_).rt->index;
                    result_node.elements[0].first = true_;
                    result_node.elements.erase(result_node.elements.begin()+1);
                }
            }
        }
    }

    // if (result_node.is_terminal()) result_node.vtree_index = get_index_by_var[result_node.value/2];
    return uniq_table_.make_or_find(result_node);
}

// addr_t Manager::generate_bigoplus_piterms(const Vtree& v) {
//    // check if constant
//    addr_t result;
//    if (v.var) {
//        result = v.var*2 + 1;
//    } else {
//        TsddNode tsdd_node;
//        tsdd_node.vtree_index = v.index;
//        Element e;
//        e.first = generate_bigoplus_piterms(*(v.lt));
//        e.second = generate_bigoplus_piterms(*v.rt);
//        tsdd_node.elements.push_back(e);
//        result = uniq_table_.make_tsdd(tsdd_node);
//     }
//     bigoplus_piterms.emplace(v.index, result);
//     return result;
// }

addr_t Manager::apply(const addr_t lhs, const addr_t rhs, OPERATOR_TYPE op) {
// cout << "Intersection..." << endl;
    if (lhs > rhs) return apply(rhs, lhs, op);
    if (lhs == rhs || lhs == false_) return lhs;

    if (is_terminal(lhs) && is_terminal(rhs))
    {
        if (get_compl_tmn(lhs) == rhs)
        {
            return lhs;
        }
        return (is_negative(lhs) && is_negative(rhs)) ? true_ : false_;
    }

    addr_t cache = cache_table_.read_cache(INTER, lhs, rhs);
    if (cache != TSDD_NULL)
        return cache;

    switch (op) {
        case AND:
            break;
        case OR:
            break;
        case XOR:
            break;
        default:
            std::cerr << "apply error 1" << std::endl;
    }
    TsddNode new_node;


    // std::cout << "before reduce intered node ---------------" << std::endl;
    // print(new_node);
    addr_t new_id = reduced(new_node);
// std::cout << "got ---------------------" << std::endl;
// print(new_id);
    cache_table_.write_cache(op, lhs, rhs, new_id);
    return new_id;
}

void Manager::print(const addr_t addr_, int indent) const {
    TsddNode tsdd_node = uniq_table_.get_node_at(addr_);
    if (tsdd_node.elements.empty()) {
        for (int i = 0; i < indent; ++i) std::cout << " ";
        if (tsdd_node.value < 2) {
            // std::cout << tsdd_node.value << std::endl;
            std::cout << tsdd_node.value << " ~ " << tsdd_node.vtree_index << std::endl;
        } else {
            if (tsdd_node.is_negative()) std::cout << "-";
            // std::cout << "x" << tsdd_node.value/2 << std::endl;
            std::cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << std::endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "Dec " << tsdd_node.vtree_index << ":" << std::endl;
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
            std::cout << tsdd_node.value << " ~ " << tsdd_node.vtree_index << std::endl;
        } else {
            if (tsdd_node.is_negative()) std::cout << "-";
            // std::cout << "x" << tsdd_node.value/2 << std::endl;
            std::cout << "x" << tsdd_node.value/2 << " ~ " << tsdd_node.vtree_index << std::endl;
        }
        return;
    }
    for (int i = 0; i < indent; ++i) std::cout << " ";
    std::cout << "Dec " << tsdd_node.vtree_index << ":" << std::endl;
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

addr_t Manager::cnf_to_tsdd(const std::string cnf_file, const std::string vtree_file) {
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
    initial_node_table_and_piterms_map();

    // v.save_vtree_file("s27_ balanced.vtree");

    // make tsdd literal by literal
    addr_t fml = true_;
    int  clause_counter = 1;
    std::vector<addr_t> clause_tsdds;
    for(int line = 0; line < col_no; ++line)  //read every line number, and save as a clause
    {
        addr_t clause = false_;
        while (true) {
            int var;
            infile >> var;
            if (var == 0) break;
            addr_t tmp_var = tsddVar(var);
            clause = apply(clause, tmp_var, OR);
        }
        fml = apply(fml, clause, AND);
        clause_tsdds.push_back(clause);
        std::cout << "clause : " << clause_counter++ << " done; " << size(fml) << std::endl;
    }
    return fml;
    while (clause_tsdds.size() > 1) {
        std::vector<addr_t> tmp_tsdds;
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

std::unordered_set<addr_t> Manager::verilog_to_tsdds(char* cnf_file, const std::string vtree_file) {
    logicNet *net = readVerilog(cnf_file);
    std::string test_name(basename(cnf_file));
    // std::cout << "readVerilog done;" << std::endl;

    std::ifstream vtreefile(vtree_file, std::ios::in);
    if (!vtreefile || vtree_file.empty()) {
        std::vector<int> vars_order;
        for (unsigned int i = 1; i <= net->Nin; ++i) vars_order.push_back(i);

        // for random vtree
        std::chrono::milliseconds ms = duration_cast< std::chrono::milliseconds >(
            system_clock::now().time_since_epoch()
        );
        srand (unsigned (ms.count() ) );
        random_shuffle(vars_order.begin(), vars_order.end(), myrandom);

        vtree = new Vtree(1, net->Nin*2-1, vars_order, RANDOM_TREE);
    } else {
        vtree = new Vtree(vtree_file);
    }

    initial_node_table_and_piterms_map();
    // std::cout << "readVerilog done;" << std::endl;

    std::unordered_set<addr_t> ids;
    // int  output_counter = 1;
    for(unsigned int i = 0; i < net->Nout; ++i){
        output_one_tsdd(&(net->output[i]));
        addr_t node_id = (addr_t)((net->output[i]).func);
        // std::cout << "output " << output_counter++ << " done, size: " << size(node_id) << std::endl;
        ids.emplace(node_id);
    }
    // print_tsdd_nodes();
    std::cout << test_name << "\t\t" << net->Nin << "\t" << net->Nout << "\t" << net->Nwire << "\t";
    freeLogicNet(net);
    return ids;
}

void Manager::output_one_tsdd(logicVar *var) {
    // build TSDD literal
    if(VARTYPE(var->info) == _INPUT_){
        var->func = tsddVar(var->varIndex);
        return;
    }
    // build TSDD formulae
    unsigned int gateType = GATETYPE(var->info);
    if(gateType == _XOR_ || gateType == _AND_ || gateType == _OR_){
        output_one_tsdd(var->A);  output_one_tsdd(var->B);
        addr_t funcA = (addr_t)(var->A->func);
        addr_t funcB = (addr_t)(var->B->func);
        if(gateType == _XOR_)       { var->func = apply(funcA, funcB, XOR); }
        else if(gateType == _AND_)  { var->func = apply(funcA, funcB, AND); }
        else                { var->func = apply(funcA, funcB, OR); }
    }else{  // INV or BUF
        output_one_tsdd(var->A);
        addr_t funcA = (addr_t)(var->A->func);

        if(gateType == _INV_)       { var->func = apply(funcA, 1, XOR); }  // ??
        else                { var->func = funcA; }
    }
    return;
}

} // namespace tsdd
