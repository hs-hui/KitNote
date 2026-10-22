# Installs KitNote for the current user. No administrator rights are needed.
# Run it from the unpacked KitNote folder (the one that contains bin\KitNote.exe):
#
#   powershell -ExecutionPolicy Bypass -File install.ps1
#
# It copies the program to %LOCALAPPDATA%\Programs\KitNote, adds a Start menu shortcut,
# opens .kitnote files with KitNote, and adds an entry to "Installed apps".
# Scores and settings are never touched.
#
#   -Uninstall          removes all of that again
#   -InstallDir <path>  installs somewhere else
#   -DesktopShortcut    also puts a shortcut on the desktop
#
# The remaining parameters redirect the places the script writes to. They exist so the
# script can be tried out without changing the real Start menu or registry.
param(
    [switch]$Uninstall,
    [string]$InstallDir = (Join-Path $env:LOCALAPPDATA 'Programs\KitNote'),
    [switch]$DesktopShortcut,
    [string]$StartMenuDir = (Join-Path $env:APPDATA 'Microsoft\Windows\Start Menu\Programs'),
    [string]$DesktopDir = [Environment]::GetFolderPath('Desktop'),
    [string]$ClassesRoot = 'HKCU:\Software\Classes',
    [string]$UninstallRoot = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall'
)

$ErrorActionPreference = 'Stop'
$progId = 'KitNote.Score'
$uninstallKey = Join-Path $UninstallRoot 'KitNote'
$startShortcut = Join-Path $StartMenuDir 'KitNote.lnk'
$desktopShortcutPath = Join-Path $DesktopDir 'KitNote.lnk'

# What a KitNote package consists of. Only these are ever deleted, so anything else that
# ended up in the folder (a score saved next to the program, say) is left alone.
$packageItems = @('ai', 'bin', 'extensions', 'locale', 'manual', 'plugins', 'qml', 'sound', 'styles', 'templates',
                  'wallpapers', 'workspaces', 'install.ps1', 'register_filetype.ps1', 'LICENSE.txt', 'KITNOTE.md', 'THIRD_PARTY_NOTICES.md')

# Full path without a trailing separator, so that two spellings of one folder compare equal.
function Get-FullPath([string]$path) {
    $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($path).TrimEnd('\')
}

function Remove-IfPresent([string]$path) {
    if (Test-Path -LiteralPath $path) {
        Remove-Item -LiteralPath $path -Recurse -Force
    }
}

# KitNote installed with its setup program (KitNote-<version>-win64-setup.exe, Inno Setup): its "Installed apps"
# entry, for the current user or for all users. The two installations would share a folder and an association.
function Test-SetupInstall {
    $setupKey = '{6C1E3B9A-6F4D-4E0B-9A57-3B2D8E1F4C21}_is1'
    $roots = @($UninstallRoot)
    # (all users' entries only for the real registry: a redirected -UninstallRoot is a trial run)
    if ($UninstallRoot -eq 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall') {
        $roots += 'HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall'
    }
    foreach ($root in $roots) {
        if (Test-Path -LiteralPath (Join-Path $root $setupKey)) {
            return $true
        }
    }
    return $false
}

# Takes back the .kitnote association this script made, and nothing else: an "Open with" list of other
# programs, or another program the user chose for .kitnote, stays.
function Remove-KitNoteAssociation {
    $extension = Join-Path $ClassesRoot '.kitnote'
    if (Test-Path -LiteralPath $extension) {
        if ((Get-ItemProperty -LiteralPath $extension).'(default)' -eq $progId) {
            # (the default value: Remove-ItemProperty cannot name it)
            $writable = (Get-Item -LiteralPath $extension).OpenSubKey('', $true)
            $writable.DeleteValue('', $false)
            $writable.Close()
        }
        $key = Get-Item -LiteralPath $extension
        if (-not $key.Property -and -not $key.SubKeyCount) {
            Remove-Item -LiteralPath $extension -Force
        }
    }
    Remove-IfPresent (Join-Path $ClassesRoot $progId)
}

function Test-KitNoteRunsFrom([string]$folder) {
    $prefix = $folder + '\'
    [bool](Get-Process KitNote -ErrorAction SilentlyContinue |
           Where-Object { $_.Path -and $_.Path.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase) })
}

# Deletes the program from a folder that holds a KitNote installation; the folder itself only if nothing else is in it.
function Remove-KitNoteFrom([string]$folder) {
    if (-not (Test-Path -LiteralPath (Join-Path $folder 'bin\KitNote.exe'))) {
        return
    }
    foreach ($item in $packageItems) {
        Remove-IfPresent (Join-Path $folder $item)
    }
    if (-not (Get-ChildItem -LiteralPath $folder -Force | Select-Object -First 1)) {
        Remove-Item -LiteralPath $folder -Force
    }
}

function New-Shortcut([string]$path, [string]$target) {
    $shell = New-Object -ComObject WScript.Shell
    $shortcut = $shell.CreateShortcut($path)
    $shortcut.TargetPath = $target
    $shortcut.WorkingDirectory = Split-Path $target
    $shortcut.Description = 'KitNote drum notation editor'
    $shortcut.Save()
}

$InstallDir = Get-FullPath $InstallDir

