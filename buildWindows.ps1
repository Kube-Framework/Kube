function Show-Help {
    Write-Host "USAGE: .\buildWindows.ps1 [arg0, ..., argN]
    release:        Build in release mode (default)
    debug:          Build in debug mode
    tests:          Enable unit testing
    benchmarks:     Enable benchmarking
    clean:          Delete all build directories
    fclean:         Alias of the 'clean' command
    core:           Mark 'Core' module for manual build
    io:             Mark 'IO' module for manual build
    audio:          Mark 'Audio' module for manual build
    gpu:            Mark 'GPU' module for manual build
    flow:           Mark 'Flow' module for manual build
    ecs:            Mark 'ECS' module for manual build
    ui:             Mark 'UI' module for manual build
    "
    Exit
}

function Clean-Build {
    if (Test-Path $buildDir) {
        Remove-Item $buildDir -Recurse -Force
    }
    Exit
}

$buildType = "Release"
$buildTests = $false
$buildBenchmarks = $false
$cmakeArgs = ""
$buildDir = "Build"
$generator = "Ninja Multi-Config"

switch ($args) {
    'release'       { $buildType = "Release" }
    'debug'         { $buildType = "Debug" }
    'tests'         { $buildTests = $true }
    'benchmarks'    { $buildBenchmarks = $true }
    'clean'         { Clean-Build }
    'fclean'        { Clean-Build }
    "--help"        { Show-Help }
    "core"          { $cmakeArgs += " -DKF_CORE=ON" }
    "io"            { $cmakeArgs += " -DKF_IO=ON" }
    "audio"         { $cmakeArgs += " -DKF_AUDIO=ON" }
    "gpu"           { $cmakeArgs += " -DKF_GPU=ON" }
    "flow"          { $cmakeArgs += " -DKF_FLOW=ON" }
    "ecs"           { $cmakeArgs += " -DKF_ECS=ON" }
    "ui"            { $cmakeArgs += " -DKF_UI=ON" }
    default         {
        Write-Host "Invalid argument, please use --help for usage"
        Exit
    }
}

$messagePrefix = "[$buildType"
if ($buildTests) {
    $messagePrefix = "$messagePrefix, Tests"
    $cmakeArgs += " -DKF_TESTS=ON"
}
if ($buildBenchmarks) {
    $messagePrefix = "$messagePrefix, Benchmarks"
    $cmakeArgs += " -DKF_BENCHMARKS=ON"
}
$messagePrefix = "$messagePrefix]"

if (-not (Test-Path $buildDir -PathType Container)) {
    $dirCommand = "cmake -E make_directory $buildDir"
    Write-Host "$messagePrefix Creating build directory: $dirCommand"
    Invoke-Expression $dirCommand
    if ($LASTEXITCODE -eq 0) {
        Write-Host "$messagePrefix Creating build directory - Success"
    } else {
        Write-Error "$messagePrefix Creating build directory - Failure"
        Exit -1
    }
}

$cmakeCommand = "cmake -G""$generator"" -B $buildDir $cmakeArgs ."
Write-Host "$messagePrefix Executing CMake: $cmakeCommand"
Invoke-Expression $cmakeCommand
if ($LASTEXITCODE -eq 0) {
    Write-Host "$messagePrefix Executing CMake - Success"
} else {
    Write-Error "$messagePrefix Executing CMake - Failure"
    Exit -1
}

$buildCommand = "cmake --build $buildDir --config $buildType --parallel"
Write-Host "$messagePrefix Building: $buildCommand"
Invoke-Expression $buildCommand
if ($LASTEXITCODE -eq 0) {
    Write-Host "$messagePrefix Building - Success"
} else {
    Write-Error "$messagePrefix Building - Failure"
    Exit -1
}