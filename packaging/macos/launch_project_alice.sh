#!/bin/zsh
set -euo pipefail

contents_dir="${0:A:h:h}"
resources_dir="$contents_dir/Resources"
alice_binary="$resources_dir/bin/Alice"
alice_assets="$resources_dir/game"
saved_root="$HOME/.local/share/Alice/victoria2_path"

victoria_root="${ALICE_VICTORIA2_ROOT:-}"
if [[ -z "$victoria_root" && -f "$saved_root" ]]; then
	victoria_root="$(<"$saved_root")"
fi

if [[ -z "$victoria_root" ]]; then
	for candidate in \
		"$HOME/Games/Victoria2" \
		"$HOME/Library/Application Support/Steam/steamapps/common/Victoria 2" \
		"$HOME/Library/Application Support/Steam/steamapps/common/Victoria II"; do
		if [[ -d "$candidate/map" && -d "$candidate/common" ]]; then
			victoria_root="$candidate"
			break
		fi
	done
fi

if [[ ! -d "$victoria_root/map" || ! -d "$victoria_root/common" ]]; then
	victoria_root="$(osascript -e 'POSIX path of (choose folder with prompt "Select your Victoria 2 installation folder")')"
	victoria_root="${victoria_root%/}"
fi

if [[ ! -d "$victoria_root/map" || ! -d "$victoria_root/common" ]]; then
	message="The selected folder is not a Victoria 2 installation. It must contain the map and common folders."
	osascript -e "display dialog \"$message\" buttons {\"OK\"} default button \"OK\" with icon stop"
	exit 66
fi

mkdir -p "${saved_root:h}"
print -r -- "$victoria_root" >| "$saved_root"

# Victoria 2 is the base root. The bundled Project Alice assets are overlaid
# through ALICE_ASSET_ROOT and take priority after scenario deserialisation.
export ALICE_ASSET_ROOT="$alice_assets"
cd "$victoria_root"

args=()
has_scenario_selector=false
for argument in "$@"; do
	if [[ "$argument" != -psn_* ]]; then
		args+=("$argument")
		if [[ "$argument" == "-test" || "$argument" == *.bin || "$argument" == "--mod" ]]; then
			has_scenario_selector=true
		fi
	fi
done

if [[ "$has_scenario_selector" == false ]]; then
	scenario_dir="$HOME/.local/share/Alice/scenarios"
	scenarios=("$scenario_dir"/*.bin(N.om))
	if (( ${#scenarios} == 0 )); then
		# Generate the initial scenario without opening a temporary GUI session.
		"$alice_binary" -test --days 0 --age-of-transformation
		scenarios=("$scenario_dir"/*.bin(N.om))
	fi
	if (( ${#scenarios} > 0 )); then
		args=("${scenarios[1]:t}" "${args[@]}")
	fi
fi

exec "$alice_binary" "${args[@]}" --age-of-transformation
