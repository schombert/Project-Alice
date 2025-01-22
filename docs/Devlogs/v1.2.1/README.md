# Project Alice v1.2.1 Changelog
Assembled by SneakBug8

## by Schombert

  - Complete rework of macrobuilder window UI
  - Complete rework of budget window UI

## by Peter

**National industry specialization (window hotkey: Ctrl+N)**. Nations can specialize their industry in particular factory types. Specialization reduces inputs: 100k points invested reduces inputs by 50%. 0.1% of invested points are lost daily. Specialization points are generated 1:1 with research points. 50% of specialization points are managed by the capitalists (private specialization) which always try to maximize profit and other 50% are managed by the state.

Private specialisation mostly follows existing setup of factories, so if you focus on small arms production and thus increase amount of money private investors spend on inputs for them, it encourages them to follow.

AI plays specialization as a race. Once it lost specialization race in one field, it'll try to respec into a different industry.

Other bits and pieces:
  - Rebalanced investment pool drains
  - Rebalanced trade routes volumes and g eneration algorithm
  - Rebalanced construction simulation during presim

## by SneakBug8

**Conferences.** Declaring a war, nation can choose to run it as a conference instead. Conferences use existing Crisis mechanics. It allows for potential diplomatic resolution to what would have been war.

**Crises and Conferences.** If crisis failed to attrack GP as defender then target becomes defender.

**Dynamic CB Infamy Cost.** War justification infamy cost is dynamic. For state-level CBs cost is modified by state population to the civ/unciv average. For country-level CBs cost is modified by country population to the civ/unciv average. Civ/unciv average helps prevent weird situations where annexing Belgium costs 2 infamy because unciv states have much higher average population than Europe. Population cost multipliier is clamped between 0.1 and 2.0.

War justification for state-level CBs (such as demand state) requires selection of the state prior to the declaration of war.

**Subject payments.** Subjects pay their overlords a share of their collected tax revenue. By default, a puppet pays 30% and a substate - 40%. Subjects payments can be seen in the budget screen under diplomatic expenses. Payments can be changed by mods with defines:

  - alice_substate_subject_money_transfer=30.0
  - alice_puppet_subject_money_transfer=40.0

**Investment Pool of subjects and overlords.** When private investment pool of the subject doesn't have anything good to do - it transfers 2% of the pool up to overlord's investment pool. When private investment pool of the overlord doesn't have anything good to do - it distributes 2% of the pool between subjects' investment pools.

**Reinforcement and repair costs.** Reinforcing regiments and repairing ships costs goods. 1% of replenishment costs 1% of base unit cost. If these goods are unavailable (spending is reduced or goods weren't available on the market) then reinforcement/repair isn't done.

**Unit Upgrades.** Regiments and ships inside an army or a navy may be changed into different unit type. For that, first select the units (use Shift+Click for multiple selection), then click upgrade button and select a unit type. After swap unit will be reduced to 1% strength. Since 1% of replenishment costs 1% of base unit cost, unit upgrade costs the same as unit construction for the new unit. The following rules for possible conversions apply:

  - Regiment/ship types must be researched and available to the owner nation.
  - Small ships cannot be converted into capitol (big) ships.

**Player password in MP.** To protect himself from impersonators player can set a password in launcher. Password is passed to the PA game as a cmd argument. Client sends password to the server in plain form. If player is known, salt is taken from DCON and hashes are compared to check whether to drop connection.  If player is new, hash and salt are stored in DCON. If player previously didn't have a password and sets one in Launcher - it'll update password. In persistent mode, hash and salt are stored in playerlist csv file.

Other bits and pieces:
  - New tooltips for private investment pool under current treasury in the topbar
  - Expanded Web API
  - JSON-based host settings
  - Right click removes diplo messages
  - Improved AI crisis peace offer
  - Support for multiple factories producing same good (unlocking same good)
  - AI dynamically choosing best inf/cav/art unit type and upgrading into it
  - Updated Russian localisation
  - No duplicate WGs for crisis sways
  - State Transfer can be used to gift states to AI and subjects
  - Government changing rebels don't rise in colonial states

Bugfixes:
  - Fix to nickname change in launcher
  - Fix network save loading

## by MeizhouWan

  - Updated Chinese localisation

## by Nivaturimika

  - Regiments & ships cannot reorganize to higher values than effective military spending (military spending slider * goods availability)
  - Fix to texture rendering
