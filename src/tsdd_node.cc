#include "tsdd_node.h"
#include "manager.h"

namespace tsdd {

bool TsddNode::operator==(const TsddNode& tsdd_node) const {
// cout << "equals..." << endl;
    if (is_terminal() && tsdd_node.is_terminal()) {
        return value == tsdd_node.value;
    } else if (elements.size() == tsdd_node.elements.size()) {
        return elements == tsdd_node.elements;
    }
    // cout << "no eq" << endl;
    return false;
}

bool TsddNode::is_terminal() const { return value!=-1; }
bool TsddNode::is_positive() const { return value>1 && value%2==0; }
bool TsddNode::is_negative() const { return value>1 && value%2==1; }
bool TsddNode::is_zero() const { return value==0; }
bool TsddNode::is_one() const { return value==1; }
bool TsddNode::is_constant() const { return is_zero() || is_one(); }


}  // namespace tsdd;
