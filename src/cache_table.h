#ifndef CACHE_TABLE_H_
#define CACHE_TABLE_H_

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include "tsdd_node.h"

using namespace std;

namespace tsdd {

class CacheTable {
	using cache_entry = std::tuple<OPERATOR_TYPE, Tsdd, Tsdd, Tsdd>;
public:

    CacheTable() : cache_table_(INIT_SIZE) {}
    CacheTable(const int init_size) : cache_table_(init_size) {}    

	void write_cache(const OPERATOR_TYPE op, const Tsdd& lhs,
	                 const Tsdd& rhs, const Tsdd& res) {
	    auto key = calc_key(op, lhs, rhs);
	    cache_table_[key] = std::make_tuple(op, lhs, rhs, res);
	}

    void clear_cache() {
        // for (auto it =  cache_table_.begin(); it != cache_table_.end(); ++it) {
        //     *it = std::make_tuple(Operation::NULLOP, -1, -1, -1);
        // }
    }

	Tsdd read_cache(const OPERATOR_TYPE op, const Tsdd& lhs, const Tsdd& rhs) {
	    auto key = calc_key(op, lhs, rhs);
	    auto res = cache_table_[key];
	    if (std::get<0>(res) == op &&
	        std::get<1>(res) == lhs &&
	        std::get<2>(res) == rhs) {
	        return (std::get<3>(res));
	    }
	    return (*new Tsdd(-1, -1));  // corresponds to empty_
	}

	size_t calc_key(const OPERATOR_TYPE op, const Tsdd& lhs,  const Tsdd& rhs) {
	    size_t key = 0;
	    hash_combine(key, std::hash<int>()(static_cast<int>(op)));
	    hash_combine(key, std::hash<tsdd::Tsdd>()(lhs));
	    hash_combine(key, std::hash<tsdd::Tsdd>()(rhs));
	    // std::cout << "cache table size: " << cache_table_.size() << std::endl;
	    return key % cache_table_.size();
	}

	void print() const {
	    cout << "cache_table:-------------------------------" << endl;
	    for (auto& x: cache_table_) {
	        cout << get<0>(x) << get<1>(x).addr_ << get<2>(x).addr_ << get<3>(x).addr_ << endl;
	    }
	}
public:
    const unsigned int INIT_SIZE = 1U<<10;
    std::vector<cache_entry> cache_table_;
};

} // namespace tsdd;


#endif
