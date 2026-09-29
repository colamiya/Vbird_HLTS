# Asset provenance

`ASSET_MANIFEST.csv` is the authoritative inventory for repository media. It currently records 114 files under `source/` (86 JPG, 25 PNG and 3 MP4; 223,688,876 bytes total), including exact paths, sizes and SHA-256 digests.

Every current row is deliberately marked `unverified`. This means the repository does **not** claim that those files are covered by the project MIT License, and they must not be included in a public binary release until the maintainer records:

- creator or original source;
- license, written permission or other redistribution basis;
- relevant modifications;
- required attribution;
- confirmation that the material contains no identifiable students, private locations or unauthorized hotel marks.

After evidence has been retained, set a row to `verified` and fill all provenance fields. Run `python tools/check_release_compliance.py` after any asset change. Release mode rejects every status other than `verified`; see [RELEASE_COMPLIANCE.md](RELEASE_COMPLIANCE.md).
