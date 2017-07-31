# Apriori



To build, run make.
Only tested on Linux.

To run,
./hcrminer `<`minsup`>` `<`minconf`>` `<`inputfile`>` `<`outputfile`>` `<`hfrange`>` `<`maxleafsize`>`

What does all of that mean?

Apriori is a data mining algorithm which looks at an unordered set of transactions and tries to derive relationships between different transactions.
For example, each transaction would be the set of groceries one customer buys in a supermarket while the set of all transactions would be the dataset which is mined.
Apriori looks into each basket and tries to predict behavioral patterns of each customer.
As in, it tries to determine that if customer buys eggs and milk, then customer is likely to buy bread.
Such relations are called rules.
Since this is probabilistic, each rule has a confidence associated with it.
That confidence is the `<`minconf`>` input.
Moreover, some set of items, i.e. the condition part of the if-then rule, may occur infrequently.
For example, only a few customers would buy cheese heads and viking caps simultaneously.
Infering rules using such infrequent transactions would be inconclusive because there isn't enough data to perform any predictions.
Therefore it is preferable to eliminate such infrequent transactions.
The <minsup> input parameter determines exactly this.
It is the minimum number of times a set of items in should be found in all transactions.
<hfrange> and <maxleafsize> are parameters relating to internal datastructures.
You can set <hfrange> to 10 and <maxleafsize> to 50.

