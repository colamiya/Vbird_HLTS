#!/usr/bin/env python3
"""Validate tracked media inventory and, optionally, public release evidence."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_PATH = ROOT / "ASSET_MANIFEST.csv"
CONFIG_PATH = ROOT / "release" / "compliance.json"
SOURCE_ROOT = ROOT / "source"
REQUIRED_COLUMNS = {
    "path",
    "sha256",
    "bytes",
    "media_type",
    "provenance_status",
    "creator_or_source",
    "license_or_permission",
    "modifications",
    "required_credit",
}


def fail(message: str) -> None:
    raise ValueError(message)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_manifest() -> list[dict[str, str]]:
    with MANIFEST_PATH.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        if set(reader.fieldnames or ()) != REQUIRED_COLUMNS:
            fail("ASSET_MANIFEST.csv columns do not match the required schema")
        rows = list(reader)
    if not rows:
        fail("ASSET_MANIFEST.csv is empty")
    return rows


def validate_inventory(rows: list[dict[str, str]]) -> None:
    tracked = {path.relative_to(ROOT).as_posix() for path in SOURCE_ROOT.rglob("*") if path.is_file()}
    declared: set[str] = set()

    for row in rows:
        relative = row["path"]
        if relative in declared:
            fail(f"duplicate asset row: {relative}")
        declared.add(relative)
        path = (ROOT / relative).resolve()
        if ROOT.resolve() not in path.parents or not path.is_file():
            fail(f"missing or unsafe asset path: {relative}")
        if str(path.stat().st_size) != row["bytes"]:
            fail(f"size mismatch: {relative}")
        if sha256(path) != row["sha256"]:
            fail(f"SHA-256 mismatch: {relative}")
        if row["media_type"].lower() != path.suffix.removeprefix(".").lower():
            fail(f"media type mismatch: {relative}")
        if row["provenance_status"] not in {"unverified", "verified"}:
            fail(f"invalid provenance status: {relative}")

    missing = sorted(tracked - declared)
    stale = sorted(declared - tracked)
    if missing:
        fail(f"assets missing from manifest: {', '.join(missing)}")
    if stale:
        fail(f"manifest rows without files: {', '.join(stale)}")


def validate_release(rows: list[dict[str, str]], runtime_manifest: Path | None) -> None:
    config = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    unverified = [row["path"] for row in rows if row["provenance_status"] != "verified"]
    incomplete = [
        row["path"]
        for row in rows
        if row["provenance_status"] == "verified"
        and (not row["creator_or_source"].strip() or not row["license_or_permission"].strip())
    ]
    if unverified:
        fail(f"public release blocked by {len(unverified)} unverified assets")
    if incomplete:
        fail(f"verified assets lack provenance evidence: {', '.join(incomplete)}")
    if config.get("public_binary_release_approved") is not True:
        fail("public_binary_release_approved is not true")
    if not config.get("reviewed_at") or not config.get("reviewed_by"):
        fail("release approval identity and date are missing")

    qt = config.get("qt", {})
    if not qt.get("version") or qt.get("linkage") not in {"dynamic", "commercial", "relinkable-static"}:
        fail("Qt version or acceptable linkage evidence is missing")
    if qt.get("linkage") == "dynamic" and not qt.get("source_offer_url"):
        fail("Qt corresponding-source URL is missing")
    if not qt.get("plugins"):
        fail("Qt plugin inventory is empty")

    multimedia = config.get("multimedia", {})
    if not multimedia.get("backend") or not multimedia.get("notices"):
        fail("Multimedia backend or notice inventory is missing")

    for license_path in ("LICENSES/LGPL-3.0-only.txt", "LICENSES/GPL-3.0-only.txt"):
        if not (ROOT / license_path).is_file():
            fail(f"required license text is missing: {license_path}")
    if runtime_manifest is None or not runtime_manifest.is_file() or runtime_manifest.stat().st_size == 0:
        fail("non-empty runtime manifest is required")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--release", action="store_true", help="enforce public binary release evidence")
    parser.add_argument("--runtime-manifest", type=Path)
    args = parser.parse_args()
    try:
        rows = load_manifest()
        validate_inventory(rows)
        if args.release:
            validate_release(rows, args.runtime_manifest)
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(f"release-compliance: FAIL: {exc}", file=sys.stderr)
        return 1
    print(f"release-compliance: PASS: {len(rows)} assets inventoried" + ("; release approved" if args.release else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
