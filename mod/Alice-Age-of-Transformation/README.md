# Alice: Age of Transformation

This is the flagship opt-in ruleset shipped with Project Alice. It intentionally
contains no `replace_path` entries and can be layered over a Victoria 2 data set
or another content mod.

Select `Alice: Age of Transformation` in the launcher and build a fresh scenario.
The capability marker in `common/alice/age_of_transformation.txt` makes the
matching game rule default to enabled; it can still be disabled in the new-game
rules screen.

The engine-side vertical slice provides:

- real-income-aware migration and multi-tier labor-market parity;
- bounded simulation diagnostics and invariant checks;
- national interest-group aggregates and normalized political power;
- deterministic governing coalitions and explainable legitimacy;
- policy execution derived from existing administration and local control;
- bounded banking health and a capped sovereign credit-risk premium;
- route capacity, congestion-sensitive transport cost and soft expansion limits;
- deterministic crisis-stage, escalation, settlement and war-risk diagnostics;
- compatibility fallback to classic rules whenever the game rule is disabled.

For an existing scenario, a one-year headless smoke run can be started with:

```sh
AliceIncremental 1.bin --days 365 --snapshot-every 30 --seed 424242 \
  --age-of-transformation --report-jsonl alice-aot-year.jsonl
```

Add `--save-at-end <basename>` to create a checkpoint, and resume it with
`--load-save <basename>.bin`. Every JSONL snapshot includes a deterministic
`save_checksum`.

The command-line switch is an unsaved automation override. Normal campaigns
should use a freshly built scenario with this mod selected.

The design and stabilization gates are documented in
`docs/features/age-of-transformation.md` in the Project Alice repository.
