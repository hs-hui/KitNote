#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2026 KitNote contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Imports the Developer ID Application certificate for code signing into a temporary keychain.
# Reads these secrets, passed in as environment variables by the workflow:
#   KITNOTE_MACOS_CERT_P12        the certificate and its private key, exported as .p12 and base64-encoded
#   KITNOTE_MACOS_CERT_PASSWORD   the password of the .p12
# With no certificate the script does nothing, and the package is ad-hoc signed.
# On success it writes KITNOTE_CODESIGN_IDENTITY (and KITNOTE_KEYCHAIN) to $GITHUB_ENV.

echo "Import the signing certificate"
set -euo pipefail

if [ -z "${KITNOTE_MACOS_CERT_P12:-}" ]; then
    echo "KITNOTE_MACOS_CERT_P12 is not set: the app will be ad-hoc signed"
    exit 0
fi
if [ -z "${KITNOTE_MACOS_CERT_PASSWORD:-}" ]; then
    echo "error: KITNOTE_MACOS_CERT_PASSWORD is not set"
    exit 1
fi

TEMP_DIR="${RUNNER_TEMP:-${TMPDIR:-/tmp}}"
P12_FILE="$TEMP_DIR/kitnote_signing.p12"
KEYCHAIN="$TEMP_DIR/kitnote-signing.keychain-db"
KEYCHAIN_PASSWORD="$(openssl rand -base64 24)"

echo "$KITNOTE_MACOS_CERT_P12" | base64 --decode > "$P12_FILE"

security create-keychain -p "$KEYCHAIN_PASSWORD" "$KEYCHAIN"
security set-keychain-settings -lut 21600 "$KEYCHAIN"
security unlock-keychain -p "$KEYCHAIN_PASSWORD" "$KEYCHAIN"
security import "$P12_FILE" -P "$KITNOTE_MACOS_CERT_PASSWORD" -A -t cert -f pkcs12 \
    -T /usr/bin/codesign -T /usr/bin/security -k "$KEYCHAIN"
security set-key-partition-list -S apple-tool:,apple: -s -k "$KEYCHAIN_PASSWORD" "$KEYCHAIN" >/dev/null
# Put the new keychain first in the search list, keeping the others
security list-keychains -d user -s "$KEYCHAIN" $(security list-keychains -d user | xargs)

rm -f "$P12_FILE"

IDENTITY="$(security find-identity -v -p codesigning "$KEYCHAIN" \
    | grep "Developer ID Application" | head -n 1 | sed -E 's/.*"(.*)".*/\1/')"
if [ -z "$IDENTITY" ]; then
    echo "error: no 'Developer ID Application' identity in the certificate"
    exit 1
fi

echo "KITNOTE_CODESIGN_IDENTITY=$IDENTITY" >> "$GITHUB_ENV"
echo "KITNOTE_KEYCHAIN=$KEYCHAIN" >> "$GITHUB_ENV"
echo "Signing identity: $IDENTITY"
