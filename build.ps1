param([string]$WorkingDir = "$PSScriptRoot\.build")
$ErrorActionPreference = "Stop"
$repo = "https://github.com/Patrix9999/union-plugin-template.git"
if (-not (Test-Path $WorkingDir)) {
    git clone --recurse-submodules $repo $WorkingDir
    if ($LASTEXITCODE -ne 0) { throw "Cannot clone Union template" }
} else {
    git -C $WorkingDir submodule update --init --recursive
    if ($LASTEXITCODE -ne 0) { throw "Cannot fetch Union API submodules" }
}
Copy-Item "$PSScriptRoot\src\Plugin.ipp" "$WorkingDir\src\Plugin.ipp" -Force
Copy-Item "$PSScriptRoot\src\AutoInvokerCore.hpp" "$WorkingDir\src\AutoInvokerCore.hpp" -Force
$cmakePath = Join-Path $WorkingDir "CMakeLists.txt"
$cmake = Get-Content -Raw $cmakePath
$cmake = $cmake.Replace('project(UnionPlugin', 'project(Union_AutoInvoker_NB')
$cmake = $cmake.Replace('set(GENERATE_UNION_API_DLL ON)', 'set(GENERATE_UNION_API_DLL OFF)')
$cmake = $cmake.Replace('set(GENERATE_VDF ON)', 'set(GENERATE_VDF OFF)')
Set-Content -Path $cmakePath -Value $cmake -Encoding utf8
cmake -S $WorkingDir -B "$WorkingDir\out\auto-nb-g2a" -G Ninja -DCMAKE_BUILD_TYPE=Release -DGOTHIC_API_G2A=ON -DPROJECT_VERSION=1.0.0.0
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }
cmake --build "$WorkingDir\out\auto-nb-g2a" --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "C++ compilation failed" }
$dll = Get-ChildItem "$WorkingDir\out\auto-nb-g2a" -Recurse -Filter Union_AutoInvoker_NB.dll -File | Select-Object -First 1
if (-not $dll) { throw "Expected Union_AutoInvoker_NB.dll not found" }
Copy-Item $dll.FullName "$PSScriptRoot\Union_AutoInvoker_NB.dll" -Force
Write-Host "DLL created: $PSScriptRoot\Union_AutoInvoker_NB.dll"
