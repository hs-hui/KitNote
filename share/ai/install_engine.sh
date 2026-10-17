#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# KitNote: installs the audio engine on a Mac - the same steps as install_engine.ps1 (Windows): a Python of its
# own (by uv), PyTorch (CPU), Demucs and its separation model (htdemucs), and engine 2's beat model Beat This!
# (beats and downbeats, CPJKU, MIT licence) - into a folder of KitNote's, nothing elsewhere. Run again on an
# engine 1, it adds what engine 2 needs. KitNote runs it only after the user has agreed (Preferences > AI).
# Progress goes to stdout as "PROGRESS <percent> <what>" lines; a failure ends with an "ERROR <what>" line.
#
#   sh install_engine.sh --root "$HOME/Library/Application Support/KitNote/ai"
#   ... --remove   takes it all away again
#
# It needs only what comes with macOS (curl, tar, shasum, sysctl) and the internet; it installs nothing
# with Homebrew and does not use the Python of the system.

set -u

ROOT=""
REMOVE=0
while [ $# -gt 0 ]; do
    case "$1" in
        --root)
            [ $# -ge 2 ] || { echo "ERROR --root needs a folder"; exit 1; }
            ROOT="$2"
            shift 2
            ;;
        --remove)
            REMOVE=1
            shift
            ;;
        *)
            echo "ERROR unknown argument: $1"
            exit 1
            ;;
    esac
done

if [ -z "$ROOT" ]; then
    echo "ERROR no --root folder given"
    exit 1
fi
case "$ROOT" in
    /|"$HOME")
        echo "ERROR the engine folder may not be $ROOT"
        exit 1
        ;;
esac

say() {
    echo "PROGRESS $1 $2"
}

# runs a program, passes its output on, and stops when it fails
run() {
    what="$1"
    shift
    if ! "$@" 2>&1; then
        echo "ERROR $what"
        exit 1
    fi
}

if [ "$REMOVE" = 1 ]; then
    rm -rf "$ROOT"
    say 100 "removed"
    exit 0
fi

mkdir -p "$ROOT" || { echo "ERROR the folder $ROOT cannot be made"; exit 1; }
UV_DIR="$ROOT/uv"
UV="$UV_DIR/uv"
VENV="$ROOT/venv"
PYTHON="$VENV/bin/python"

export UV_PYTHON_INSTALL_DIR="$ROOT/python"
export UV_CACHE_DIR="$ROOT/cache"
export TORCH_HOME="$ROOT/torch"

# the processor: Apple silicon (arm64) or Intel. A translated (Rosetta) shell still gets the native build.
if [ "$(sysctl -n hw.optional.arm64 2>/dev/null || echo 0)" = "1" ]; then
    UV_TARGET="aarch64-apple-darwin"
    UV_SHA256="a9c1b29002cf3c83f07fa9cd8a887a3be0107d90e23189721221e7257db8e3d6"
    INTEL=0
else
    UV_TARGET="x86_64-apple-darwin"
    UV_SHA256="5f44dcbde809b632f47c36fadb241cb4d6f9af71d0c8f172b5d2026d3dde742c"
    INTEL=1
fi

# uv of a known release (0.13.0, as on Windows), checked against the SHA-256 published with it
UV_URL="https://github.com/astral-sh/uv/releases/download/0.13.0/uv-$UV_TARGET.tar.gz"

if [ ! -f "$UV" ]; then
    say 3 "downloading uv (the Python installer)"
    TAR="$ROOT/uv.tar.gz"
    if ! curl -fsSL --proto '=https' --tlsv1.2 -o "$TAR" "$UV_URL"; then
        rm -f "$TAR"
        echo "ERROR uv could not be downloaded"
        exit 1
    fi
    if [ "$(shasum -a 256 "$TAR" | cut -d ' ' -f 1)" != "$UV_SHA256" ]; then
        rm -f "$TAR"
        echo "ERROR uv could not be downloaded: the download is not the uv release it should be"
        exit 1
    fi
    rm -rf "$UV_DIR"
    mkdir -p "$UV_DIR"
    if ! tar -xzf "$TAR" -C "$UV_DIR"; then
        rm -f "$TAR"
        echo "ERROR uv could not be unpacked"
        exit 1
    fi
    rm -f "$TAR"
    # the archive keeps uv in a folder of its own (uv-<target>/uv): it is moved to where the script looks for it
    if [ ! -f "$UV" ]; then
        FOUND="$(find "$UV_DIR" -type f -name uv | head -n 1)"
        if [ -n "$FOUND" ]; then
            mv "$FOUND" "$UV"
        fi
    fi
    chmod +x "$UV"
fi

if [ ! -e "$PYTHON" ]; then
    say 10 "installing Python 3.11"
    run "Python could not be installed" "$UV" python install 3.11
    # a Python of uv's own, in this folder: not one the user may uninstall
    # --clear: a folder left by an interrupted install is replaced (uv refuses an existing folder otherwise)
    run "Python could not be set up" "$UV" venv --clear --managed-python --python 3.11 "$VENV"
fi

# Intel Macs: PyTorch has no wheels after 2.2.2 (torch 2.2.2 needs numpy 1.x)
if [ "$INTEL" = 1 ]; then
    TORCH_SPEC="torch==2.2.2"
    TORCHAUDIO_SPEC="torchaudio==2.2.2"
    NUMPY_SPEC="numpy<2"
else
    TORCH_SPEC="torch==2.5.1"
    TORCHAUDIO_SPEC="torchaudio==2.5.1"
    NUMPY_SPEC="numpy<2.3"
fi

# (on a Mac the PyPI wheels of PyTorch are the processor's build - there is no CUDA index to choose)
say 15 "installing PyTorch (about 200 MB)"
run "PyTorch could not be installed" "$UV" pip install --python "$PYTHON" "$TORCH_SPEC" "$TORCHAUDIO_SPEC"

say 45 "installing Demucs and the audio libraries"
run "Demucs could not be installed" "$UV" pip install --python "$PYTHON" "demucs==4.0.1" "soundfile>=0.12" scipy "$NUMPY_SPEC"

say 60 "downloading the separation model (htdemucs, about 80 MB)"
run "The separation model could not be downloaded" "$PYTHON" -c "from demucs.pretrained import get_model; get_model('htdemucs')"

# engine 2: the beat model (KitNote's drum model comes with KitNote, in its Resources folder)
say 75 "installing the beat model"
run "Beat This! could not be installed" "$UV" pip install --python "$PYTHON" "beat-this==1.1.0" "einops==0.8.2" \
    "soxr==1.1.0" "rotary-embedding-torch==0.9.1" tqdm
say 90 "downloading the beat model (Beat This!, about 80 MB)"
run "The beat model could not be downloaded" "$PYTHON" -c "from beat_this.inference import load_model; load_model('final0', 'cpu')"

# written last: without engine.json the engine is half installed
printf '%s\n' '{ "engine": "demucs+beat_this", "version": 2 }' > "$ROOT/engine.json.tmp" && mv "$ROOT/engine.json.tmp" "$ROOT/engine.json"
say 100 "installed"
