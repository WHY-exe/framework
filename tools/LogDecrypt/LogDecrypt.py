#!/usr/bin/env python3
"""Decrypt logs written by FileSinkEx and RotationFileSinkEx.

The sinks use AES-256-CBC with PKCS#7 padding. The IV token is embedded in the
filename as `base.<ivhex>[.<index>].ext`, and rotating files keep the same IV
token. The writer feeds the token's first 16 ASCII bytes into OpenSSL, so this
script does the same. It decrypts the active file plus any rotated siblings in
numeric order.
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.padding import PKCS7


IV_HEX_RE = re.compile(r"^(?P<prefix>.+)\.(?P<iv>[0-9A-Fa-f]{32})(?:\.(?P<index>\d+))?\.(?P<ext>[^.]+)$")


@dataclass(frozen=True, slots=True)
class LogPart:
    path: Path
    index: int
    iv_hex: str


def hex_bytes(value: str) -> bytes:
    if len(value) % 2:
        raise ValueError("hex value must have an even number of characters")
    return bytes.fromhex(value)


def normalize_key(raw_key: str, key_hex: str | None) -> bytes:
    if key_hex:
        key = hex_bytes(key_hex)
        if len(key) != 32:
            raise ValueError("--key-hex must decode to exactly 32 bytes")
        return key

    key = raw_key.encode("utf-8")
    if len(key) >= 32:
        return key[:32]
    return key.ljust(32, b"\0")


def parse_part(path: Path) -> tuple[str, str, str]:
    match = IV_HEX_RE.match(path.name)
    if not match:
        raise ValueError(f"unsupported filename format: {path.name}")
    return match.group("prefix"), match.group("iv"), match.group("ext")


def discover_parts(path: Path) -> list[LogPart]:
    prefix, iv_hex, ext = parse_part(path)
    pattern = re.compile(
        rf"^{re.escape(prefix)}\.{iv_hex}(?:\.(?P<index>\d+))?\.{re.escape(ext)}$"
    )

    parts: list[LogPart] = []
    for candidate in sorted(path.parent.iterdir()):
        if not candidate.is_file():
            continue
        match = pattern.match(candidate.name)
        if not match:
            continue
        index = int(match.group("index") or 0)
        parts.append(LogPart(candidate, index, iv_hex))

    parts.sort(key=lambda item: item.index)
    if not parts:
        raise FileNotFoundError("no matching encrypted log files found")
    return parts


def decrypt_part(path: Path, key: bytes, iv: bytes) -> bytes:
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv))
    decryptor = cipher.decryptor()
    padded_plaintext = decryptor.update(path.read_bytes()) + decryptor.finalize()
    unpadder = PKCS7(algorithms.AES.block_size).unpadder()
    return unpadder.update(padded_plaintext) + unpadder.finalize()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=Path, help="encrypted log file path")
    parser.add_argument("--key", required=True, help="raw encryption key text")
    parser.add_argument(
        "--key-hex",
        help="optional 64-hex-character AES-256 key override",
    )
    parser.add_argument("-o", "--output", type=Path, help="write plaintext to file")
    args = parser.parse_args()

    key = normalize_key(args.key, args.key_hex)

    parts = discover_parts(args.path)
    iv = parts[0].iv_hex[:16].encode("ascii")
    plaintext = b"".join(decrypt_part(part.path, key, iv) for part in parts)

    if args.output:
        args.output.write_bytes(plaintext)
    else:
        sys.stdout.buffer.write(plaintext)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
