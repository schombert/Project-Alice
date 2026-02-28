# User Stories
*Automatically generated file on 2026-02-24*

## US1.  Regiment construction

| AC4 |  All goods costs must be built |
| AC5 |  But no faster than construction_time |

## US2. Ships construction

| AC5 |  But no faster than construction_time |

## US3.  Trade

| AC2 | register trade demand on transportation labor |
| AC3 | register demand on local transportation |
| AC7 | US3AC8 Ban international sea routes or international land routes based on the corresponding modifiers |
| AC8 |  Ban international sea routes or international land routes based on the corresponding modifiers |
| AC9 |  Wartime embargoes |
| AC10 |  diplomatic embargos |
| AC11 |  sphere joins embargo |
| AC12 | subject joins embargo |
| AC15 |  Equal |
| AC17 |  if market capital controller is at war with market coastal controller is different |
| AC21 | effect of scale |

## US4.  War


## US5.  Warscore

| AC1 | Army arrives to province |
| AC2 | Army siege |
| AC4 |  What % of the province score should be counted towards occupation. [0.0f |
| AC5 |  count 50% of occupation score for wars declared after targetted war |

## US6.  Special Army Orders

| AC3 | Reset selected army orders |

## US7. Move and Siege army order

| AC1 | Handle  |
| AC2 | Enable  |

## US8. Strategic Redeployment army order

| AC1 | Movement finished  |
| AC2 | Button to order Strategic Redeployment |
| AC3 | Toggle strategic redeployment order on B. |

## US9. Pursue and engage army order

| AC1 | Command army to pursue the target |
| AC3 | Button to order pursue_to_engage |
| AC4 | Toggle pursue order on N. |

## US11. Army supplies


## US12. Regiment supplies


## US13. Regiments organization regain

| AC3 | US13AC4 US13AC5 Morale  |
| AC6 | Max organization of the regiment is 100%  |
| AC7 | Unfulfilled supply doesn't lower max org as it makes half the game unplayable |
| AC8 | Unfilfilled supply doesn't prevent org regain as it makes half the game unplayable |

## US14. Calculates reinforcement for a particular regiment


## US15. Navy supplies


## US16. Ship supplies


## US17. Ships organization regain


## US18. Ships repairs


## US31.  Map

| AC3 | If a valid province has been selected, reset selection of armies as well |

## US48. Economic Scene Commodities tab

| AC0 | Display data only if a commodity is selected |
| AC1 | On national level, when price option is selected, display median price |
| AC2 | On national level, when supply option is selected, display total supply  |
| AC3 | On national level, when demand option is selected, display total supply  |
| AC4 | On national level, when a production option is selected, display total production  |
| AC5 | On national level, when a consumption option is selected, display total consumption  |
| AC6 | On national level, when a stockpiles option is selected, display total stockpiles  |
| AC7 | On national level, when a potentials option is selected, display total potentials  |
| AC8 | On national level, when a balance option is selected, display total balance with logarithmic scale  |
| AC9 | On national level, when a trade_in option is selected, display total imports volume  |
| AC10 | On national level, when a trade_out option is selected, display total exports volume  |
| AC11 | On national level, when a trade_balance option is selected, display total trade_balance volume  |
| AC12 | On market level, when a price option is selected, display price |
| AC13 | On market level, when a supply option is selected, display commodity supply |
| AC14 | On market level, when a demand option is selected, display commodity demand |
| AC15 | On market level, when a production option is selected, display commodity production |
| AC16 | On market level, when a consumption option is selected, display commodity consumption |
| AC17 | On market level, when a stockpiles option is selected, display commodity stockpiles |
| AC18 | On market level, display potentials option only for commodities that use resource potentials  |
| AC19 | On market level, when a potentials option is selected, display commodity potentials |
| AC20 | On market level, when a balance option is selected, display commodity trade balance |
| AC21 | On market level, when a trade_in option is selected, display commodity trade_in volume |
| AC22 | On market level, when a trade_out option is selected, display commodity trade_out volume |
| AC23 | On market level, when a trade_balance option is selected, display commodity trade_balance volume  |

## US49. stacked_calculation class allows uniting number calculations for backend with explanation tooltips for the UI

| AC1 | stacked_calculation is constructed with initial float value  |
| AC2 | User can add a value with a string_view explanation of the reason |
| AC3 | User can subtract a value with a string_view explanation of the reason |
| AC4 | User can multiply current value by provided value with a string_view explanation of the reason |
| AC5 | User can get resulting value from the stack |
| AC6 | User can get all steps used for calculation for tooltips and UI |
| AC7 | User can reuse existing stack by clearing it |
| AC8 | User can reuse existing stack by resetting it to a different initial value |

## US50. The State constructs factories

| AC1 | Each factory type is evaluated by its profitability, payback time, and a number of synergies |
| AC2 | The State takes top 5 factory options for construction |
| AC3 | The State takes one random option for construction |
| AC4 | The State doesn't initiate more constructions if it doesn't have free funds |
| AC5 | The State doesn't evaluate the cost of the factory it constructs beyond inital analysis |
| AC10 | This is legacy flow that is no longer called for State Constructions, refer to US52 |

## US51. 

| AC6 | Private Investment takes top 5 factory options for contrustion |

## US52. Nation builds some random factories if it can build by itself

| AC2 | Exclude already present factories |
| AC3 | Construction stops when state budget runs out of construction budget appetite |

## US53. Build Factory Window

| AC10 | Tooltip for a factory type displays if the factory type has been activated with a technology |
| AC11 | Tooltip for a factory type displays if the target state is a colony and target factory type can be built in colonies |
| AC12 | Tooltip for a factory type displays if the factory type requires potentials and target state doesn't have required potentials |
| AC13 | Tooltip for a factory type displays key economic metrics of the potential construction |
| AC14 | Tooltip for a factory type displays the score AI places to the factory type |

## US101. Sieges and Occupations

| AC2 | Forts reduce siege speed by alice_fort_siege_slowdown factor  |
| AC3 | Forts increase hostile siege attrition by state.defines.alice_fort_siege_attrition_per_level per level |
| AC4 | Calculate victory points that a province P is worth in the nation N. Used for warscore, occupation rate. |
| AC5 | defines the general algorithm for getting the effective fort level with said amount of total strength of units who are enemies with the fort controller, |
