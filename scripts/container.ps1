# Runs a command in the dev container (.devcontainer/Dockerfile) with the
# repository mounted, building the image first. Works with podman or docker,
# from Windows (WSL-based engine, drawing through WSLg) or Linux (X11).
#
#   scripts/container.ps1 cmake --preset linux
#   scripts/container.ps1 -Port 8000 python3 -m http.server -d artifacts/web 8000
#   scripts/container.ps1 bash

$ErrorActionPreference = 'Stop'

# Parsed by hand so the command's own options (sh -c, ...) pass through.
$Port = 0
$NoGpu = $false
$Command = [System.Collections.Generic.List[string]]::new()
for ($i = 0; $i -lt $args.Count; $i++) {
  if ($Command.Count -eq 0 -and $args[$i] -eq '-Port') { $Port = [int]$args[++$i] }
  elseif ($Command.Count -eq 0 -and $args[$i] -eq '-NoGpu') { $NoGpu = $true }
  else { $Command.Add([string]$args[$i]) }
}
$repo = Split-Path -Parent $PSScriptRoot
$image = 'padonak-fps-dev'
$workdir = '/workspaces/padonak-fps'

$engine = if (Get-Command podman -ErrorAction SilentlyContinue) { 'podman' } else { 'docker' }

& $engine build --quiet -t $image -f "$repo/.devcontainer/Dockerfile" "$repo/.devcontainer" | Out-Null
if ($LASTEXITCODE) { exit $LASTEXITCODE }

$run = @('run', '--rm', '-v', "${repo}:$workdir", '-w', $workdir)
if (-not [Console]::IsInputRedirected) { $run += '-it' }
if ($Port) { $run += @('-p', "127.0.0.1:${Port}:$Port") }

# The engine's kernel says whether it runs in WSL, where the display is WSLg.
$kernel = & $engine info --format '{{.Host.Kernel}}'
if ($kernel -match 'microsoft|WSL') {
  $run += @('-v', '/mnt/wslg/.X11-unix:/tmp/.X11-unix', '-e', 'DISPLAY=:0')
  if (-not $NoGpu) {
    # Mesa's d3d12 driver draws on the Windows GPU through /dev/dxg.
    $run += @('--device', '/dev/dxg', '-v', '/usr/lib/wsl:/usr/lib/wsl:ro',
      '-e', 'LD_LIBRARY_PATH=/usr/lib/wsl/lib', '-e', 'GALLIUM_DRIVER=d3d12')
  }
} elseif ($env:DISPLAY) {
  $run += @('-v', '/tmp/.X11-unix:/tmp/.X11-unix', '-e', "DISPLAY=$env:DISPLAY")
  if (Test-Path /dev/dri) { $run += @('--device', '/dev/dri') }
}

if ($Command.Count -eq 0) { $Command.Add('bash') }
& $engine @run $image @Command
exit $LASTEXITCODE
