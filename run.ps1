cmake -B build
if ($?) {
    Write-Host "-------------------------" -ForegroundColor green
    Write-Host "solution added to ./build" -ForegroundColor green
    Write-Host "-------------------------" -ForegroundColor green
} else {
    Write-Host "---------------------------" -ForegroundColor red
    Write-Host "failed to generate ./build!" -ForegroundColor red
    Write-Host "---------------------------" -ForegroundColor red
    exit
}
$name = Split-Path -Path (Get-Location) -Leaf
$path = "./build/{0}.sln" -f $name
msbuild $path
if ($?) {
    Write-Host "---------------------------------" -ForegroundColor green
    Write-Host "successfully generated executable" -ForegroundColor green
    Write-Host "---------------------------------" -ForegroundColor green
} else {
    Write-Host "------------------------------" -ForegroundColor red
    Write-Host "failed to generate executable!" -ForegroundColor red
    Write-Host "------------------------------" -ForegroundColor red
    exit
}
if (-not (Test-Path -PathType Container "output")) {
  New-Item -ItemType Directory -Force -Path "output"
}
Copy-Item -Path ("build/Debug/{0}.exe" -f $name) -Destination ("output/{0}.exe" -f $name)
Copy-Item -Path "build/Debug/SDL3.dll" -Destination "output/SDL3.dll"
Copy-Item -Path "build/Debug/SDL3_ttf.dll" -Destination "output/SDL3_ttf.dll"
cd output
. ("./{0}.exe" -f $name)
if ($?) {
    Write-Host "-----------------" -ForegroundColor green
    Write-Host "successfully run!" -ForegroundColor green
    Write-Host "-----------------" -ForegroundColor green
} else {
    Write-Host "--------" -ForegroundColor red
    Write-Host "crashed!" -ForegroundColor red
    Write-Host "--------" -ForegroundColor red
}
cd ../