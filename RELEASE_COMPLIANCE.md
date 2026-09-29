# Public binary release gate

The source tree can be reviewed and built for development, but a public Windows binary must not be published until every item below is complete. This is a release gate, not a claim that the current repository is ready for redistribution.

## 1. Assets

1. Review every row in `ASSET_MANIFEST.csv`.
2. Retain evidence for the creator/source and redistribution permission.
3. Fill `creator_or_source`, `license_or_permission`, `modifications` and `required_credit`.
4. Change `provenance_status` to `verified` only after the evidence exists.
5. Remove or replace any file that cannot be verified.

## 2. Qt and Multimedia payload

1. Build with a recorded Qt version and dynamically link Qt unless a commercial license or relinkable static-link arrangement is being used.
2. Run the deployment tool for the exact build, then inventory every shipped DLL, plugin and codec backend in `dist/qt-runtime-manifest.txt`.
3. Record the Qt version, linkage, plugins, corresponding-source URL and Multimedia backend in `release/compliance.json`.
4. Ship Qt copyright notices plus `LICENSES/LGPL-3.0-only.txt` and `LICENSES/GPL-3.0-only.txt` with the package.
5. Preserve all notices required by the actual FFmpeg or platform-codec payload. Check patent obligations for the formats and territories being distributed.
6. Do not impose terms that prevent reverse engineering for debugging modifications to LGPL libraries.

## 3. Approval and verification

Set `public_binary_release_approved` to `true` only after a human review of the exact release payload. Then run:

```text
python tools/check_release_compliance.py --release --runtime-manifest dist/qt-runtime-manifest.txt
```

The command verifies manifest coverage and hashes, rejects unverified assets, requires the release metadata, and checks that the runtime inventory exists. The GitHub workflow runs the inventory check on normal changes and the strict gate on version tags.
