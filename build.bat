@echo off
cd "%~dp0"
gcc -shared -s -Os nvcpluir.c -lwtsapi32 -o nvcpluir.dll
upx --best --ultra-brute nvcpl.exe>nul 2>&1