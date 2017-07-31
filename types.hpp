#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <list>
#include <map>
#include <unordered_map>
#include <vector>

typedef int Item;
typedef std::vector<int> Transaction;
typedef std::list<Transaction> DB;
typedef Transaction Itemset;
typedef std::vector<Itemset> Itemsets;

/**
 * Hash function used to store support counts for each itemset.
 * HashFunction: http://stackoverflow.com/q/4948780/4646773
 * ZeroInitialized new inserts: http://stackoverflow.com/a/8943287/4646773
 */
class ItemsetHash {
public:
	size_t operator() (const Itemset &itemset) const {
		std::size_t seed = itemset.size();
		for (Item i : itemset)
			seed ^= i + 0x9e3779b97f4a7c15 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
typedef std::unordered_map<Itemset, int, ItemsetHash> SupportCountTable;

/**
 * Rule
 */
struct Rule {
	Itemset LHS, RHS;
	int sup;
	float conf;
	Rule(Itemset a, Itemset b, int s, float c) : LHS(a), RHS(b), sup(s), conf(c) {}
};

typedef std::list<Rule> Rules;

#endif