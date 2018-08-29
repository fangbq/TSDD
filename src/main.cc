/*
 * File: main.cpp
 * Author: Biqing Fang
 * this test file is used to test a case from benchmarks:
 * 1. f=0+0
 * 2. f=0+1     6. f=1+1
 * 3. f=0+x1    7. f=1+x1   10. f=x1+x1
 * 4. f=0+-x1   8. f=1+-x1  11. f=x1+-x1  13. f=-x1+-x1
 * 5. f=0+x4    9. f=1+x4   12. f=x1+x4   14. f=-x1+x4   15. f=x4+x4
 */
#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <time.h>
#include "manager.h"

using namespace std;
using namespace tsdd;

std::map<int, int> get_index_by_var = {{0,0}};  // x1 stored as "1", x4 stored as "4"...
                                           // indexs of true and false both are "0"
int VTREE_SIZE = 0;

int main(int argc, char** argv) {
    if (argc) std::cout << "";
    Manager manager;
    // cout << "Benchmarks\tInputs\tOutputs\tWires\tNode Count\tRuntime (s)" << endl;  // table header
    clock_t start = clock();
    string vtree_file = "";
    bool input_cnf_file = false;
    if (argv[2]) vtree_file = argv[2];
    if (strstr(argv[1], "cnf")) input_cnf_file = true;
    Tsdd fml;
    fml = manager.cnf_to_tsdd(argv[1], vtree_file);  // for CNF file
    clock_t finish = clock();
    double ptime = (double)(finish - start) / CLOCKS_PER_SEC;  // runtime
    cout.setf(ios::showpoint);
    cout.precision(4);
    cout.setf(ios::fixed);
    cout << manager.size(fml) << "\t\t" << ptime << endl;

    return 0;
}
