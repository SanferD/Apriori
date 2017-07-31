#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <ctime>
#include <cstdlib>

/**
 * Reads the input file given by fn.
 * Assumes that the transaction ids are ordered increasingly in the file.
 * Assumes that each transaction in input file has a unique element. 
 * Puts all items belonging to a transaction in a vector.
 * It then sorts all the transactions in increasing order
 * @param  fn      The input file
 * @param  out_db [out param] The vector of sorted transactions
 * @return         The largest item found. Range is therefore [1, max_item].
 */
int read_db(const std::string& fn, DB& out_db) {
	/* open inputfile and read contents into string stream */
	std::stringstream ss;
	std::ifstream in(fn.c_str());
	if (!in.is_open()) {
		printf("incorrent input filename \'%s\'\n", fn.c_str());
		exit(1);
	}
	ss << in.rdbuf();
	in.close();

	/* Clean slate */
	out_db.clear();

	/* read all transactions */
	int old_tid=-1, max_item=-1, tid, item;
	while (ss >> tid >> item) {
		if (old_tid != tid) {
			old_tid = tid;
			out_db.push_back(Transaction());
		}
		if (item > max_item)
			max_item = item;

		out_db.back().push_back(item);
	}

	/* sort transactions in ascending order */
	for (Transaction t : out_db)
		std::sort(t.begin(), t.end());

	/* return max item */
	return max_item;
}

/**
 * Write the frequent itemsets in to the output file.
 * Format of output file: { <item1> <item2> <itemN> } <length> <support-count>.
 * Also the output file is sorted in increasing order of itemset size and lexicographically for itemsets with same size.
 * @param fn 			the name of the file
 * @param F 			the number of frequent itemsets
 * @params sigma 	the support count of each itemset
 */
void write_frequent_itemsets(const std::string& fn, const std::vector<Itemsets>& F, const SupportCountTable& sigma, const int how_many) {
	std::stringstream ss;
	ss.precision(3);
	for (const Itemsets sets : F)
		for (const Itemset set : sets) {
			int sc = sigma.at(set);
			for (Item i : set)
				ss << i << " ";
			ss << "| {} | " << (float)sc/(float)how_many << " | -1\n";
		}

	std::ofstream out(fn.c_str());
	out << ss.rdbuf();
	out.close();
}


/**
 * Calculates set1 - set2. 
 * Assumes set1 and set2 are ordered lexicographically.
 */
Itemset set_dif(const Itemset &set1, const Itemset &set2) {
	Itemset result;
	for (Item i1 : set1) {
		bool insert = true;
		for (Item i2 : set2)
			if (i2 >= i1) {
				if (i1 == i2)
					insert = false;
				break;
			}
		if (insert)
			result.push_back(i1);
	}
	return result;
}

/**
 * Write the rules to file
 * @param fn    		The output file
 * @param rules 		The rules
 * @param how_many	Number of transactions
 */
void write_rules(const std::string& fn, const Rules& rules, const int how_many) {
	std::stringstream ss;
	ss.precision(3);
	for (Rule rule : rules) {
		for (Item i : rule.LHS)
			ss << i << " ";
		ss << "| ";
		for (Item i : rule.RHS)
			ss << i << " ";
		ss << "| " << (float)rule.sup/(float)how_many << " | " << rule.conf << "\n";
	}

	std::ofstream out(fn.c_str());
	out << ss.rdbuf();
	out.close();
}

/**
 * Print the itemset
 * @param set The itemset to print
 */
void print_itemset(const Itemset& set) {
	std::cout << "{ ";
	for (Item i : set)
		std::cout << i << " ";
	std::cout << "}" << std::endl;
}
