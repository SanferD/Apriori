#include "hash_tree.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <cstdlib>

std::vector<int> indeces;

/* Empty constructor for declarations purposes */
HashTree::HashTree() {}

/**
 * @constructor (private)
 * used to set values from intermediate node to its children. 
 * The parameter meanings can be infered from their assignments in the below constructor.
 */
HashTree::HashTree(const int b, const int d, const int n, const int max, SupportCountTable *t) {
	isLeaf = true;
	bfactor = b;
	depth = d;
	num_items = n;
	bz_max = max;
	sigma = t;

	bucket.reserve((size_t) bz_max);
	node.reserve((size_t) bfactor);
}


/**
 * @constructor
 * Generate the hashtree with the given itemsets.
 * If leaf node, the itemsets are ordered lexicophically between itemsets as well as within itemsets.
 * If intermediate node, a has function i%hfrange is used to determine which branch to tranverse. 
 * @param C 					The input candidate itemsets. Assumes lexicographical ordering of the candate itemsets as well as items within itemsets.
 * @param table 			The support table for which to generate the support count
 * @param hfrange 		The branching factor
 * @param maxleafsize The maximum bucket size
 */
HashTree::HashTree(const Itemsets &C, SupportCountTable *table, const int hfrange, const int maxleafsize) {
	isLeaf = true;
	bfactor = hfrange;
	depth = 0;
	num_items = C.back().size();
	sigma = table;
	bz_max = maxleafsize;

	bucket.reserve((size_t) bz_max);
	node.reserve((size_t) bfactor);
	for (const Itemset &s : C) {
		add_itemset(&s);
	}
	// print_leaves();
	indeces.clear();
	indeces.reserve(depth);
}

/**
 * Adds the provided itemset to the hash tree
 * @param set         The input itemset to add. Assumes lexicographical ordering.
 */
void HashTree::add_itemset(const Itemset *set) {
	if (isLeaf) {
		bucket.push_back(set);

		// if bucket is larger than max then decompose into multiple branches
		if ((int) bucket.size() > bz_max) {
			if (depth == num_items) // can't decompose with this bfactor so do nothing
				return;
			
			isLeaf = false; // now an intermediate node
			// initialize the children
			for (int i=0; i!=bfactor; i++)
				node.push_back( HashTree(bfactor, depth+1, num_items, bz_max, sigma) );

			// add the itemsets in the bucket to the corresponding children
			for (const Itemset *s : bucket) 
				node[ (*s)[depth]%bfactor ].add_itemset(s);
			
			// clear the old leaf node memory
			bucket.clear();
		}

	}
	else // intermediate node so hash away
		node[ (*set)[depth]%bfactor ].add_itemset(set);
}

/*
 * Updates the support count of the input transaction
 * @param t 	The input transaction for which to update the support count
 * @return 		True if transaction updated support count. False otherwise. 
 */
bool HashTree::operator()(const Transaction &t) {
	/* reset visited flag */
	if ((int) t.size() < num_items)
		return false;
	else
		return traverse(t, 0);
}

/**
 * Traverse the hashtree with the given transaction so as to visit every candidate subset of t atleast once
 * @param t 				the transaction
 * @param indeces 	the index at which we hash the transaction for the current deoth
 */
bool HashTree::traverse(const Transaction &t, int index) {
	/* if already visited bucket then don't traverse again */
	bool has_updated = false;

	if (isLeaf) {
		/* special efficient case for k=2 */
		if (num_items==2) {
			if (depth == 2) {
				int t1=t[ indeces[0] ], t2=t[ indeces[1] ];
				for (const Itemset *set : bucket) {
					const Itemset &s = *set;
					if (t1==s[0] && t2==s[1]) {
						(*sigma)[s] += 1;
						has_updated = true;
					}
					// else if ( (t1==s[0]&&t2<s[1]) || t1<s[0] )
					// 	return has_updated;
				}
			}
			else if (depth == 1) {
				int i, i1, t_sz = (int) t.size(), t1 = t[ i1=indeces[0] ];
				for (const Itemset *set : bucket) {
					const Itemset &s = *set;
					if (t1==s[0]) {
						for (i=i1+1; i<t_sz && t[i]!=s[1]; i++);
						if (i<t_sz) {
							(*sigma)[s] += 1;
							has_updated = true;
						}
					}
					// else if (t1<s[0])
					// 	return has_updated;
				}
			}
			else 
				for (const Itemset *s : bucket)
					if ( std::includes(t.begin(), t.end(), s->begin(), s->end()) ) 
					{
						(*sigma)[*s] += 1;
						has_updated = true;
					}
		}
		else
			for (const Itemset *s : bucket) {
				bool match_prefix = true;
				int j=0, i=-1;
				for (int ii : indeces)
					if (t[i=ii] != (*s)[j++]) {
						match_prefix = false;
						break;
					}
				if (match_prefix) {
					if ( j==num_items || std::includes(t.begin()+index, t.end(), s->begin()+j, s->end()) ) 
					{
						(*sigma)[*s] += 1;
						has_updated = true;
					}
					
				}
				else if (t[i]<(*s)[0])
					return has_updated;
			}
	}
	else { // intermediate node
		// hash the transaction over each branch
		int t_sz = (int) t.size(), cap = t_sz-num_items+depth+1;
		cap = cap>t_sz? t_sz: cap;
		for (int j=index; j<cap; j++) {
			indeces.push_back(j);
			has_updated = node[ t[j]%bfactor ].traverse(t, j+1) || has_updated; // avoid short-circuit
			indeces.pop_back();
		}
	}

	return has_updated;
}


/**
 * Print the leaves of the hash tree
 */
void HashTree::print_leaves() {
	if (isLeaf) {
		for (const Itemset *s : bucket)
			print_itemset(*s);
		std::cout << std::endl<<std::endl;
	}
	else
		for (HashTree &t : node)
			t.print_leaves();
}