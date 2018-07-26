CXX = g++


CXXFLAGS +=  -O3 -Wall -Wextra -std=c++11 -pg


APPS =  tsdd


SRC=$(shell ls *.cc)

all :  $(APPS)



clean :
	rm -f *.o $(APPS)



dep:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MG $(SRC) >makefile.depend


.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<


tsdd : main.o manager.o tsdd_vtree.o tsdd_node.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $^ -o $@

-include makefile.depend