if ($Uninstall) {
    # An "Installed apps" entry made by this script records where the program went.
    if (Test-Path $uninstallKey) {
        $recorded = (Get-ItemProperty $uninstallKey).InstallLocation
        if ($recorded) {
            $InstallDir = Get-FullPath $recorded
        }
    }
    if (Test-KitNoteRunsFrom $InstallDir) {
        Write-Error 'KitNote is running. Close it and run the uninstall again.'
        exit 1
    }

    # The program first and the "Installed apps" entry last: if a file is locked and the removal
    # stops half-way, the entry is still there to run the uninstall again.
    Remove-KitNoteFrom $InstallDir
    Remove-IfPresent $startShortcut
    Remove-IfPresent $desktopShortcutPath
    # (an installation made with the setup program has the same association: it stays with it)
    if (-not (Test-SetupInstall)) {
        Remove-KitNoteAssociation
    }
    Remove-IfPresent $uninstallKey

    Write-Host 'KitNote was removed. Your scores and settings were left in place.'
    exit 0
}

$source = Get-FullPath $PSScriptRoot
if (-not (Test-Path -LiteralPath (Join-Path $source 'bin\KitNote.exe'))) {
    Write-Error "bin\KitNote.exe was not found next to this script ($source). Run it from the unpacked KitNote folder."
    exit 1
}

if (Test-SetupInstall) {
    Write-Error ('KitNote is installed with its setup program. Update it by running the new KitNote setup ' +
                 '(KitNote-<version>-win64-setup.exe), or remove it under Settings > Apps > Installed apps first.')
    exit 1
}

$exe = Join-Path $InstallDir 'bin\KitNote.exe'
if ($source -ine $InstallDir) {
    # The unpacked folder inside the install folder (or the other way round) would be deleted or copied into itself.
    $sourceInsideTarget = $source.StartsWith($InstallDir + '\', [StringComparison]::OrdinalIgnoreCase)
    $targetInsideSource = $InstallDir.StartsWith($source + '\', [StringComparison]::OrdinalIgnoreCase)
    if ($sourceInsideTarget -or $targetInsideSource) {
        Write-Error "Unpack KitNote outside of $InstallDir, or choose another folder with -InstallDir."
        exit 1
    }
    if (Test-KitNoteRunsFrom $InstallDir) {
        Write-Error 'The installed KitNote is running. Close it and run the installer again.'
        exit 1
    }
    # Replace an earlier installation, but nothing else that may be in the folder.
    if (Test-Path -LiteralPath $exe) {
        Remove-KitNoteFrom $InstallDir
    } elseif ((Test-Path -LiteralPath $InstallDir) -and (Get-ChildItem -LiteralPath $InstallDir -Force | Select-Object -First 1)) {
        Write-Error "$InstallDir exists and is not a KitNote installation. Choose another folder with -InstallDir."
        exit 1
    }
    New-Item $InstallDir -ItemType Directory -Force | Out-Null
    foreach ($item in $packageItems) {
        $from = Join-Path $source $item
        if (Test-Path -LiteralPath $from) {
            Copy-Item -LiteralPath $from -Destination $InstallDir -Recurse -Force
        }
    }
}

New-Item $StartMenuDir -ItemType Directory -Force | Out-Null
New-Shortcut $startShortcut $exe
if ($DesktopShortcut) {
    New-Shortcut $desktopShortcutPath $exe
}

# .kitnote opens with KitNote. The second icon in the executable is the score-file icon.
New-Item (Join-Path $ClassesRoot '.kitnote') -Force | Out-Null
Set-ItemProperty (Join-Path $ClassesRoot '.kitnote') -Name '(default)' -Value $progId
New-Item (Join-Path $ClassesRoot "$progId\DefaultIcon") -Force | Out-Null
New-Item (Join-Path $ClassesRoot "$progId\shell\open\command") -Force | Out-Null
Set-ItemProperty (Join-Path $ClassesRoot $progId) -Name '(default)' -Value 'KitNote drum score'
Set-ItemProperty (Join-Path $ClassesRoot "$progId\DefaultIcon") -Name '(default)' -Value "`"$exe`",1"
Set-ItemProperty (Join-Path $ClassesRoot "$progId\shell\open\command") -Name '(default)' -Value "`"$exe`" `"%1`""

# "Installed apps" entry, so that KitNote can be removed from Windows Settings.
$version = (Get-Item -LiteralPath $exe).VersionInfo.ProductVersion
$uninstallCommand = "powershell.exe -NoProfile -ExecutionPolicy Bypass -File `"$(Join-Path $InstallDir 'install.ps1')`" -Uninstall"
$sizeKb = [int]((Get-ChildItem -LiteralPath $InstallDir -Recurse -File | Measure-Object Length -Sum).Sum / 1KB)
New-Item $uninstallKey -Force | Out-Null
Set-ItemProperty $uninstallKey -Name 'DisplayName' -Value 'KitNote'
Set-ItemProperty $uninstallKey -Name 'DisplayVersion' -Value $version
Set-ItemProperty $uninstallKey -Name 'Publisher' -Value 'KitNote'
Set-ItemProperty $uninstallKey -Name 'DisplayIcon' -Value $exe
Set-ItemProperty $uninstallKey -Name 'InstallLocation' -Value $InstallDir
Set-ItemProperty $uninstallKey -Name 'UninstallString' -Value $uninstallCommand
Set-ItemProperty $uninstallKey -Name 'EstimatedSize' -Value $sizeKb -Type DWord
Set-ItemProperty $uninstallKey -Name 'NoModify' -Value 1 -Type DWord
Set-ItemProperty $uninstallKey -Name 'NoRepair' -Value 1 -Type DWord

Write-Host "KitNote $version was installed to $InstallDir"
Write-Host 'Start it from the Start menu, or open a .kitnote file.'
