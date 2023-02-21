@echo off
cd "%~dp0"
gcc -mwindows -s -Os nvcpl.c -o nvcpl.exe
upx --best --ultra-brute nvcpl.exe>nul 2>&1