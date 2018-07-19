cd /d %~dp0
@ECHO off
SETLOCAL enabledelayedexpansion
cls
COLOR 1f

cd ..
mkdir soui-demo
cd soui-demo

call :update_repo UiEditor
call :update_repo qqlogin
call :update_repo 360
call :update_repo 360Preview
call :update_repo souispy
call :update_repo PcManager
call :update_repo SoTool
call :update_repo mclistview_demo
call :update_repo souitest
call :update_repo VUI
call :update_repo BesLyric
call :update_repo QQMain
call :update_repo FrogPlay
call :update_repo MultiLangs
call :update_repo TXMgr
call :update_repo SLogViewer
call :update_repo souieditor
call :update_repo DropDemo
call :update_repo ThreeClearGame
call :update_repo baiduwp
call :update_repo ScreenShotTool
call :update_repo SoftMaster
call :update_repo SweepingEdge
call :update_repo iectrl
call :update_repo mfc.demo
call :update_repo CapScreenDemo
call :update_repo SOUI-CEF
call :update_repo nsisdemo

goto :eof

:update_repo
if not exist %1 (
  echo "clone %1"
  git clone https://github.com/soui-demo/%1.git
) else (
  echo "pull %1"
  cd %1
  git pull https://github.com/soui-demo/%1.git
  cd ..
)