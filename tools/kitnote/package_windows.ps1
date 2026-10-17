# Packs the installed build (build.install) into dist\KitNote-<version>-win64.zip.
# Run tools\kitnote\build_windows.bat first.
$root = Resolve-Path (Join-Path $PSScriptRoot '..\..')
$install = Join-Path $root 'build.install'
if (-not (Test-Path (Join-Path $install 'bin\KitNote.exe'))) {
    Write-Error 'build.install\bin\KitNote.exe not found. Build first.'
    exit 1
}

$version = foreach ($part in 'MAJOR', 'MINOR', 'PATCH') {
    (Select-String -Path (Join-Path $root 'version.cmake') -Pattern "set\(KITNOTE_APP_VERSION_$part `"(\d+)`"\)").Matches[0].Groups[1].Value
}
$name = 'KitNote-{0}-win64' -f ($version -join '.')

$dist = Join-Path $root 'dist'
$stage = Join-Path $dist $name
if (Test-Path $stage) {
    Remove-Item $stage -Recurse -Force
}
New-Item $stage -ItemType Directory -Force | Out-Null
Copy-Item (Join-Path $install '*') $stage -Recurse
Copy-Item (Join-Path $PSScriptRoot 'install.ps1') $stage
Copy-Item (Join-Path $PSScriptRoot 'register_filetype.ps1') $stage
Copy-Item (Join-Path $root 'LICENSE.txt') $stage
Copy-Item (Join-Path $root 'KITNOTE.md') $stage
Copy-Item (Join-Path $root 'THIRD_PARTY_NOTICES.md') $stage

# install.ps1 copies and removes the package item by item. Something new in the package
# that its list does not name would be left out of an installation, so stop here instead.
$installScript = Get-Content (Join-Path $PSScriptRoot 'install.ps1') -Raw
$unknown = Get-ChildItem $stage | Where-Object { $installScript -notmatch ("'" + [regex]::Escape($_.Name) + "'") }
if ($unknown) {
    Write-Error ("install.ps1 does not list: " + ($unknown.Name -join ', ') + '. Add it to $packageItems.')
    exit 1
}

$zip = Join-Path $dist "$name.zip"
if (Test-Path $zip) {
    Remove-Item $zip -Force
}
# Windows' bundled tar writes a standard zip with forward slashes in entry names.
# Compress-Archive on Windows PowerShell 5.1 writes backslashes, which other unzip tools mishandle.
$tar = Join-Path $env:SystemRoot 'System32\tar.exe'
& $tar -a -cf $zip -C $dist $name
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $zip)) {
    Write-Error "tar could not write $zip"
    exit 1
}
Remove-Item $stage -Recurse -Force
'{0}: {1:N1} MB' -f $zip, ((Get-Item $zip).Length / 1MB)
