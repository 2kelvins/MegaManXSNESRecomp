# Run the ROM-backed CTest fixtures first; this opens a private copy of a save.
param(
    [ValidateSet('setup', 'active')][string]$Scene = 'active',
    [string]$BuildDirectory = (Join-Path (Split-Path $PSScriptRoot -Parent) 'build')
)
$ErrorActionPreference = 'Stop'
$taskBuild = (Resolve-Path -LiteralPath $BuildDirectory).Path
$taskRepo = Split-Path $PSScriptRoot -Parent
$taskSource = Join-Path $taskBuild "magic-carpet-widescreen\magic-carpet-$Scene.sav"
if (-not (Test-Path -LiteralPath $taskSource)) {
    throw 'Build mmx_state_tests with MMX_STATE_TESTS=ON, then run ctest -R mmx_magic_carpet_widescreen first.'
}
$taskSession = Join-Path ([IO.Path]::GetTempPath()) ('mmx-magic-carpet-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $taskSession | Out-Null
$taskSnapshot = Join-Path $taskSession 'resume.sav'
Copy-Item -LiteralPath $taskSource -Destination $taskSnapshot
$taskConfig = Join-Path $taskSession 'config.ini'
$taskConfigText = [IO.File]::ReadAllText((Join-Path $taskBuild 'config.ini'))
$taskConfigText = $taskConfigText -replace '(?m)^Widescreen\s*=.*$', 'Widescreen = 1'
[IO.File]::WriteAllText($taskConfig, $taskConfigText)
# Unattended checks use a dummy device. This is the owner's interactive handoff.
$taskStart = New-Object System.Diagnostics.ProcessStartInfo
$taskStart.FileName = Join-Path $taskBuild 'MegaManXSNESRecomp.exe'
$taskStart.WorkingDirectory = $taskBuild
$taskStart.UseShellExecute = $false
$taskStart.Arguments = '--no-launcher --rom "' + (Join-Path $taskRepo 'mmx.sfc') +
    '" --config "' + $taskConfig + '" --resume-state "' + $taskSnapshot + '"'
if ($taskStart.EnvironmentVariables['SDL_AUDIODRIVER'] -eq 'dummy') {
    $taskStart.EnvironmentVariables.Remove('SDL_AUDIODRIVER')
}
[Diagnostics.Process]::Start($taskStart) | Out-Null
Write-Host "Opened Magic Carpet $Scene with normal audio. Practice files: $taskSession"
