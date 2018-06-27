#ifndef TSDD_NODE_H
#define TSDD_NODE_H

#include <vector>
#include <iostream>
#include "define.h"
#include "tsdd_vtree.h"
#include <unordered_set>
#include <type_traits>
#include <functional>

namespace tsdd {

class Manager;

using Element = std::pair<addr_t, addr_t>;

class TsddNode {
public:
    int tag_ = 0;
    int value = -1;  /* terminal nodes: 0 respects false, 1 respects true; 2*x respects the xth variable (x);
                        2*x+1 respects the negation of the xth variable (-x);
                        nonterminal: -1
                    */
    std::vector<Element> elements;
    int vtree_index = 0;  // respect to vtree node, constant respects 0
    int hash_value = 0;
public:
    TsddNode() { elements.clear(); }
    TsddNode(const TsddNode& s) { value = s.value; elements = s.elements; vtree_index = s.vtree_index; }
    ~TsddNode() { value = -1; elements.clear(); vtree_index = 0; }
    TsddNode(int v, int i = 0, int t = 0) { elements.clear(); value = v; vtree_index = i; tag_ = t; }
    TsddNode& operator=(const TsddNode& tsdd_node) {
        value = tsdd_node.value;
        elements = tsdd_node.elements;
        vtree_index = tsdd_node.vtree_index;
        tag_ = tsdd_node.tag_;
        return *this;
    }
    bool operator==(const TsddNode& tsdd_node) const;

    bool is_terminal() const;
    bool is_positive() const;
    bool is_negative() const;
    bool is_zero() const;
    bool is_one() const;
    bool is_constant() const;
};


} // namespace tsdd

namespace std {
template <> struct hash<tsdd::TsddNode> {
    std::size_t operator()(const tsdd::TsddNode& n) const {
        size_t h = 0;
        if (n.value >= 0) {
            tsdd::hash_combine(h, hash<int>()(n.value));
            tsdd::hash_combine(h, hash<int>()(n.vtree_index));
            tsdd::hash_combine(h, hash<int>()(n.tag_));
            return h;
        } else if (n.value < 0) {
            for (const auto& e : n.elements) {
                tsdd::hash_combine(h, hash<tsdd::addr_t>()(e.first));
                tsdd::hash_combine(h, hash<tsdd::addr_t>()(e.second));
            }
            tsdd::hash_combine(h, hash<int>()(n.vtree_index));
        }
        return h;
    }
};
}


#endif
