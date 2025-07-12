# US3. Trade

**As a Nation,**
**I want markets associated to my states to trade goods between themselves,**
**So that the exchange resolves natural natural supply-demand disrepancies.**

**Acceptance Criteria:**
| AC1 | Markets are created on state level |
| AC2 | Markets have associated labour costs for trade routes management |
| AC3 | Markets have associated labour costs for trade inside the state |
| AC4 | Markets run exports and imports with trade routes |
| AC5 | Naval bases increase appeal of a state for trade routes by 25% |
| AC6 | Discounts on movement cost decrease the effective distance between the states increasing the appeal for trade routes appearance |
| AC7 | If disallow_naval_trade national modifier is non-zero then its states can't trade by sea |
| AC8 | If disallow_land_trade national modifier is non-zero then its states can't trade by land |
| AC9 | If parties are at war, the trade between them is paused (wartime embargo) |
| AC10 | If one of the parties sets an embargo against the counterparty, then parties can't trade between themselves |
| AC11 | If sphere leader has set up an embargo, its sphere members follow the embargo. |
| AC12 | If overlord has set up an embargo, its subjects follow the embargo. |
| AC13 | Merchants take a cut of the price difference between the origin and the target states |
| AC14 | In international trade, both export tariffs of the origin country and import tariffs of the target country are applied. |
| AC15 | Unequal treaty imposed on the nation disables its export tariffs on exports and import tariffs on imports. |
| AC16 | Wartime blockade stops the trade from the state market completely. |
| AC17 | Wartime occupation stops the trade from the state market completely. |
| AC18 | During travel, based on distance, some of the goods are lost. |
| AC19 | Local and precious goods cannot be traded. |
| AC20 | Not yet unlocked commodities aren't traded. |
| AC21 | Increase of volume reduces the transport cost. |
| AC22 | National `trade_routes_attraction` modifier changes appeal of its states for trade routes |

**Definition of Done:**
- [X] All acceptance criteria are met.
- [X] Code is reviewed and approved.
- [ ] Necessary tests are written and pass.
- [X] Documentation is updated, if applicable.
- [x] Feature is available in release versions of PA.
