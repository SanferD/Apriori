#include "apriori.hpp"
#include "hash_tree.hpp"
#include "types.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>

#ifdef _DEBUG
#define	DPRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DPRINT(fmt, ...) 
#endif

#ifdef _DEBUG
	int delete_counter=0; // used to count how many candidates were deleted
#endif


/**
 * Performs the apriori algorithm for finding frequent itemsets.
 * @param sigma  	[out_parameter] the support counts for all itemsets
 * @param F      	[out_parameter] a vector of frequent itemsets where F[k] is the vector of all frequent k-itemsets
 * @param T      	all transactions
 * @param params 	the parameters used, i.e., minsup, hfrange, maxleafsize 
 * @param maxitem the largest item found in all transactions
 */
void apriori(SupportCountTable &sigma, std::vector<Itemsets> &F, DB &T, const Params &params, const Item maxitem) {
	/* extract relevant parameters */
	int minsup = params.minsup, hfrange = params.hfrange, maxleafsize = params.maxleafsize;

	/* clean slate */
	F.clear();

	/* find all 1-frequent itemsets */
	F.push_back(Itemsets()); // initially empty 1-frequent itemsets
	for (Transaction t : T) // get the suport count for all 1-itemsets
		for (Item i : t)
			sigma[ Itemset({i}) ] += 1;
	for (Item item=0; item<=maxitem; item++) {// extract the frequent 1-itemsets
		Itemset s({item}); 
		if (sigma[s] >= minsup)
			F.back().push_back(s);
		else
			sigma.erase(s);
	}

	/* reserve sigma for support counting */
	sigma.reserve(F.back().size()*1000);

	DPRINT("total number of 1-itemsets: %i\n", maxitem);
	DPRINT("# of frequent 1-itemsets: %zu\n", F.back().size());
	DPRINT("# of transactions: %zu\n", T.size());

	/* Generate all frequent itemsets */
	for (int k=1; F.back().size()>1; k++) {
		DPRINT("\n>>> k : %i\n", k);

		/* Prune the database */
		prune_db(T, F.back());
		DPRINT("# of transactions: %zu\n", T.size());

		/* Generate the candidate itemset for this level */
		Itemsets C;
		apriori_gen(C, F.back());
		if (C.size() == 0)
			break;

		DPRINT("# of generated candidates pruned: %i\n", delete_counter);
		DPRINT("# of remaining candidates: %zu\n", C.size());

		/* Generate the hash tree for the candidate itemsets and perform support counting */
#ifdef _DEBUG
		int counter;
#endif
		HashTree hash_tree(C, &sigma, hfrange, maxleafsize);
		for (auto it=T.begin(); it!=T.end(); )
			if ( !hash_tree(*it) ) {
				it = T.erase(it);
#ifdef _DEBUG
				counter++;
#endif				
			}
			else
				it++;
		DPRINT("# of transactions pruned by ht: %i\n", counter);

		/* Get the frequent itemsets */		
		F.push_back(Itemsets());
		for (const Itemset &s : C)
			if (sigma[s] >= minsup)
				F.back().push_back(s);
			else
				sigma.erase(s);
		DPRINT("# of frequent %i-itemsets: %zu\n", k+1, F.back().size());
	}

}


/**
 * Generate the next level candiates from the current level frequent itemsets.
 * Uses Fk-1 Fk-1 merge. Assumes that each frequent item is ordered lexicographically.
 * Also assumes that the sequence of itemsets are ordered lexicographically.
 * @param out_C [out_param] the candidate itemsets to generate 
 * @param f_set the frequent itemsets from which to create the candidates. Assumes f_set not empty.
 */
