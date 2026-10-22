# Associates .kitnote files with KitNote for the current user (no administrator
# rights needed), for a portable copy that is not installed with install.ps1.
# Run it from the folder that contains bin\KitNote.exe:
#
#   powershell -ExecutionPolicy Bypass -File register_filetype.ps1
#
# Pass -Remove to undo it. -ClassesRoot redirects the registry key that is written,
# so that the script can be tried out without touching the real association.
param(
    [switch]$Remove,
    [string]$ClassesRoot = 'HKCU:\Software\Classes'
)

$ErrorActionPreference = 'Stop'
$progId = 'KitNote.Score'
$extensionKey = Join-Path $ClassesRoot '.kitnote'
$progIdKey = Join-Path $ClassesRoot $progId

if ($Remove) {
    # only what this script wrote: an "Open with" list of other programs, or another program chosen for
    # .kitnote, stays
    if (Test-Path -LiteralPath $extensionKey) {
        if ((Get-ItemProperty -LiteralPath $extensionKey).'(default)' -eq $progId) {
            # (the default value: Remove-ItemProperty cannot name it)
            $writable = (Get-Item -LiteralPath $extensionKey).OpenSubKey('', $true)
            $writable.DeleteValue('', $false)
            $writable.Close()
        }
        $key = Get-Item -LiteralPath $extensionKey
        if (-not $key.Property -and -not $key.SubKeyCount) {
            Remove-Item -LiteralPath $extensionKey -Force
        }
    }
    if (Test-Path -LiteralPath $progIdKey) {
        Remove-Item -LiteralPath $progIdKey -Recurse -Force
    }
    Write-Host 'Removed the .kitnote file association.'
    exit 0
}

$exe = Join-Path $PSScriptRoot 'bin\KitNote.exe'
if (-not (Test-Path -LiteralPath $exe)) {
    Write-Error "KitNote.exe was not found at $exe. Run this script from the KitNote folder."
    exit 1
}

New-Item $extensionKey -Force | Out-Null
Set-ItemProperty $extensionKey -Name '(default)' -Value $progId
New-Item (Join-Path $progIdKey 'DefaultIcon') -Force | Out-Null
New-Item (Join-Path $progIdKey 'shell\open\command') -Force | Out-Null
Set-ItemProperty $progIdKey -Name '(default)' -Value 'KitNote drum score'
# the second icon in the executable is the score-file icon
Set-ItemProperty (Join-Path $progIdKey 'DefaultIcon') -Name '(default)' -Value "`"$exe`",1"
Set-ItemProperty (Join-Path $progIdKey 'shell\open\command') -Name '(default)' -Value "`"$exe`" `"%1`""
Write-Host "Registered .kitnote to open with $exe"
