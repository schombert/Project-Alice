#!/usr/bin/env python3
"""Print balance-relevant checkpoints from a Project Alice JSONL simulation report."""

from __future__ import annotations

import argparse
import json
import math
import pathlib
import sys
from typing import Any


def number(snapshot: dict[str, Any], *path: str) -> float:
    value: Any = snapshot
    for key in path:
        value = value[key]
    result = float(value)
    if not math.isfinite(result):
        raise ValueError(f"non-finite {'.'.join(path)}")
    return result


def load_report(path: pathlib.Path) -> list[dict[str, Any]]:
    snapshots: list[dict[str, Any]] = []
    with path.open(encoding="utf-8") as report:
        for line_number, line in enumerate(report, start=1):
            if not line.strip():
                continue
            try:
                snapshot = json.loads(line)
                if not snapshot["valid"]:
                    raise ValueError("reported invariant violation")
                snapshots.append(snapshot)
            except (json.JSONDecodeError, KeyError, TypeError, ValueError) as error:
                raise ValueError(f"{path}:{line_number}: invalid snapshot: {error}") from error
    if not snapshots:
        raise ValueError(f"{path}: no snapshots")
    return snapshots


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Summarize economy, living-standard and factory telemetry from a JSONL simulation report."
    )
    parser.add_argument("report", type=pathlib.Path)
    parser.add_argument(
        "--every", type=int, default=12,
        help="print every Nth snapshot after the initial row (default: 12)",
    )
    args = parser.parse_args()
    if args.every < 1:
        parser.error("--every must be at least one")

    try:
        snapshots = load_report(args.report)
        print("date_raw,population,gdp,unemployment,life_needs,everyday_needs,luxury_needs,"
              "factory_profit,unprofitable_factory_share,debt,treasury,inflation")
        last_index = len(snapshots) - 1
        for index, snapshot in enumerate(snapshots):
            if index not in (0, last_index) and index % args.every:
                continue
            population = number(snapshot, "economy", "population")
            factories = number(snapshot, "counts", "factories")
            row = (
                int(number(snapshot, "date_raw")),
                population,
                number(snapshot, "economy", "market_gdp"),
                number(snapshot, "living_standards", "unemployed_population") / population if population else 0.0,
                number(snapshot, "living_standards", "life_needs_population_sum") / population if population else 0.0,
                number(snapshot, "living_standards", "everyday_needs_population_sum") / population if population else 0.0,
                number(snapshot, "living_standards", "luxury_needs_population_sum") / population if population else 0.0,
                number(snapshot, "economy", "factory_profit"),
                number(snapshot, "counts", "unprofitable_factories") / factories if factories else 0.0,
                number(snapshot, "finance", "government_debt"),
                number(snapshot, "finance", "treasury"),
                number(snapshot, "economy", "inflation"),
            )
            print(",".join(
                str(row[0]) if column == 0 else f"{value:.6g}"
                for column, value in enumerate(row)
            ))
    except (OSError, KeyError, ValueError) as error:
        print(error, file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
