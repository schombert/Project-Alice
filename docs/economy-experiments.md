# Economy experiments

Use the real headless game simulation to test an economic hypothesis. Every
experiment runs an unchanged baseline and a candidate from the same scenario and
seed. It writes JSONL reports for diagnosis and prints the final comparison as
CSV. A non-zero exit means that the simulation failed, emitted an invalid
snapshot, or the candidate missed a declared threshold.

For an initial smoke experiment, both runs may use the same rules. This validates
the runner and establishes a baseline before a gameplay change is added:

```sh
python3 scripts/run_economy_experiment.py \
  build/macos-arm64-debug/AliceIncremental 1.bin \
  --days 365 --snapshot-every 30 --common-arg=--age-of-transformation
```

When an experiment introduces a runtime-selectable candidate switch, state the
hypothesis as tolerances. For example, a factory-growth change may be accepted
only if it does not lower GDP by more than 1%, does not reduce average life-needs
fulfilment by more than 0.5 percentage points, and does not add more than 1 point
of unemployment:

```sh
python3 scripts/run_economy_experiment.py \
  build/macos-arm64-debug/AliceIncremental 1.bin \
  --days 3650 --common-arg=--age-of-transformation \
  --candidate-arg=--factory-growth-cap \
  --min-gdp-relative-change=-0.01 --min-life-needs-delta=-0.005 \
  --max-unemployment-delta=0.01
```

`--candidate-arg` is deliberately generic: a proposed mechanics change must
expose an explicit runtime switch before it can be compared fairly. The harness
does not silently change a scenario, seed, or game rule between the two runs.

The reports live in `out/economy-experiment/` by default. Inspect their time
series with:

```sh
python3 scripts/summarize_simulation_report.py out/economy-experiment/candidate.jsonl
```

Suggested progression:

1. PR: 1-year smoke experiment and unit tests.
2. Nightly: 10-year experiments for each active economic hypothesis.
3. Before release: 1836-to-end baseline, candidate, and save/load determinism
   runs for the flagship ruleset.
