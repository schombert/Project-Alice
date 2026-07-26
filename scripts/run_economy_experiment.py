#!/usr/bin/env python3
"""Run a baseline and candidate Project Alice simulation, then test a hypothesis."""

from __future__ import annotations

import argparse
import json
import math
import pathlib
import subprocess
import sys
from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class metrics:
    population: float
    gdp: float
    life_needs: float
    everyday_needs: float
    unemployment: float
    factory_profit: float
    unprofitable_factory_share: float
    debt: float
    inflation: float


def finite(snapshot: dict[str, Any], *path: str) -> float:
    value: Any = snapshot
    for key in path:
        value = value[key]
    result = float(value)
    if not math.isfinite(result):
        raise ValueError(f"non-finite {'.'.join(path)}")
    return result


def final_metrics(report_path: pathlib.Path) -> metrics:
    final_snapshot: dict[str, Any] | None = None
    with report_path.open(encoding="utf-8") as report:
        for line_number, line in enumerate(report, start=1):
            if not line.strip():
                continue
            try:
                snapshot = json.loads(line)
                if not snapshot["valid"]:
                    raise ValueError("invariant violation")
                final_snapshot = snapshot
            except (json.JSONDecodeError, KeyError, TypeError, ValueError) as error:
                raise ValueError(f"{report_path}:{line_number}: invalid snapshot: {error}") from error
    if final_snapshot is None:
        raise ValueError(f"{report_path}: simulation emitted no snapshots")

    population = finite(final_snapshot, "economy", "population")
    factories = finite(final_snapshot, "counts", "factories")
    return metrics(
        population=population,
        gdp=finite(final_snapshot, "economy", "market_gdp"),
        life_needs=finite(final_snapshot, "living_standards", "life_needs_population_sum") / population if population else 0.0,
        everyday_needs=finite(final_snapshot, "living_standards", "everyday_needs_population_sum") / population if population else 0.0,
        unemployment=finite(final_snapshot, "living_standards", "unemployed_population") / population if population else 0.0,
        factory_profit=finite(final_snapshot, "economy", "factory_profit"),
        unprofitable_factory_share=(
            finite(final_snapshot, "counts", "unprofitable_factories") / factories if factories else 0.0
        ),
        debt=finite(final_snapshot, "finance", "government_debt"),
        inflation=finite(final_snapshot, "economy", "inflation"),
    )


def run_simulation(binary: pathlib.Path, scenario: str, days: int, cadence: int, seed: int,
                   report_path: pathlib.Path, common_args: list[str], variant_args: list[str]) -> None:
    command = [
        str(binary), scenario,
        "--days", str(days),
        "--snapshot-every", str(cadence),
        "--seed", str(seed),
        "--report-jsonl", str(report_path),
        *common_args,
        *variant_args,
    ]
    print("+", " ".join(command), file=sys.stderr)
    completed = subprocess.run(command, check=False)
    if completed.returncode:
        raise RuntimeError(f"simulation failed with exit code {completed.returncode}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare a real Project Alice baseline against a candidate economic ruleset."
    )
    parser.add_argument("binary", type=pathlib.Path, help="Alice executable with the headless runner")
    parser.add_argument("scenario", help="scenario .bin argument accepted by Alice")
    parser.add_argument("--days", type=int, default=3650, help="simulation length (default: 10 years)")
    parser.add_argument("--snapshot-every", type=int, default=30, help="JSONL cadence in days")
    parser.add_argument("--seed", type=int, default=424242)
    parser.add_argument("--output-dir", type=pathlib.Path, default=pathlib.Path("out/economy-experiment"))
    parser.add_argument("--common-arg", action="append", default=[], help="argument passed to both runs")
    parser.add_argument("--baseline-arg", action="append", default=[], help="argument passed only to baseline")
    parser.add_argument("--candidate-arg", action="append", default=[], help="argument passed only to candidate")
    parser.add_argument("--min-gdp-delta", type=float, default=-math.inf)
    parser.add_argument("--min-gdp-relative-change", type=float, default=-math.inf,
                        help="minimum candidate GDP change relative to baseline, e.g. -0.01 for -1%%")
    parser.add_argument("--min-life-needs-delta", type=float, default=-math.inf)
    parser.add_argument("--max-unemployment-delta", type=float, default=math.inf)
    parser.add_argument("--max-unprofitable-factory-share-delta", type=float, default=math.inf)
    parser.add_argument("--max-debt-delta", type=float, default=math.inf)
    parser.add_argument("--max-inflation-delta", type=float, default=math.inf)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.days < 0 or args.snapshot_every < 1 or not 0 <= args.seed <= 2**32 - 1:
        print("--days must be non-negative, --snapshot-every positive, and --seed a uint32.", file=sys.stderr)
        return 2
    if not args.binary.is_file():
        print(f"Executable not found: {args.binary}", file=sys.stderr)
        return 2

    args.output_dir.mkdir(parents=True, exist_ok=True)
    baseline_report = args.output_dir / "baseline.jsonl"
    candidate_report = args.output_dir / "candidate.jsonl"
    try:
        run_simulation(args.binary, args.scenario, args.days, args.snapshot_every, args.seed,
                       baseline_report, args.common_arg, args.baseline_arg)
        run_simulation(args.binary, args.scenario, args.days, args.snapshot_every, args.seed,
                       candidate_report, args.common_arg, args.candidate_arg)
        baseline = final_metrics(baseline_report)
        candidate = final_metrics(candidate_report)
    except (OSError, KeyError, RuntimeError, ValueError) as error:
        print(error, file=sys.stderr)
        return 2

    deltas = {field: getattr(candidate, field) - getattr(baseline, field) for field in metrics.__annotations__}
    if baseline.gdp == 0.0:
        print("baseline GDP is zero; relative GDP hypothesis cannot be evaluated", file=sys.stderr)
        return 2
    gdp_relative_change = deltas["gdp"] / baseline.gdp
    print("metric,baseline,candidate,delta")
    for field in metrics.__annotations__:
        print(f"{field},{getattr(baseline, field):.8g},{getattr(candidate, field):.8g},{deltas[field]:+.8g}")
    print(f"gdp_relative_change,{baseline.gdp:.8g},{candidate.gdp:.8g},{gdp_relative_change:+.8g}")

    checks = (
        ("gdp", deltas["gdp"], ">=", args.min_gdp_delta),
        ("gdp_relative_change", gdp_relative_change, ">=", args.min_gdp_relative_change),
        ("life_needs", deltas["life_needs"], ">=", args.min_life_needs_delta),
        ("unemployment", deltas["unemployment"], "<=", args.max_unemployment_delta),
        ("unprofitable_factory_share", deltas["unprofitable_factory_share"], "<=",
         args.max_unprofitable_factory_share_delta),
        ("debt", deltas["debt"], "<=", args.max_debt_delta),
        ("inflation", deltas["inflation"], "<=", args.max_inflation_delta),
    )
    failures = [f"{name} delta {actual:+.6g} must be {operator} {limit:+.6g}"
                for name, actual, operator, limit in checks
                if (actual < limit if operator == ">=" else actual > limit)]
    if failures:
        print("Hypothesis rejected:", file=sys.stderr)
        print("\n".join(f"- {failure}" for failure in failures), file=sys.stderr)
        return 1
    print("Hypothesis accepted.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
