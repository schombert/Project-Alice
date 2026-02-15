# User Stories
*Automatically generated file on 2026-02-10*

## US1.  Regiment construction

| AC4 |  All goods costs must be built |
| AC5 |  But no faster than construction_time |

## US2. Ships construction

| AC5 |  But no faster than construction_time |

## US3.  Trade

| AC9 |  Wartime embargoes |
| AC10 |  diplomatic embargos |
| AC11 |  sphere joins embargo |
| AC15 |  Equal |
| AC17 |  if market capital controller is at war with market coastal controller is different |
| AC2 | register trade demand on transportation labor |
| AC3 | register demand on local transportation |
| AC7 | US3AC8 Ban international sea routes or international land routes based on the corresponding modifiers |
| AC8 |  Ban international sea routes or international land routes based on the corresponding modifiers |
| AC12 | subject joins embargo |
| AC21 | effect of scale |
| AC5 | Trade Route Attraction from naval bases |
| AC22 | Trade Route attraction modifier |

## US4.  War and peace


## US5.  Warscore

| AC4 |  What  |
| AC5 |  count 50 |
| AC1 | Army arrives to province |
| AC2 | Army siege |

## US6.  Ticking warscore

| AC3 | Reset selected army orders |
| AC20 | Reduce accumulated ticking warscore when the side adds a new wargoal |
| AC2 | US6AC3 |
| AC12 | US6AC13 US6AC17 US6AC18 US6AC19 Ticking warscore may go into negative only after grace period ends |
| AC5 | US6AC6 Ticking warscope for make_puppet war |
| AC7 | We hold some non |
| AC8 | Battle score |
| AC10 | US6AC17 US6AC18 US6AC19 |

## US7. Move and Siege army order

| AC2 | Enable  |
| AC1 | Handle  |

## US8. Strategic Redeployment army order

| AC3 | Toggle strategic redeployment order on B. |
| AC2 | Button to order Strategic Redeployment |
| AC1 | Movement finished  |

## US9. Pursue and engage army order

| AC1 | Command army to pursue the target |
| AC3 | Button to order pursue_to_engage |
| AC4 | Toggle pursue order on N. |

## US11. Army supplies


## US12. Regiment supplies


## US13. Regiments organization regain

| AC3 | US13AC4 US13AC5 Morale  |
| AC7 | Unfulfilled supply doesn't lower max org as it makes half the game unplayable |
| AC8 | Unfilfilled supply doesn't prevent org regain as it makes half the game unplayable |
| AC6 | Max organization of the regiment is 100 |

## US14. Calculates reinforcement for a particular regiment


## US15.  Host settings


## US16.  Player accounts


## US17. Ships organization regain


## US18. Ships repairs


## US20. De facto annexations


## US27.  Scriptable buttons

| AC5 | National level interactions first |
| AC1 | US27AC2 US27AC3 US27AC4 |

## US28.  Toggleable windows

| AC5 | US28AC6 UI variable toggle buttons can have nation |
| AC2 | When clicking button with toggle_ui_key the associated UI variable is toggled True |
| AC4 | Window is shown only when UI variable in `visible_ui_key` is set to True |

## US29.  Scriptable images with dynamic frames through datamodels

| AC2 | US29AC3 When an icon has `datamodel |
| AC3 | US29AC3 When an icon has `datamodel |
| AC4 | US29AC5 Extracted into a separate function because both icon classes  |
| AC6 | US29AC7 |

## US31.  Map

| AC3 | If a valid province has been selected, reset selection of armies as well |

## US40.  Winter

| AC1 | Winter textures |
| AC2 | Winter siege attrition |

## US49. StackedCalculationWithExplanations allows uniting number calculations for backend with explanation tooltips for the UI

| AC1 | StackedCalculationWithExplanations is constructed with initial float value |
| AC2 | User can add a value with a string_view explanation of the reason |
| AC3 | User can subtract a value with a string_view explanation of the reason |
| AC4 | User can divide current value by provided value with a string_view explanation of the reason |
| AC5 | User can get resulting value from the stack |
| AC6 | User can get all steps used for calculation for tooltipsand UI |
| AC7 | User can reuse existing stack by clearing it |
| AC8 | User can reuse existing stack by reseting it to a different initial value |

## US50. The State constructs factories

| AC1 | Each factory type is evaluated by its profitability, payback time, and a number of synergies |
| AC2 | The State takes top 5 factory options for construction |
| AC4 | The State doesn't initiate more constructions if it doesn't have free funds |
| AC3 | The State takes one random option for construction |
| AC5 | The State doesn't evaluate the cost of the factory it constructs beyond inital analysis |

## US51. Private investment fund builds factories

| AC6 | Private Investment takes top 5 factory options for contrustion |
| AC1 | Investment fund doesn't build if it's over courage with current investments |
| AC2 | Colonial pops are eligible for construction of certain factories |
| AC3 | Provinces are sorted by population descending |
| AC4 | If the province an existing construction, no new construction will be run |
| AC5 | If the state has over state.defines.factories_per_state factories, no new construction will be done |
| AC7 | Private Investment takes random option out of top 5 |
| AC8 | Private Investment doesn't get afraid of expensive constructions. It initiates one and stop the following ones |

## US52. Nation builds some random factories if it can build by itself

| AC2 | Exclude already present factories |
| AC3 | Construction stops when state budget runs out of construction budget appetite |

## US53. Build Factory Window

| AC10 | Tooltip for a factory type displays if the factory type has been activated with a technology |
| AC11 | Tooltip for a factory type displays if the target state is a colony and target factory type can be built in colonies |
| AC12 | Tooltip for a factory type displays if the factory type requires potentials and target state doesn't have required potentials |
| AC13 | Tooltip for a factory type displays key economic metrics of the potential construction |
| AC14 | Tooltip for a factory type displays the score AI places to the factory type |

## US1010. When a factory is created, do market adjustments

| AC1 | When a factory is created, add 10 units of its output to local market to stimulate consumption and demand registration |
| AC2 | when a factory is created, increase price of its inputs to stimulate producing those |
