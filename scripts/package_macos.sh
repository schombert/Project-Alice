#!/bin/zsh
set -euo pipefail

project_root="${0:A:h:h}"
binary="${1:-$project_root/build/macos-arm64-release/Alice}"
dist_dir="$project_root/dist"
staging_dir="$dist_dir/macos-dmg"
app_name="Project Alice - Age of Transformation.app"
app_dir="$staging_dir/$app_name"
contents_dir="$app_dir/Contents"
resources_dir="$contents_dir/Resources"
dmg_path="$dist_dir/Project-Alice-Age-of-Transformation-arm64.dmg"

if [[ ! -x "$binary" ]]; then
	print -u2 "error: Release binary not found: $binary"
	print -u2 "build it with: cmake --build --preset macos-arm64-release"
	exit 66
fi
if [[ "$(file "$binary")" != *"Mach-O 64-bit executable arm64"* ]]; then
	print -u2 "error: expected an arm64 macOS executable: $binary"
	exit 65
fi

rm -rf "$staging_dir"
mkdir -p "$contents_dir/MacOS" "$resources_dir/bin" "$resources_dir/game" "$dist_dir"

ditto "$binary" "$resources_dir/bin/Alice"
ditto "$project_root/assets" "$resources_dir/game/assets"
ditto "$project_root/LICENSE" "$resources_dir/LICENSE"
ditto "$project_root/packaging/macos/Info.plist" "$contents_dir/Info.plist"
ditto "$project_root/packaging/macos/launch_project_alice.sh" "$contents_dir/MacOS/Project Alice"
chmod 755 "$contents_dir/MacOS/Project Alice" "$resources_dir/bin/Alice"

iconset="$staging_dir/Alice.iconset"
mkdir -p "$iconset"
for size in 16 32 128 256 512; do
	sips -z "$size" "$size" "$project_root/src/alice.png" --out "$iconset/icon_${size}x${size}.png" >/dev/null
	double_size=$((size * 2))
	sips -z "$double_size" "$double_size" "$project_root/src/alice.png" --out "$iconset/icon_${size}x${size}@2x.png" >/dev/null
done
iconutil -c icns "$iconset" -o "$resources_dir/Alice.icns"
rm -rf "$iconset"

codesign --force --sign - "$resources_dir/bin/Alice"
codesign --force --deep --sign - "$app_dir"
ln -s /Applications "$staging_dir/Applications"

rm -f "$dmg_path"
hdiutil create -volname "Project Alice - Age of Transformation" \
	-srcfolder "$staging_dir" -ov -format UDZO "$dmg_path"

print -- "$dmg_path"
