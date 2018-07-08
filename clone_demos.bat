cd /d %~dp0
@ECHO off
SETLOCAL enabledelayedexpansion
cls
COLOR 1f

cd ..
mkdir soui-demo
cd soui-demo
git clone https://github.com/soui-demo/souitest.git