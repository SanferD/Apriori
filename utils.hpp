#ifndef _UTILS_HPP
#define _UTILS_HPP

#include "types.hpp"

#include <string>
#include <vector>

int read_db(const std::string& fn, DB& out_vec);
void write_frequent_itemsets(const std::string& fn, const std::vector<Itemsets>& F, const SupportCountTable& sigma, const int how_many);
Itemset set_dif(const Itemset &set1, const Itemset &set2);
void write_rules(const std::string& fn, const Rules& rules, const int how_many);
void print_itemset(const Itemset& set);

#endif