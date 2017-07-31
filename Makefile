all:
	g++ -std=c++11 utils.cpp hcrminer.cpp apriori.cpp hash_tree.cpp -o hcrminer
debug:
	g++ -std=c++11 utils.cpp hcrminer.cpp apriori.cpp hash_tree.cpp -D_DEBUG -g -o hcrminer
