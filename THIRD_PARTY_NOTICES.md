# Third-party and distribution notices

Vbird HLTS source code is licensed under the MIT License. Third-party frameworks, plugins, codecs, and tools remain under their own licenses.

## Qt

The source uses Qt 6 Widgets, Multimedia, and MultimediaWidgets. An open-source binary distribution must comply with the license of the exact Qt build and modules being shipped. For an LGPL deployment this normally includes:

- dynamically linking the LGPL Qt libraries, or otherwise providing the relinkable material required by the applicable license;
- shipping the applicable LGPL text, Qt copyright and attribution notices;
- making the corresponding Qt source available by an accepted method;
- allowing reverse engineering for debugging modifications to the LGPL library;
- documenting every bundled Qt plugin and module instead of relying on this source-level list.

Static linking needs additional relinking measures or an appropriate commercial Qt license. See the official [Qt licensing overview](https://doc.qt.io/qt-6/licensing.html) and [Qt LGPL obligations](https://www.qt.io/development/open-source-lgpl-obligations).

## Multimedia codecs

Qt Multimedia can use FFmpeg and platform codecs. A release must preserve the notices for the actual backend and codec libraries it includes. Codec and patent obligations vary by format and distribution territory; the project MIT license does not grant patent rights.

## Assets

See [ASSET_PROVENANCE.md](ASSET_PROVENANCE.md). A release maintainer must complete the inventory for the exact image, audio, video, font, and hotel-brand material included in a binary package.
