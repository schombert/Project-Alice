# US7. Scripting

- Moved links to triggers & effects away from UI element definition into a special `scripted_interaction` DCON table. This reduces memory usage (there is over 8k UI elements in the basegame each having 6 bytes for these links) and increases calculations during UI update (find the interaction ID from GUI element ID).
- AI can now use scripted buttons that have `ai_will_do` evaluation defined by iterating over a limited number of scripted interactions along with decisions.


**As a Modder,**
**I want to mod scripted buttons,**
**So that I add extra interactions to the game.**

**Acceptance Criteria:**
| AC1 | Allow trigger is parsed |
| AC2 | Visibility trigger is parsed |
| AC3 | Effect is parsed |
| AC4 | Ai_will_do block is parsed |
| AC5 | AI takes national interactions |

**Definition of Done:**
- [X] All acceptance criteria are met.
- [X] Code is reviewed and approved.
- [ ] Necessary tests are written and pass.
- [X] Documentation is updated, if applicable.
- [x] Feature is available in release versions of PA.
