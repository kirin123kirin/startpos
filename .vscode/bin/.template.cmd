@echo off

@rem Pattern1. Simply Wrapper
bash %~dp0\%~n0.sh %*

@rem Pattern2. set variable of return value
@REM for /f "tokens=*" %%a in ("bash %~dp0\%~n0.sh %* ^|  tr -d '\r' ^| tr '\n' ' '") do set %1=%%a
