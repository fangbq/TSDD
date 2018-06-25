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
	    auto res = uniq_table_.find(sorted_node);
	    if (res != uniq_table_.end()) {
	        return res->second;
	    }
	    return make_tsdd(sorted_node);
	}

public:
    std::vector<TsddNode> tsdd_nodes_;
    std::unordered_map<TsddNode, addr_t> uniq_table_;
};

}

#endif // ENDIF