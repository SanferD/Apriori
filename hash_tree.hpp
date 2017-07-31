#ifndef _HASH_TREE
#define _HASH_TREE

#include <vector>

#include "types.hpp"

class HashTree {
	typedef std::vector<const Itemset*> LeafNode;
	typedef std::vector<HashTree> InterNode;

	bool isLeaf;
	int bfactor, depth, num_items, bz_max;
	SupportCountTable *sigma;

	InterNode node;
	LeafNode bucket;
	
	void add_itemset(const Itemset *set);
	bool traverse(const Transaction &t, int index);
	HashTree(const int b, const int d, const int n, const int max, SupportCountTable *t);
	void print_leaves();
public:
	HashTree();
	HashTree(const Itemsets &C, SupportCountTable *table, const int hfrange, const int maxleafsize);
	bool operator()(const Transaction &t);
};

#endif