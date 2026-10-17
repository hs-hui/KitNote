# Tries install.ps1 against scratch folders and a scratch registry key:
#
#   powershell -ExecutionPolicy Bypass -File tools\kitnote\test_install.ps1
#
# Needs a built KitNote in build.install. Nothing outside %TEMP%\kitnote-install-test and
# HKCU:\Software\KitNoteInstallTest is touched, and both are removed at the end.
$ErrorActionPreference = 'Stop'
# (TEMP can be a short 8.3 path, e.g. C:\Users\RUNNER~1\... on a build server, while shortcuts and the
# registry hold long paths: the scratch folder is named by its long path so that the comparisons hold)
Add-Type -Namespace KitNoteTest -Name Path -MemberDefinition @'
[DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
public static extern uint GetLongPathName(string shortPath, System.Text.StringBuilder longPath, uint size);
'@
$longTemp = New-Object System.Text.StringBuilder 1024
$tempRoot = if ([KitNoteTest.Path]::GetLongPathName($env:TEMP, $longTemp, 1024) -gt 0) { $longTemp.ToString() } else { $env:TEMP }
$S = Join-Path $tempRoot 'kitnote-install-test'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$stage = Join-Path $S 'itest\stage folder'          # a space in the path on purpose
$target = Join-Path $S 'itest\Programs\KitNote'
$menu = Join-Path $S 'itest\StartMenu'
$desk = Join-Path $S 'itest\Desktop'
$reg = 'HKCU:\Software\KitNoteInstallTest'

if (Test-Path (Join-Path $S 'itest')) { Remove-Item (Join-Path $S 'itest') -Recurse -Force }
if (Test-Path $reg) { Remove-Item $reg -Recurse -Force }
New-Item $stage, $desk -ItemType Directory -Force | Out-Null
New-Item "$reg\Classes", "$reg\Uninstall" -Force | Out-Null

function New-Stage([string]$folder) {
    # a light stand-in for the unpacked zip: the real executable and a few files
    New-Item (Join-Path $folder 'bin'), (Join-Path $folder 'locale') -ItemType Directory -Force | Out-Null
    Copy-Item "$repo\build.install\bin\KitNote.exe" (Join-Path $folder 'bin')
    Copy-Item "$repo\build.install\locale\languages.json" (Join-Path $folder 'locale')
    Copy-Item "$repo\tools\kitnote\install.ps1" $folder
}
New-Stage $stage

$redirect = @('-StartMenuDir', $menu, '-DesktopDir', $desk, '-ClassesRoot', "$reg\Classes", '-UninstallRoot', "$reg\Uninstall")
function Check([string]$name, [bool]$ok) { '{0}  {1}' -f $(if ($ok) { 'ok   ' } else { 'FAIL ' }), $name }
function Invoke-Quiet([string]$script, [string[]]$arguments) {
    # stderr of a refused run must not stop this script
    $quoted = ($arguments | ForEach-Object { if ($_ -match '\s') { "`"$_`"" } else { $_ } }) -join ' '
    & cmd /c "powershell -NoProfile -ExecutionPolicy Bypass -File `"$script`" $quoted 2>&1" | Out-Null
    $LASTEXITCODE
}

$code = Invoke-Quiet (Join-Path $stage 'install.ps1') (@('-InstallDir', $target, '-DesktopShortcut') + $redirect)
Check 'install exits with 0' ($code -eq 0)
Check 'program copied' (Test-Path "$target\bin\KitNote.exe")
Check 'install.ps1 copied (for uninstalling)' (Test-Path "$target\install.ps1")
Check 'Start menu shortcut' (Test-Path "$menu\KitNote.lnk")
Check 'desktop shortcut' (Test-Path "$desk\KitNote.lnk")
$lnk = (New-Object -ComObject WScript.Shell).CreateShortcut("$menu\KitNote.lnk")
Check 'shortcut points at the installed exe' ($lnk.TargetPath -eq "$target\bin\KitNote.exe")
Check '.kitnote -> KitNote.Score' ((Get-ItemProperty "$reg\Classes\.kitnote").'(default)' -eq 'KitNote.Score')
$cmd = (Get-ItemProperty "$reg\Classes\KitNote.Score\shell\open\command").'(default)'
Check 'open command' ($cmd -eq "`"$target\bin\KitNote.exe`" `"%1`"")
$u = Get-ItemProperty "$reg\Uninstall\KitNote"
Check "uninstall entry: $($u.DisplayName) $($u.DisplayVersion), $($u.EstimatedSize) KB" ($u.DisplayName -eq 'KitNote' -and $u.DisplayVersion -and $u.EstimatedSize -gt 0)
Check 'uninstall string names the installed script' ($u.UninstallString -like "*$target\install.ps1*-Uninstall")
Check 'install location is recorded as a full path' ($u.InstallLocation -eq $target)

# installing again over itself; a file the user put there must survive
Set-Content (Join-Path $target 'my score.kitnote') 'x'
$code = Invoke-Quiet (Join-Path $stage 'install.ps1') (@('-InstallDir', $target) + $redirect)
Check 'second install exits with 0' ($code -eq 0 -and (Test-Path "$target\bin\KitNote.exe"))
Check 'a file of the user survives reinstalling' (Test-Path (Join-Path $target 'my score.kitnote'))

# a folder that is not KitNote is refused
$other = Join-Path $S 'itest\Other'
New-Item $other -ItemType Directory -Force | Out-Null
Set-Content (Join-Path $other 'keep.txt') 'x'
$code = Invoke-Quiet (Join-Path $stage 'install.ps1') (@('-InstallDir', $other) + $redirect)
Check 'refuses a non-empty folder that is not KitNote' ($code -ne 0 -and (Test-Path (Join-Path $other 'keep.txt')))

# the unpacked folder inside the install folder is refused, and nothing is deleted
$inner = Join-Path $target 'unpacked'
New-Item $inner -ItemType Directory -Force | Out-Null
New-Stage $inner
$code = Invoke-Quiet (Join-Path $inner 'install.ps1') (@('-InstallDir', $target) + $redirect)
Check 'refuses to install from inside the install folder' ($code -ne 0 -and (Test-Path "$inner\bin\KitNote.exe") -and (Test-Path "$target\bin\KitNote.exe"))
Remove-Item $inner -Recurse -Force

# another spelling of the unpacked folder itself: registers in place, deletes nothing
Push-Location $stage
$code = Invoke-Quiet (Join-Path $stage 'install.ps1') (@('-InstallDir', '.') + $redirect)
Pop-Location
Check 'installing "in place" with a relative path keeps the folder' ($code -eq 0 -and (Test-Path "$stage\bin\KitNote.exe"))
Check 'in-place install records the full path' ((Get-ItemProperty "$reg\Uninstall\KitNote").InstallLocation -eq $stage)

# back to the normal install, then uninstall the way "Installed apps" would (from the installed copy)
$code = Invoke-Quiet (Join-Path $stage 'install.ps1') (@('-InstallDir', $target, '-DesktopShortcut') + $redirect)
$code = Invoke-Quiet "$target\install.ps1" (@('-Uninstall') + $redirect)
Check 'uninstall exits with 0' ($code -eq 0)
Check 'program removed' (-not (Test-Path "$target\bin") -and -not (Test-Path "$target\install.ps1"))
Check 'the file of the user is still there' (Test-Path (Join-Path $target 'my score.kitnote'))
Check 'shortcuts removed' (-not (Test-Path "$menu\KitNote.lnk") -and -not (Test-Path "$desk\KitNote.lnk"))
Check 'registry entries removed' (-not (Test-Path "$reg\Classes\.kitnote") -and -not (Test-Path "$reg\Classes\KitNote.Score") -and -not (Test-Path "$reg\Uninstall\KitNote"))
Check 'the unpacked folder was not touched by the uninstall' (Test-Path "$stage\bin\KitNote.exe")

Remove-Item $reg -Recurse -Force
Remove-Item $S -Recurse -Force
