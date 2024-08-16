# Automation

Project Alice offers several automation features to help with some of busywork inherited from Victoria 2.

## Battleplanner
This feature helps with:
- Moving a blob of regiments across sea
- Sieging undefended islands and territories
- Keeping defensive stacks in selected provinces.

To open battleplanner, press `Ctrl-Z` hotkey.

### Army Groups
Main concept of this feature is Army Group. Any Army Group consist from:
- List of attached regiments.
- List of attached navies.
- HQ - headquarters. This is a location where all attached idle regiments will gather.
- List of Defend orders. Army group will try to keep armies in location up to supply limit. It will attempt to keep ratio of unit types similar to ratio of units in the Army Group
- List of Siege orders. Army group will try to siege these locations if possible
- List of Ferry orders. Army group will try to use these provinces as main ports to transport troops.

Interactions with army groups are straightforward.

- To create Army Group, select province and press `Create Army Group` button above the list of army groups, (hotkey: `Z`). This action will create a new army group and set selected province as HQ.
- All created Army Groups are listed in the window on the right. You can select and deselect Army Group by clicking items in the list.
- To add new regiments or navies or remove them, press `Add/Remove units` button. It will move you to "Add units to battleplanner" scene. Select desired units and press the according button.


## Enable AI for units
TODO: explain how feature works

Note: this feature doesn't interact well with Battleplanner (yet), do not enable it for regiments attached to it.

## Rebel Hunting
TODO

Note: this feature doesn't interact well with Battleplanner (yet), do not enable it for regiments attached to it.

## Tech Queue
TODO

## Automatic selection of event choices
TODO

## Diplomacy QOL
TODO