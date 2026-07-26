#!/usr/bin/env python3
"""Compare deterministic save-state checksums at dates shared by two JSONL runs."""

from __future__ import annotations

import argparse
import json
import pathlib
import sys


def load_report(path: pathlib.Path) -> dict[int, tuple[int, str]]:
    snapshots: dict[int, tuple[int, str]] = {}
    with path.open("r", encoding="utf-8") as report:
        for line_number, line in enumerate(report, start=1):
            if not line.strip():
                continue
            try:
                snapshot = json.loads(line)
                date_raw = int(snapshot["date_raw"])
                tick = int(snapshot["tick"])
                checksum = str(snapshot["save_checksum"])
            except (json.JSONDecodeError, KeyError, TypeError, ValueError) as error:
                raise ValueError(f"{path}:{line_number}: invalid snapshot: {error}") from error
            if len(checksum) != 64 or any(char not in "0123456789abcdef" for char in checksum):
                raise ValueError(f"{path}:{line_number}: invalid save_checksum")
            snapshots[date_raw] = (tick, checksum)
    if not snapshots:
        raise ValueError(f"{path}: no snapshots")
    return snapshots


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compare save-state checksums on every date shared by two simulation reports."
    )
    parser.add_argument("left", type=pathlib.Path)
    parser.add_argument("right", type=pathlib.Path)
    args = parser.parse_args()

    try:
        left = load_report(args.left)
        right = load_report(args.right)
    except (OSError, ValueError) as error:
        print(error, file=sys.stderr)
        return 2

    shared_dates = sorted(left.keys() & right.keys())
    if not shared_dates:
        print("No shared snapshot dates.", file=sys.stderr)
        return 2

    for date_raw in shared_dates:
        left_tick, left_checksum = left[date_raw]
        right_tick, right_checksum = right[date_raw]
        if left_checksum != right_checksum:
            print(
                f"Mismatch at date_raw={date_raw}: "
                f"left tick={left_tick} {left_checksum}, "
                f"right tick={right_tick} {right_checksum}",
                file=sys.stderr,
            )
            return 1

    print(
        f"Matched {len(shared_dates)} shared snapshot date(s), "
        f"from {shared_dates[0]} through {shared_dates[-1]}."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
