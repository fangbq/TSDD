/*
#ifndef TSDD_H
#define TSDD_H

#include "manager.h"
#include <iostream>
#include <string>
#include <set>


namespace tsdd {

class Tsdd {
public:
    int tag_ = 0;
    addr_t addr_ = -1;
    // Manager& m;
public:
    Tsdd(const int tag, const addr_t addr) : tag_(tag), addr_(addr) {}
    Tsdd(const Tsdd& tsdd) : tag_(tsdd.tag_), addr_(tsdd.addr_) {}
    Tsdd& operator=(const Tsdd& tsdd) {
        addr_ = tsdd.addr_;
        tag_ = tsdd.tag_;
        return *this;
    }

    bool operator==(const Tsdd& tsdd) const {
        return tag_==tsdd.tag_ && addr_==tsdd.addr_;
    }

    bool operator<(const Tsdd& tsdd) const {
        if (tag_ < tsdd.tag_) return true;
        else if (tag_ == tsdd.tag_) return addr_<tsdd.addr_;
        else return false;
    }

    addr_t addr() const { return addr_; }
};

} // namespace tsdd

namespace std {
template <> struct hash<tsdd::Tsdd> {
    std::size_t operator()(const tsdd::Tsdd& t) const {
        size_t h = 0;
        tsdd::hash_combine(h, hash<int>()(t.tag_));
        tsdd::hash_combine(h, hash<tsdd::addr_t>()(t.addr_));
        return h;
    }
};
}

#endif
*/