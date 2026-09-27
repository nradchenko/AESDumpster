#!/usr/bin/env python3
"""Generate a synthetic UE-like binary fixture for AESDumpster tests.

Lays out key pattern 0 from KeyTools/KeyDumpster.h (the C7 mov-dword
instruction sequence) around a known 32-byte AES key placed at
m_keyDwordOffsets[0], and prints the key as the uppercase hex string the
tool is expected to report.
"""
import hashlib
import random
import sys

# Key pattern 0 and m_keyDwordOffsets[0] from KeyTools/KeyDumpster.h.
PATTERN = ("C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? "
           "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?")
DWORD_OFFSETS = [3, 10, 17, 24, 35, 42, 49, 56]


def main():
    out_path = sys.argv[1]
    key = hashlib.sha256(b"AESDumpster synthetic test key").digest()

    rng = random.Random(0xA35D)
    tokens = PATTERN.split()
    margin = 64
    buf = bytearray(rng.randbytes(len(tokens) + 2 * margin))
    start = margin

    # Place the C7 opcode bytes per the pattern.
    for i, tok in enumerate(tokens):
        if tok == "C7":
            buf[start + i] = 0xC7

    # Place the key dwords at the table offsets (memory order).
    for i, off in enumerate(DWORD_OFFSETS):
        buf[start + off:start + off + 4] = key[i * 4:(i + 1) * 4]

    with open(out_path, "wb") as f:
        f.write(buf)
    print(key.hex().upper())


if __name__ == "__main__":
    main()