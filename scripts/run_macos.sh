#!/bin/zsh
set -euo pipefail

if [[ $# -lt 1 ]]; then
	print -u2 "usage: $0 /path/to/Victoria2 [Alice arguments...]"
	exit 64
fi

victoria_root="$1"
shift
project_root="${0:A:h:h}"
binary="$project_root/build/macos-arm64-release/Alice"

if [[ ! -d "$victoria_root" ]]; then
  print -u2 "error: Victoria II root not found: $victoria_root"
  exit 66
fi

if [[ ! -x "$binary" ]]; then
  print -u2 "error: native binary not found or not executable: $binary"
  exit 67
fi

export ALICE_ASSET_ROOT="$project_root"
cd "$victoria_root"
exec "$binary" "$@"
