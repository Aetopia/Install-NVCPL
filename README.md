# Install-NVCPL
A simple PowerShell script to install the NVIDIA Control Panel as a UWP or Win32 App!

## Usage
Use the following command to run the script:
```ps
irm "https://raw.githubusercontent.com/Aetopia/Install-NVCPL/main/Install-NVCPL.ps1" | iex
```

## Build NVIDIA Control Panel Service Launcher 
1. Install `GCC` and [`UPX`](https://upx.github.io/).
2. Run `build.bat`.
3. Now simply extract a NVIDIA Control Panel APPX package as a `.zip` archive.              
4. Replace `nvcpluir.dll` in `Display.Driver\NVCPL` within extracted `.zip` file directory.
5. Run `nvcplui.exe` as an Administrator to launch the NVIDIA Control Panel.