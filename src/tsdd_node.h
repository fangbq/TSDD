#ifndef TSDD_NODE_H
#define TSDD_NODE_H

#include <vector>
#include <iostream>
#include "define.h"
#include "tsdd_vtree.h"
#include <unordered_set>
#include <type_traits>
#include <functional>

extern int VTREE_SIZE;

namespace tsdd {

class Tsdd {
public:
    int tag_ = 0;
    addr_t addr_ = -1;
    // Manager& m;
public:
    Tsdd() {}
    Tsdd(const int tag, const addr_t addr) : tag_(tag), addr_(addr) {}
    Tsdd(const Tsdd& tsdd) : tag_(tsdd.tag_), addr_(tsdd.addr_) {}
    Tsdd& operator=(const Tsdd& tsdd) {
        addr_ = tsdd.addr_;
        tag_ = tsdd.tag_;
        return *this;
    }

    inline bool is_true() const { return addr_>0 && addr_<=VTREE_SIZE && (long long int)tag_==addr_; }

    bool operator==(const Tsdd& tsdd) const {
        // for same_sub, different sytax trues done in manager
        if (is_true() && tsdd.is_true()) return true;
        return tag_==tsdd.tag_ && addr_==tsdd.addr_;
    }

    bool operator!=(const Tsdd& tsdd) const {
        return !(*this==tsdd);
    }

    bool operator<(const Tsdd& tsdd) const {
        if (tag_ < tsdd.tag_) return true;
        else if (tag_ == tsdd.tag_) return addr_<tsdd.addr_;
        else return false;
    }

    addr_t addr() const { return addr_; }
};

using Element = std::pair<tsdd::Tsdd, tsdd::Tsdd>;  // tag 1, prime, tag 2, sub

class TsddNode {
public:
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
    TsddNode(int v, int i = 0) { elements.clear(); value = v; vtree_index = i; }
    TsddNode& operator=(const TsddNode& tsdd_node) {
        value = tsdd_node.value;
        elements = tsdd_node.elements;
        vtree_index = tsdd_node.vtree_index;
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

// class Tsdd;

template <> struct hash<tsdd::Tsdd> {
    std::size_t operator()(const tsdd::Tsdd& t) const {
        size_t h = 0;
        if (t.is_true()) {
            // std::cout << "hhhhh" << std::endl;
            tsdd::hash_combine(h, hash<int>()(-1));
            tsdd::hash_combine(h, hash<tsdd::addr_t>()(-1));
            return h;
        }
        tsdd::hash_combine(h, hash<int>()(t.tag_));
        tsdd::hash_combine(h, hash<tsdd::addr_t>()(t.addr_));
        return h;
    }
};

template <> struct hash<tsdd::TsddNode> {
    std::size_t operator()(const tsdd::TsddNode& n) const {
        size_t h = 0;
        if (n.value >= 0) {
            tsdd::hash_combine(h, hash<int>()(n.value));
            tsdd::hash_combine(h, hash<int>()(n.vtree_index));
            return h;
        } else if (n.value < 0) {
            for (const auto& e : n.elements) {
                tsdd::hash_combine(h, hash<tsdd::Tsdd>()(e.first));
                tsdd::hash_combine(h, hash<tsdd::Tsdd>()(e.second));
            }
            tsdd::hash_combine(h, hash<int>()(n.vtree_index));
        }
        return h;
    }
};
}


#endif
