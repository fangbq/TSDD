#ifndef UNQUE_TABLE_H_
#define UNQUE_TABLE_H_
#include "tsdd_node.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <algorithm>


namespace tsdd {

class UniqueTable {
public:

    UniqueTable() :
        tsdd_nodes_(),
        uniq_table_() {}

    TsddNode& get_node_at(const addr_t i) {
        auto& n = tsdd_nodes_[i];
        return n;
    }
    const TsddNode& get_node_at(const addr_t i) const {
        auto& n = tsdd_nodes_[i];
        return n;
    }

	addr_t make_tsdd(const TsddNode& new_node) {
	    // tsdd_nodes_.emplace_back();
	    size_t node_id = tsdd_nodes_.size();
	    uniq_table_.emplace(new_node, node_id);
	    tsdd_nodes_.push_back(new_node);
	    return node_id;
	}

	addr_t make_or_find(const TsddNode& new_node) {
	    TsddNode sorted_node = new_node;
	    sort(sorted_node.elements.begin(), sorted_node.elements.end());
// if (sorted_node.value == 19 && sorted_node.vtree_index == 17) {
// 	std::cout << "tring find 19 ;;;;;;;;;;;;;;;;;;; " << new_node.elements.size() << std::endl;
// }
	    auto res = uniq_table_.find(sorted_node);
	    if (res != uniq_table_.end()) {
	        return res->second;
	    }
// if (sorted_node.value == 19 && sorted_node.vtree_index == 17) {
// 	std::cout << "not found     ;;;;;;;;;;;;;;;;;;; " << std::endl;
// }
	    return make_tsdd(sorted_node);
	}

	int node_size() const {
		return tsdd_nodes_.size();
	}

	int table_size() const {
		return uniq_table_.size();
	}

	void clean() {
		tsdd_nodes_.clear();
		uniq_table_.clear();
	}
public:
    std::vector<TsddNode> tsdd_nodes_;
    std::unordered_map<TsddNode, addr_t> uniq_table_;
};

}

#endif // ENDIF