#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "apriori.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "params.hpp"

using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

high_resolution_clock::time_point t1, t2, t3;

#ifdef _DEBUG // used by me in comparing my frequent itemsets with support count with those of professor's code
	void debug_write_frequent_itemsets(const std::string& fn, const std::vector<Itemsets>& F, const SupportCountTable& sigma);
#endif

int main(int argc, char *argv[]) {
	/* basic input validity checking */
	if (argc != 7) {
		printf("usage: ./hcrminer <minsup> <minconf> <inputfile> <outputfile> <hfrange> <maxleafsize>\n");
		exit(1);
	}

	/* get the params */
	Params params(argc, argv);
#ifdef _DEBUG
	params.print();
#endif

	/* get the transactions */
	DB T;
	int max_item = read_db(params.inputfile, T);
	int how_many = (int) T.size();

	/* start the clock */
	t1 = high_resolution_clock::now();

	/* get the frequent itemsets along with their support values */
	std::vector<Itemsets> F;
	SupportCountTable sigma;
	apriori(sigma, F, T, params, max_item);

	t2 = high_resolution_clock::now();

	/* get the rules */
	Rules rules;
	if (params.minsup > 20)
			get_rules(rules, sigma, F, params.minconf);

	t3 = high_resolution_clock::now();
	
	/* get execution times */
	int m_total, m_freq, m_rules; 
	m_total = duration_cast<milliseconds>(t3-t1).count();
	m_freq = duration_cast<milliseconds>(t2-t1).count();
	m_rules = duration_cast<milliseconds>(t3-t2).count(); 

	/* save the rules to output file */
	if (params.minsup > 20) {
		write_rules(params.outputfile, rules, how_many);
		std::cout << "# of rules: " << rules.size() << std::endl;
	}
	else {
		write_frequent_itemsets(params.outputfile, F, sigma, how_many);
		std::cout << "# of rules: " << -1 << std::endl;
	}
	
	size_t size=0;
	for (auto &v:F)
		size += v.size();
	std::cout << "# of frequent itemsets: " << size << std::endl;

	std::cout << "time-frequent: " << (float) m_freq / 1000.0 << std::endl;
	std::cout << "time-rules: " << (float) m_rules / 1000.0 << std::endl;
	std::cout << "time-total: " << (float) m_total / 1000.0 << std::endl;

#ifdef _DEBUG
	debug_write_frequent_itemsets(params.inputfile+"_frequent", F, sigma);
#endif

	return 0;
}

#ifdef _DEBUG
/**
 * Write the frequent itemsets in to the output file.
 * Format of output file: { <item1> <item2> <itemN> } <length> <support-count>.
 * Also the output file is sorted in increasing order of itemset size and lexicographically for itemsets with same size.
 * @param fn 			the name of the file
 * @param F 			the number of frequent itemsets
 * @params sigma 	the support count of each itemset
 */
void debug_write_frequent_itemsets(const std::string& fn, const std::vector<Itemsets>& F, const SupportCountTable& sigma) {
	std::stringstream ss;
	for (const Itemsets is : F)
		for (const Itemset s : is) {
			int sc = sigma.at(s);
			ss << "{ ";
			for (int i=0; i!=(int)s.size(); i++) {
				const Item item = s[i];
				ss << item;
				ss << " ";
			}
			ss << "} " << s.size() << " " << sc << "\n";
		}

	std::ofstream out(fn.c_str());
	out << ss.rdbuf();
	out.close();
}
#endif