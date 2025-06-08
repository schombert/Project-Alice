# Project Alice v1.2.3 Changelog

Assembled by SneakBug8

## by Schombert



## by Peter

- Local Administrations and Province Control.

**Province-level economy**

- Move labour market, factories and artisans logic to provinces
- Implement many helper functions to help with the future rework of production window
- Mostly unify production, consumption and employment logic of artisans, rgo and factories
- Implement efficiency goods for rgos: rgos decide on their efficiency level which at some point requires efficiency goods to expand
- Fix a few bugs and tweak values to adapt economy to new logic

- Economy rebalance and bugfixes
- Small adjustments to migration and bureaucracy
- Visual urban sprawl on the map
- Smoother borders and graphics improvements
- Fix lingering constructions, allow to blockade occupied land. 

## by SneakBug8

- War exhaustion in AI peace offer evaluation
- AI properly research doctrines costing leadership points
- AI won't ally weak neighbours
- A bit smarter construction
- AI army size estimation
- AI won't upgrade units if shortages
- change_factory_limit effect for mods
- Modding option to make always available wargoals cost infamy.
- Fixes to crisis mechanics
- Fix lost localization for free trade & embargoes 
- Hotfix to national stockpiles: do not use commodities until draw mode is selected
- Expanded provid tooltip
- AI embargoes.
- Tooltip showing all embargoes by/targeting the country in the diplomacy window.

## by MeizhouWan

  - Updated Chinese localisation

## by ZombieFreak115

Refactors the land combat str/org damage formula into its own function to avoid repetitions, and also thereby makes defenders use the "defence" stat, as opposed to only using attack. Also refactored land target picking logic into its own function.

Adds siege units reducing fort levels in battles, aswell as refactoring the general "get effective fort" formula into its own function.

Adds recon units being able to reduce dig-in levels, and dig-in levels being reduced by 1 every time the attacker rolls higher than defender behavoir from base V2.

Adds percentages for siege and recon efficiency in the army window, and tooltip for dig-in, and other minor dig in changes.

Max organisation implemenation of military units & other military QoL and fixes.

Implenting allowing reserves in battle to reinforce/reorg, among other things. 

Leader selection fix .
