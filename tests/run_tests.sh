#!/usr/bin/env bash
# AESDumpster end-to-end tests (Linux build + mingw/wine Windows build).
set -euo pipefail
cd "$(dirname "$0")/.."

fail() { echo "FAIL: $1" >&2; exit 1; }

# Build fresh binaries.
make clean >/dev/null
make >/dev/null
make windows >/dev/null

# Generate fixtures in a mktemp dir under /tmp (no in-script cleanup; left
# for systemd-tmpfiles to age out).
TMPDIR_FIXTURE=$(mktemp -d)
FIXTURE="$TMPDIR_FIXTURE/fixture.bin"
ZEROS="$TMPDIR_FIXTURE/zeros.bin"
head -c 256 /dev/zero > "$ZEROS"
KEY=$(python3 tests/gen_fixture.py "$FIXTURE")
echo "Expected key: 0x$KEY"

# 1. Positive: fixture yields the expected key.
out=$(./aesdumpster "$FIXTURE" </dev/null)
echo "$out" | grep -q "0x$KEY" || fail "expected key not found in positive test"
echo "PASS: positive"

# 2. Negative: all-zeros file reports no keys.
out=$(./aesdumpster "$ZEROS" </dev/null)
echo "$out" | grep -q "There were no keys to be found" || fail "negative test did not report missing keys"
echo "PASS: negative"

# 3. Multi-file: both files processed in one invocation.
out=$(./aesdumpster "$FIXTURE" "$ZEROS" </dev/null)
echo "$out" | grep -q "0x$KEY" || fail "multi-file test: key missing"
echo "$out" | grep -q "There were no keys to be found" || fail "multi-file test: zeros file not processed"
echo "PASS: multi-file"

# 4. No args: prints usage instructions.
out=$(./aesdumpster </dev/null)
echo "$out" | grep -q "Usage:" || fail "no-args test: usage not printed"
echo "PASS: no-args"

# 5. Windows binary under wine finds the same key (Z: maps to / in wine).
out=$(wine aesdumpster.exe "Z:$FIXTURE" </dev/null 2>/dev/null)
echo "$out" | grep -q "0x$KEY" || fail "wine test: key not found"
echo "PASS: windows-under-wine"

# 6. --help prints usage.
out=$(./aesdumpster --help </dev/null)
echo "$out" | grep -q "Usage: aesdumpster" || fail "--help test: usage not printed"
echo "PASS: help-flag"

# 7. --version prints name and a version string.
out=$(./aesdumpster --version </dev/null)
echo "$out" | grep -qE "^AESDumpster .+" || fail "--version test: version not printed"
echo "PASS: version-flag"

echo "All tests passed."