void apriori_gen(Itemsets &out_C, const Itemsets &f_set) {
#ifdef _DEBUG
	delete_counter=0;
#endif

	int k = f_set.back().size();
	if (k == 1) // special efficient case for merging 1-itemsets
		for (auto it1=f_set.begin(); it1!=f_set.end()-1; it1++)
			for (auto it2=it1+1; it2!=f_set.end(); it2++) {
				out_C.push_back( Itemset({it1->back(), it2->back()}) );
			}
	else // merge frequent k-itemsets to get (k+1)-itemsets, k>1
		for (int i=0, f_sz = (int) f_set.size(); i!=f_sz; i++) {
			const Itemset& s1=f_set[i];		
			for (int j=i+1; j<f_sz; j++)
				if (std::equal(s1.begin(), s1.end()-1, f_set[j].begin())) {
					do {
						/* create the candidate */
						Itemset s(s1);
						s.push_back(f_set[j].back());

						/* level-wise pruning trick: check if all k-itemset subsets of s are contained in f_set   */
						bool to_add = true;
						int s_sz = (int) s.size();
						for (int m, ii=s_sz-1; ii>=0; ii--) { // ii corresponds to the index of the removed element in s to create k-itemset
							for (m=i; m<f_sz; m++) {

								const Itemset& fm = f_set[m];
								if ( !std::equal(s.begin(), s.begin()+ii, fm.begin()) ) { 
									m=f_sz;
									break;
								}
								else if ( std::equal(s.begin()+ii+1, s.end(), fm.begin()+ii) )
									break;
							
							}
							if (m>=f_sz) {// did not find a match
								to_add = false;
								break;	
							}
						}

						/* add candidate to the set of all candidates */
						if (to_add)
							out_C.push_back(s); 
#ifdef _DEBUG
						else
							delete_counter++;
#endif
					} while (++j<f_sz && s1[k-2]==f_set[j][k-2]);
				}
		}
}

/**
 * Get the rules corresponding to the provided frequent itemsets
 * @param rules   [out_param] the found rules
 * @param sigma 	the support count of itemsets
 * @param F       the frequent itemsets found using apriori
 * @param minconf the minimum confidence which the rules have to meet
 */
void get_rules(Rules &rules, const SupportCountTable &sigma, const std::vector<Itemsets> &F, const float minconf) {
	rules.clear();
	/* for each k-itemset, k>=2 */
	for (auto it=F.begin()+1; it!=F.end(); it++)
		for (Itemset fk : *it) {
			/* create possible 1-item consequents from the items of fk and push into H */
			Itemsets H;
			for (Item i : fk)
				H.push_back( Itemset({i}) );

			/* get rules for fk */
			ap_genrules(rules, sigma, fk, H, minconf);
		}
}

/**
 * Generates the rules with the given input frequent k-itemset, k>=2
 * @param rules   [out_param] the generated rules
 * @param sigma   the support count of evrey frequent itemset
 * @param fk      the frequent itemset for which we generate the rules
 * @param H       the candidate m-itemsets for the rule-consequent, k>m>=1
 * @param minconf the minimum acceptable confidence
 */
void ap_genrules(Rules &rules, const SupportCountTable &sigma, Itemset& fk, Itemsets& H, const float minconf) {
	int sup;
	if (fk.size() > H.back().size()) {
		for (auto iter=H.begin(); iter!=H.end(); ) {
			Itemset& h=*iter, dif;
			std::set_difference(fk.begin(), fk.end(), h.begin(), h.end(), std::back_inserter(dif));
			/* dif subset of fk and fk is frequent so sigma.at(dif) is not 0 */
			float conf = (float)( sup=sigma.at(fk) )/(float)sigma.at(dif);
			if (conf >= minconf) {
				rules.push_back( Rule(dif, h, sup, conf) );
				iter++; // only move forward if we didn't erase anything
			}
			else {
				iter = H.erase(iter);
			}
		}
		if (H.size() > 0) {
			Itemsets next_H;
			apriori_gen(next_H, H);
			if (next_H.size() > 0)
				ap_genrules(rules, sigma, fk, next_H, minconf);			
		}
	}
}

/**
 * Prune the data base of any transactions which do not contain frequent itemsets
 * @param T  The database to prune
 * @param Fk All frequent k-itemsets
 */
void prune_db(DB& T, const Itemsets& Fk) {
	/* Prune those transactions which do not contain frequent itemsets */
	int k = (int) Fk.back().size();
	for (auto it=T.begin(); it!=T.end();) {
		bool prune = true;
		if (k <= (int)it->size())
			for (const Itemset& s : Fk)
				if ( std::includes(it->begin(), it->end(), s.begin(), s.end()) ) {
					prune = false;
					break;
				}
		if (prune)
			it = T.erase(it);
		else
			it++;
	}
}