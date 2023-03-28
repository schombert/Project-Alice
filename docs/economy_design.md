# Economy design

This document provides a design overview for how the 1.0 economy will be implemented. The 1.0 economy is not an exact replica of the Victoria 2 economy. It is designed to be a close approximation, but some alterations have been made to improve performance. Substantially, this means we try to store as little information as possible and try to make as few passes over the data as possible. Unfortunately, even a close approximation requires substantial sequencing, and little parallelism is possible. The best we may be able to do is make the economy as self-contained as possible so that the remainder of the day's update can run alongside it.

## Terms

Virtual profitability -- how profitable one standardized unit of production *would* be *if* all inputs were available, with the worker bonuses as if they had full employment. (Accounting for the fraction of production sold in the previous day compared to actual production)

Production scale -- producer production will be scaled down by two factors: First: by the availability of inputs. Effective production scale is capped by input availability (and real production scale will drop down to meet this value). When supply is greater than real demand: production scale goes down at some rate (tbd -- and probably to some minimum amount, not to 0). When supply is less than real demand: production scale goes up at some rate. We may need a second derivative factor here to tamp down fluctuations, as this is a second feedback mechanism that is pushing in the same direction as price movements.

Closed -- a factory with a sufficiently low production scale is closed. Artisans with a sufficiently low production scale will change production, even if it is nominally profitable (as this means that they are having input availability problems)

## Daily update

We iterate over nations, in order of rank

### Initial state of domestic and global supply

The initial state of domestic supply is based on what was produced domestically, and global supply is what was produced domestically the day before that and not consumed. For the remainder of this document, we take domestic supply to mean "domestic supply, national stockpiles if buying from them is enabled, and whatever is left over in the sphere leader's domestic supply," which emulates the behavior of Victoria 2.

As the day starts, we move production, fractionally, into the sphere leaders domestic production pool, following the same logic as Victoria 2

#### Data required:

Per-nation production (= domestic_market_pool) for day N - 1

### Calculate effective prices

The two price problem:

To calculate demand and distribute what is available evenly throughout the nation's consumers, we need to know what the prices are to determine how much it is possible for a consumer to purchase given a particular income (and what their income would be assuming that they could sell their production). The problem is that tariffs, etc make purchasing from the global pool more expensive. Essentially, there are two prices, and the part that you can buy domestically may be cheaper (or more expensive, in the case of subsidies) than the commodity on the global market. And this means that the the effective cost depends on how much is purchased in total, which in turn depends on the cost ...

Victoria 2 solves this by essentially ordering the consumers in an arbitrary fashion. They make purchases in order, each buying from the domestic supply (yes, even if it is cheaper to buy from the global supply) until it runs out, and then the remainder have to deal with the price from the global supply. This is ... not great if those prices ever diverge significantly, because it means that some things may or may not be profitable based on where they fall in this arbitrary order.

We will solve this problem as follows: we will use the real demand from the *previous* day to determine how much of the purchasing will be done from the domestic and global pools (i.e. what percentage was able to be done from the cheaper pool). We will use that to calculate an effective price. And then, at the end of the current day, we will see how much of that purchasing actually came from each pool, etc. Depending on the stability of the simulation, we may, instead of taking the previous day, instead build this value iteratively as a linear combination of the new day and the previous day.

(If we were to be *more* realistic than Victoria 2, we could also calculate a second sales price ... but why should we, if the base game doesn't bother to).

#### Data required:

Per-nation real demand for day N - 1. Temporary per-commodity effective price buffer.

### Determine employment

The possible employees are distributed to factories by doing the following:

We sort factories by priority and then by *virtual* profitability (OK, as an implementation detail, it is easier to do a stable sort by profitability and then a stable sort by priority, with stability in both cases ensuring the order remains the same everywhere). No factory hires more workers than its production scale from the previous day would permit. Employment is only allowed to shift by so much in one day (tbd).

This applies to RGOs as well, although there is only one target for distribution.

#### Data required

Per-factory production scale, per-province RGO and artisan production scale. Per-factory and per-province RGO employment values

### Nation determines target and actual consumption

Desired consumption is calculated for all sources (pops, factories, artisans, national stockpiles, projects) based on how much they can buy given their money. Artisans and factories are assumed to borrow against their expected profit if they will be profitable given the current effective prices (i.e. we if they are virtually profitable, with employment effects factored in, and factories should take a minimum wage labor input cost into account as well for expected profit). If they are, they try to purchase as much as they can given the limits of their production scale and employment numbers (and if not their production scale trends downwards). Pops try to purchase as much as they can (up to their last income). Nations ... here we have a problem. It is quite natural to say: as much as they can given their reserves. This would mean that nations would never go into debt. Is that what we want? (Maybe for the AI and as a player option.) In any case, this gives us "real demand" numbers.

From this, we derive maximum limits on production scale for factories / artisans (But these numbers won't be entirely accurate: limits on commodity A availability may make more of B available because the production process using both A and B will consume less of B, leaving more of B available for a production process not limited by A. There is an algorithm that can solve for this--it is a form of linear optimization problem, which are solvable--but it is somewhat complicated.). We also scale down all other purchases in the same way.

With consumption scale calculated, we then subtract commodities from the various supply pools starting with the cheaper of global or domestic. Within domestic, we first subtract from true domestic, then from remaining sphere leader domestic, then from national stockpiles (if enabled).

#### Data required

Global supply pool. Per-pop previous-day income. Temporary per-commodity buffers for working out availability. National stockpiles.

### Effects of consumption

The commodities produced (factories, artisans, rgos) are placed into the domestic supply pool for the next day. The profit of factories and artisans, and the earnings of rgos are calculated, and from these values incomes for various employee pops are calculated for the next day. Pop purchases are used to calculate needs satisfaction, which is not a direct result of how much is consumed, but rather a linear combination of previous satisfaction, which simulates pop savings.

Any remaining domestic supply moves to the global supply for the next day.

### End of day

Real supply vs real demand causes prices to adjust. Any remaining supply in the global pool is trashed. We figure out how much of the total world production was purchased, as this will affect the effective sales price for the next day.

#### Data required

Global production values, actual consumption values (or maybe just the fraction).