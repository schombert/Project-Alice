# Project Alice v1.2.0 (Dec 2nd 2024) Changelog
Assembled by SneakBug8

## by Schombert

  - Complete rework of macrobuilder window UI

## by Peter

**State-level Trade and local prices**. Now trade is simulated on state-level with states having trade routes between each other.

To see the trade routes a state has, navigate to province tab and select a good on the local market subwindow. With a province selected, the game shows all trade routes impacting that state AND big trade routes in the world.

Trade route placement is impacted by geography (distance) and population of states. Lowering tariffs will not bring you extra trade as an intermediary.

Route "desired volume" shows the actual volume of goods transferred. It's impacted by prices on both sides of the route, tariffs, and distance. Every unit of distance increases delivery price by 0.05£.

State purchases (army, navy, national stockpile) are done in the capital. Construction materials are purchased locally.

Province trade throughput has limited capacity which is increased by building railways and ports.

Nations that are at war are embargoing each other.

Tariffs are taken on goods moving in or out of the country. In case of an international trade route, both countries (exporter and importer) take tariffs.

AI always sets tariffs to 5%. Therefore, if you have more expensive/less productive labour than the AI you are competing with - raise tariffs higher than theirs. This way you protect domestic industry from unfair competition. If you have cheaper/more productive labour than the AI you are competing with - lower tariffs lower than theirs. This way you support exports that will suppress other country's industry. Reasonable amount of tariffs help avoid closure of your factories and massive unemployment.

With 100% tariffs under 100% administrative efficiency you will block all trade going in or out.

As a concequence, new Production screen and new Trade screen.

## by SneakBug8

**National Bank**. Your pops (in 1.20 - upper class, in the future - middle class) save some of their money before luxury spending in the National Bank. On the opposite, if they don't have enough money to support themselves - pops will drain National Bank. There is only one bank per country. It allows state to take money from the bank as loans. Interest on loans is paid back to the bank.

Country is bankrupted only if it cannot pay back its interest. Loans are repaid automatically as the state goes out of deficit.

**Persistent Multiplayer.** In this MP mode, players are automatically returned to their nations on both lobby join and hotjoin and playerlist is saved between sessions on the host side. AI isn't placed on player's country upon disconnection.

This allows for 24/7 servers where players join and leave any time they want with the game progressing (previously known in browser games)

**Hotjoin**. Project Alice allows for hotjoining the game. If the player (with his nickname) was already in the game - he will be placed back on his country. New players are placed on AI-led countries sorted by rank from the top (UK, France, Russia, etc.).

**JSON Web API**. PA can expose Web API enabled with alice_expose_webui define. As of now it allows viewing key stats on trade routes, and some stats on nations and commodities.

Other bits and pieces:
    - Tooltip for planned unit arrival time
    - Fix truce lengths
    - New 4th line of mapmodes. Collapsible mapmodes.
    - War subsidies are replaced with subsidies that can be given at peace.
    - Improved Russian localisation
    - Fix research points given on conquest to uncivs.
    - Disarmament CB properly deletes regiments and factories under construction
    - Units can flee further than 1 province
    - Map arrows showing where units are moving even when not selected
    - Full capitulation (or peace worth of >100 warscore) is accepted automatically
    - `po_make_puppet` wargoal now has ticking warscore to allow achieving 100 warscore with that goal.
