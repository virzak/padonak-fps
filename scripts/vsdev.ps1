# Runs a command inside the Visual Studio developer environment, which puts
# cmake, the compiler and Visual Studio's bundled vcpkg (VCPKG_ROOT) on hand.
#
#   powershell -File scripts/vsdev.ps1 cmake --preset windows

$ErrorActionPreference = 'Stop'

$installer = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer'
$vs = & (Join-Path $installer 'vswhere.exe') -latest -prerelease -products * `
  -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) {
  throw 'Visual Studio with the "Desktop development with C++" workload was not found.'
}

# VsDevCmd looks for vswhere on PATH
$env:PATH = "$installer;$env:PATH"
Import-Module (Join-Path $vs 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -no_logo' | Out-Null

$command, $rest = $args
& $command @rest
exit $LASTEXITCODE
