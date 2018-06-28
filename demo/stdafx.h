// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <event/notifycenter.h>

#include "resource.h"	//APP资源
#define R_IN_CPP
#include "res/resource.h"

#include "../controls.extend/SWkeWebkit.h"
#include "../controls.extend/gif/SGifPlayer.h"
#include "../controls.extend/SIPAddressCtrl.h"
#include "../controls.extend/SImageMaskWnd.h"
#include "../controls.extend/SRatingBar.h"
#include "../controls.extend/SChromeTabCtrl.h"
#include "../controls.extend/smiley/SSmileyCtrl.h"
#include "../controls.extend/SChatEdit.h"
#include "../controls.extend/SFreeMoveWindow.h"
#include "../controls.extend/tipwnd.h"

#include "SInterpolatorView.h"
#include "SPathView.h"

using namespace SOUI;

