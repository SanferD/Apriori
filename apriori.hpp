#ifndef _APRIORI_HPP
#define _APRIORI_HPP

#include "utils.hpp"
#include "params.hpp"
#include "types.hpp"

#include <vector>

void apriori(SupportCountTable &sigma, std::vector<Itemsets> &F, DB &T, const Params &params, const Item maxitem);
void apriori_gen(Itemsets &C, const Itemsets &f_set);
void get_rules(Rules &rules, const SupportCountTable &sigma, const std::vector<Itemsets> &F, const float minconf);
void ap_genrules(Rules &rules, const SupportCountTable &sigma, Itemset& fk, Itemsets& H, const float minconf);
void prune_db(DB& T, const Itemsets& Fk);

#endif