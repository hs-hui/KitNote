# SPDX-License-Identifier: GPL-3.0-only
# KitNote: installs the audio engine - a Python of its own (by uv), PyTorch (CPU), Demucs and its
# separation model (htdemucs), and engine 2's beat model Beat This! (beats and downbeats, CPJKU, MIT licence) -
# into a folder of KitNote's, nothing elsewhere. Run again on an engine 1, it adds what engine 2 needs. KitNote runs it only after
# the user has agreed (Preferences > AI). Progress goes to stdout as "PROGRESS <percent> <what>" lines.
#
#   powershell -ExecutionPolicy Bypass -File install_engine.ps1 -Root "%LOCALAPPDATA%\KitNote\ai"
#   ... -Remove   takes it all away again

param(
    [Parameter(Mandatory = $true)][string]$Root,
    [switch]$Remove
)

# (not "Stop": Windows PowerShell would take every line a program writes to stderr for an error)
$ErrorActionPreference = "Continue"
$ProgressPreference = "SilentlyContinue"
# Windows PowerShell 5.1 on an older .NET does not offer TLS 1.2 by itself
[Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

function Say($percent, $what) { Write-Output "PROGRESS $percent $what" }

# runs a program, passes its output on as text, and stops when it fails
function Run($what, $exe, [string[]]$arguments) {
    & $exe @arguments 2>&1 | ForEach-Object { "$_" }
    if ($LASTEXITCODE -ne 0) { Write-Output "ERROR $what"; exit 1 }
}

if ($Remove) {
    if (Test-Path $Root) { Remove-Item -Recurse -Force $Root }
    Say 100 "removed"
    exit 0
}

New-Item -ItemType Directory -Force $Root | Out-Null
$uvDir = Join-Path $Root "uv"
$uv = Join-Path $uvDir "uv.exe"
$venv = Join-Path $Root "venv"
$python = Join-Path $venv "Scripts\python.exe"

$env:UV_PYTHON_INSTALL_DIR = Join-Path $Root "python"
$env:UV_CACHE_DIR = Join-Path $Root "cache"
$env:TORCH_HOME = Join-Path $Root "torch"

# uv of a known release, checked against the SHA-256 published with it
$uvUrl = "https://github.com/astral-sh/uv/releases/download/0.13.0/uv-x86_64-pc-windows-msvc.zip"
$uvSha256 = "088962f9e7b7bd9ea740c04c650b2a21c8928c345bd99ac24350dc924dba656c"

if (-not (Test-Path $uv)) {
    Say 3 "downloading uv (the Python installer)"
    $zip = Join-Path $Root "uv.zip"
    try {
        Invoke-WebRequest -UseBasicParsing $uvUrl -OutFile $zip -ErrorAction Stop
        if ((Get-FileHash -Algorithm SHA256 $zip -ErrorAction Stop).Hash -ne $uvSha256) {
            Remove-Item $zip
            throw "the download is not the uv release it should be"
        }
        Expand-Archive -Force $zip $uvDir -ErrorAction Stop
    } catch {
        Write-Output "ERROR uv could not be downloaded: $_"
        exit 1
    }
    Remove-Item $zip
    if (-not (Test-Path $uv)) {
        $found = Get-ChildItem -Recurse $uvDir -Filter uv.exe | Select-Object -First 1
        if ($found) { Move-Item $found.FullName $uv -Force }
    }
}

if (-not (Test-Path $python)) {
    Say 10 "installing Python 3.11"
    Run "Python could not be installed" $uv @("python", "install", "3.11")
    # a Python of uv's own, in this folder: not one the user may uninstall
    Run "Python could not be set up" $uv @("venv", "--managed-python", "--python", "3.11", $venv)
}

Say 15 "installing PyTorch (about 200 MB)"
Run "PyTorch could not be installed" $uv @("pip", "install", "--python", $python, "torch==2.5.1", "torchaudio==2.5.1", "--index-url", "https://download.pytorch.org/whl/cpu")

Say 45 "installing Demucs and the audio libraries"
Run "Demucs could not be installed" $uv @("pip", "install", "--python", $python, "demucs==4.0.1", "soundfile>=0.12", "scipy", "numpy<2.3")

Say 60 "downloading the separation model (htdemucs, about 80 MB)"
Run "The separation model could not be downloaded" $python @("-c", "from demucs.pretrained import get_model; get_model('htdemucs')")

# engine 2: the beat model (KitNote's drum model comes with KitNote, in this folder of its program)
Say 75 "installing the beat model"
Run "Beat This! could not be installed" $uv @("pip", "install", "--python", $python, "beat-this==1.1.0", "einops==0.8.2",
    "soxr==1.1.0", "rotary-embedding-torch==0.9.1", "tqdm")
Say 90 "downloading the beat model (Beat This!, about 80 MB)"
Run "The beat model could not be downloaded" $python @("-c", "from beat_this.inference import load_model; load_model('final0', 'cpu')")

Set-Content -Encoding UTF8 (Join-Path $Root "engine.json") '{ "engine": "demucs+beat_this", "version": 2 }'
Say 100 "installed"
