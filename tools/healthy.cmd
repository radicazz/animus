@echo off
REM Healthy - Health check tool for Helipad
REM Usage: tools\healthy.cmd [command] [options]

pushd "%~dp0healthy-cli"
if "%~1"=="" (
    uv run healthy check
) else (
    uv run healthy %*
)
popd
