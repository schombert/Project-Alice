# Alice: Age of Transformation

`Age of Transformation` is Project Alice's opt-in political-economy ruleset. Its
core loop is deliberately small and explainable:

```text
production and wages
  -> income, savings and property proxies
  -> interest-group political power
  -> governing coalition and legitimacy
  -> local policy execution
  -> new economic and political outcomes
```

The ruleset is selected with the `Alice: Age of Transformation` game rule. It is
disabled by default. When disabled, interest-group updates and their gameplay
effects are no-ops, and existing Victoria 2 parties, elections, issues, reforms,
events and decisions retain their legacy behavior.

## Compatibility contract

- Existing parties remain the public government identity and the ruling party is
  the coalition anchor.
- Interest groups are aggregate national views, not hard membership stored on
  every POP. Migration, splitting and merging therefore cannot leave dangling
  political membership.
- New derived diagnostics are never serialized. Saved model state must receive an
  explicit version/migration before it is added.
- The master game rule reuses the reserved hardcoded-game-rule slot, preserving
  the size and ordering of the handwritten scenario section.
- Existing tax and construction paths already apply administrative control and
  must not be multiplied by it a second time.

## Stabilization gate

The vertical slice may be expanded only after all of these hold:

1. Migration opportunity uses the same multi-tier wages, employment and target
   culture acceptance as actual POP income.
2. Labor ratios never evaluate a masked divide by zero, and all migration weights
   are finite and bounded.
3. Army logistics performs at most one route search per army and one per depot per
   daily cache build; UI reads do not rebuild simulation state.
4. Old saves without logistics fields receive full initial reserves and normal
   priority through an explicit load migration.
5. A bounded headless runner can emit JSONL snapshots and fail on invariant
   violations.
6. A fixed-input simulation is deterministic across repeated runs and save/load.

## Interest groups and political power

The first release uses stable archetypes derived from existing POP types. Each
group reports population support, political power, wealth, income security,
property, literacy and consciousness. Wealth and property inputs are capped and transformed
through bounded signals before normalization so a single rich POP cannot overflow
the model.

Political power is based on:

```text
population weight
  * group affinity
  * bounded per-capita power from wealth, income security, property,
    literacy and consciousness
```

Land ownership is persistent economic state in the flagship ruleset. Existing
province landowner and capitalist shares initialize old scenarios, then change
only through a capped monthly land market. Its accounting rules are:

1. The price capitalizes a 270-day (nine-month) exponential average of net RGO
   rent, with the existing dividend bank as a conservative initialization floor.
2. Buyers submit cash-backed bids and owners submit separate voluntary or forced
   asks; only their matched minimum becomes turnover.
3. POP bids may use only savings above six months of current life-needs costs.
4. Unmet life needs, unemployment and a lack-of-cash debt proxy create forced
   listings. Pension and unemployment-benefit law act as tenant protection and
   reduce that pressure.
5. Rural land use is reported separately as owner-smallholders, tenants and
   landless laborers; these categories are not treated as interchangeable POPs.
6. Sale proceeds follow the seller class's persisted wealth/ownership proxy, not
   local population, so a numerous class cannot receive another owner's sale.
7. State and foreign holdings participate alongside smallholders, landed elites
   and capitalists. Their RGO dividends go to the domestic treasury or the
   investing nations in proportion to recorded foreign investment.
8. Existing political-economic rules drive the first legal layer: effective rich
   tax becomes a land tax; universal voting enables an estate cap and gradual
   agrarian reform; social insurance protects distressed tenants; state-only
   construction policy nationalizes; private-only construction policy privatizes;
   and the foreign-investment rule admits or winds down foreign ownership.

The five shares remain bounded and sum to one, buyer payments equal seller
receipts, land-tax receipts go to the treasury, and classic games retain the
legacy immediate recalculation. New state, foreign, smoothing and turnover
values are derived runtime fields rather than a silent save-format change.

Coalition selection is deterministic. Equal candidates are ordered by stable group
id. The smallest coalition reaching the configured power threshold governs. The
pure selector can model an incumbent, but the live ruleset derives its coalition
only from serialized society and economy state; unsaved hysteresis cannot make a
reloaded campaign diverge from a continuous run.

Legitimacy is an explainable balance of political-power mandate, popular support,
coalition cohesion, social breadth and majority status, with explicit minority,
representation-gap and fragmentation penalties. Every component and the final
value are bounded to `[0, 100]`.

## State capacity and execution

Policy execution is a read-only breakdown built on the existing administration
system:

- national administrative efficiency;
- normalized provincial `control_ratio`;
- policy funding;
- availability of bureaucratic labor;
- political compliance from legitimacy/coalition support.

Connected gameplay consumers now include:

- crime suppression and the share of a movement it can actually dismantle;
- public education delivery and literacy growth (private education is unaffected);
- pensions and unemployment-benefit delivery, with undelivered funds retained by
  the treasury instead of disappearing;
- the implementation gap that drives reform movements;
- provincial mobilization speed.

Assimilation is not part of this balance pass.

## Cities and human development

The first demographic expansion reuses the existing local housing, city,
education and literacy systems and introduces no serialized fields. In the
flagship ruleset:

