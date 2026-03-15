# US11 Army supplies

# US12 Regiment supplies

# US13 Regiments organization regain

**As a Nation,**
**I want to replenish the organization of my regiments,**
**So that they can fight in the next battles.**

**Acceptance Criteria:**
| AC1 | Organization of regiments is replenished daily |
| AC2 | Organization regain is reduced by the fulfilled army upkeep (army supply) |
| AC3 | National `org_regain` modifier increases organization regain rate |
| AC4 | Traits of the general can increase organization regain rate |
| AC5 | Prestige (experience) of the general increases organization regain rate |
| AC6 | Max organization of the regiment is 100% (1.0) |
| AC7 | Unfulfilled supply doesn't lower max org as it makes half the game unplayable |
| AC8 | Unfilfilled supply doesn't prevent org regain as it makes half the game unplayable |

**Definition of Done:**
- [X] All acceptance criteria are met.
- [X] Code is reviewed and approved.
- [ ] Necessary tests are written and pass.
- [X] Documentation is updated, if applicable.
- [x] Feature is available in release versions of PA.

# US14 Regiments reinforcement

# US15 Navy supplies

# US16 Ship supplies

# US17 Ships organization regain

# US18 Ships repairs

# US101 Sieges and Occupations

| AC1 | Forts are built once with construction cost of some goods comparable to an extra factory level. |
| AC2 | Forts reduce siege speed by alice_fort_siege_slowdown factor (0.75 by default) per level. |
| AC3 | Forts increase hostile siege attrition by state.defines.alice_fort_siege_attrition_per_level per level (0.35 by default). | 
| AC4 | Forts moderately impact victory points score of a province (+1 per fort level). |
| AC5 | Engineers reduce the effective fort level. Number of engineers required to reduce by 1 level depends on the sieging army size. At least state.defines.engineer_unit_ratio required (0.1 by default) |
| AC6 | Recon units increase occupation speed of any province. At least state.defines.recon_unit_ratio is required (0.1 by default) |
| AC7 | Nation sieges its owned provinces 25% faster. |
| AC8 | Nation sieges its core provinces 10% faster. |
