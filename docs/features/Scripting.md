# Scripting

Adding new buttons wouldn't mean much if you couldn't make them do things. To allow you to add custom button effects to the game, we have introduced two new ui element types: `provinceScriptButtonType` and `nationScriptButtonType`. These buttons are defined in the same way as a `guiButtonType`, except that they can be given additional `allow` and `effect` parameters. For example:

```
	provinceScriptButtonType = {
		name = "wololo_button"
		extends = "province_view_header"
		position = { x= 146 y = 3 }
		quadTextureSprite = "GFX_wololo"
		visible = {
			tag = USA
		}
		allow = {
			owner = { tag = FROM }
		}
		effect = {
			assimilate = "yes please"
		}
	}

	nationScriptButtonType = {
		name = "wololo_button"
		extends = "province_view_header"
		position = { x= 146 y = 3 }
		quadTextureSprite = "GFX_wololo"
		visible = {
			tag = USA
		}
		allow = {
			owner = { tag = FROM }
		}
		effect = {
			assimilate = "yes please"
		}
		ai_will_do = {
			always = yes
		}
	}
```

How does it work:
- A province script button has its main and THIS slots filled with the province that the containing window is about, with FROM the player's nation.
- A nation script button has its main and THIS slots filled with the nation that the containing window is about, if there is one, or the player's nation if there is not, and has FROM populated with the player's nation.
- The `visible` trigger condition is optional and is used to determine when the button is rendered. If the allow condition is omitted, the button will always be enabled.
- The `allow` trigger condition is optional and is used to determine when the button is enabled. If the allow condition is omitted, the button will always be enabled.
- The tooltip for these scriptable buttons will always display the relevant allow condition and the effect. You may also optionally add a custom description to the tooltip by adding a localization key that is the name of the button followed by `_tooltip`. In the case of the button above, for example, the tooltip is defined as `wololo_button_tooltip;Wololo $PROVINCE$`. The following three variables can be used in the tooltip: `$PROVINCE$`, which will resolve to the targeted province, `$NATION$`, which will resolve to the targeted nation or the owner of the targeted province, and `$PLAYER$`, which will always resolve to the player's own nation.
- AI evaluates national scripted interactions once a month in a similar way to decisions.
- AI doesn't use province scripted interactions.

## Technical side

### US7. Scriptable buttons

Recent changes:

- SneakBug8: Moved links to triggers & effects away from UI element definition into a special `scripted_interaction` DCON table. This reduces memory usage (there is over 8k UI elements in the basegame each having 6 bytes for these links) and increases calculations during UI update (find the interaction ID from GUI element ID).
- SneakBug8: AI can now use scripted buttons that have `ai_will_do` evaluation defined by iterating over a limited number of scripted interactions along with decisions.

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
