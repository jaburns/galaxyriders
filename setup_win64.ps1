Add-Type -AssemblyName System.IO.Compression.FileSystem

$scriptpath = $MyInvocation.MyCommand.Path
$dir = Split-Path $scriptpath
Push-Location $dir
Push-Location external
[Environment]::CurrentDirectory = $PWD

New-Item "..\src\client\gen" -ItemType Directory -ErrorAction Ignore

Write-Output "Downloading GLEW 2.1.0..."
(New-Object System.Net.WebClient).DownloadFile("https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip", "glew.zip")
Write-Output "Extracting GLEW 2.1.0..."
Remove-Item ".\glew-2.1.0" -Recurse -ErrorAction Ignore
[System.IO.Compression.ZipFile]::ExtractToDirectory("glew.zip", ".")
Remove-Item ".\glew.zip"

Write-Output "Downloading SDL2 2.0.5..."
(New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.5-VC.zip", "sdl.zip")
Write-Output "Extracting SDL2 2.0.5..."
Remove-Item ".\SDL2-2.0.5" -Recurse -ErrorAction Ignore
[System.IO.Compression.ZipFile]::ExtractToDirectory("sdl.zip", ".")
Remove-Item ".\sdl.zip"

Write-Output "Copying DLLs to project root..."
Copy-Item "glew-2.1.0/bin/Release/x64/glew32.dll" ".."
Copy-Item "SDL2-2.0.5/lib/x64/SDL2.dll" ".."

Write-Output "Done!"

Pop-Location
Pop-Location
[Environment]::CurrentDirectory = $PWD