- unmet urban housing demand creates an overcrowding pressure proportional to
  urbanization;
- overcrowding adds a bounded monthly growth penalty and militancy adjustment;
- literacy, education access and urbanization create an aggregate demographic
  transition that reduces natural growth without requiring age/sex cohorts;
- internal and colonial migration combine real life-needs opportunity with a
  bounded housing-availability multiplier;
- classic games return before reading the new service inputs and remain an exact
  numerical no-op.

The POP growth tooltip exposes housing access, urbanization, overcrowding,
demographic transition and the local human-development index. Headless JSONL
snapshots add a demographic account with baseline natural growth, starvation,
housing loss, transition reduction, net natural change, population-weighted
development inputs and gross migration flows. These values make the first balance
pass observable without changing save compatibility.

## Reforms, movements and political conflict

Each issue option now has an explainable position for every interest group. POP
issue support is weighted twice: once by population and once by the wealth,
property, literacy and organization model that creates political power. A reform
therefore exposes popular support, political-power support and governing-coalition
support as distinct values.

Under the flagship ruleset, political and social reform eligibility requires a
50% composite mandate: 45% political-power support, 35% coalition support and 20%
popular support. AI governments rank eligible reforms with the same signals,
movement pressure and expected implementation capacity. Legacy upper-house rules
remain unchanged when the flagship rule is disabled.

Movement radicalism adds a bounded, inspectable adjustment from political-power
backing, opposition to the coalition, low legitimacy, member hardship and the
reform-implementation gap. Failed suppression creates additional backlash and
militancy; effective state capacity removes a larger deterministic share of the
movement. Radicalism still enters the existing movement-to-rebel pipeline at the
normal threshold, so this extends rather than replaces Project Alice's rebellion
system.

## Simulation observability

The bounded runner advances the same `single_game_tick()` used by interactive play.
Snapshots include date/tick, a 256-bit checksum of the serialized save state,
population, savings, employment, labor prices and sales, treasuries/debt/banks,
administrative control, logistics reserves, political legitimacy, banking
stress, trade congestion, crisis pressure, demographic accounts and invariant failures. JSONL fields
use a stable order to make output diffable.

Recommended automation:

- pull requests: unit tests and a one-year smoke run;
- nightly: fixed-seed 10–30-year run with invariant failure enabled;
- weekly: 1836-to-end run, save/load at midpoint and paired-state checksum
  comparison.

For bounded regression runs on scenarios created before the new gamerule existed,
the Unix executable accepts `--age-of-transformation`. This is an unsaved runtime
override intended for automation; a normal flagship campaign should be created by
selecting the bundled mod and building its scenario.

Example bounded run:

```sh
AliceIncremental 1.bin --days 365 --snapshot-every 30 --seed 424242 \
  --age-of-transformation --report-jsonl alice-aot-year.jsonl
```

For a save/load determinism gate, compare a continuous run with a checkpointed
continuation. Save names are resolved in Project Alice's normal save directory:

```sh
AliceIncremental 1.bin --days 365 --snapshot-every 5 --seed 424242 \
  --age-of-transformation --report-jsonl continuous.jsonl

AliceIncremental 1.bin --days 180 --snapshot-every 5 --seed 424242 \
  --age-of-transformation --save-at-end aot-midpoint \
  --report-jsonl before-checkpoint.jsonl

AliceIncremental 1.bin --load-save aot-midpoint.bin --days 185 \
  --snapshot-every 5 --age-of-transformation \
  --report-jsonl after-checkpoint.jsonl

python3 scripts/compare_simulation_reports.py continuous.jsonl after-checkpoint.jsonl
```

The comparison is keyed by simulation date, so the resumed run's local tick
counter may start from zero. Any checksum mismatch fails with the first divergent
date.

To turn a report into balance-review checkpoints, use the companion summarizer.
It exits non-zero when a snapshot is invalid and prints population-normalized
need fulfilment and unemployment alongside GDP, factory profitability, debt and
inflation. With monthly snapshots, its default is one row per year:

```sh
python3 scripts/summarize_simulation_report.py alice-aot-year.jsonl
```

For a reproducible baseline-versus-candidate hypothesis test, see
[Economy experiments](../economy-experiments.md).

## First expansion slice

The first deliberately narrow post-vertical-slice systems are now connected:

1. Banking exposes bounded credit health, reserve/debt-service coverage and
   financial stress. Stress adds a capped risk premium to existing government
   interest payments; no new serialized balance-sheet fields are introduced.
2. World trade derives effective route capacity from throughput, land transport
   labor and port services. Congestion increases transport cost and slows positive
   route expansion without imposing a brittle hard cap.
3. Diplomatic crises expose deterministic stages, escalation pressure, settlement
   pressure and war risk from the existing crisis state. This first pass is
   diagnostic and intentionally does not rewrite crisis AI or temperature updates.

All three systems are exact no-ops when the flagship rule is disabled. Separate
deposits/equity, regional credit, convoy loss, global trade institutions and new
crisis templates remain later balance/content work rather than hidden changes to
legacy campaigns.
