add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/components)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)
include_directories(${PROJECT_SOURCE_DIR}/third-part/wke/include)
include_directories(${PROJECT_SOURCE_DIR}/third-part/mhook/mhook-lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRCS  *.cpp)
file(GLOB_RECURSE CURRENT_SOUIS  *.xml *.idx *.ico)
file(GLOB_RECURSE CURRENT_RC *.rc *.rc2)

set(EXTENDCOTROLS ${EXTENDCOTROLS}
	${PROJECT_SOURCE_DIR}/controls.extend/SWkeWebkit.h
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SAniImgFrame.h
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SGifPlayer.h
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SSkinGif.h
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SSkinAPNG.h
	${PROJECT_SOURCE_DIR}/controls.extend/SSkinImgFrame3.h
	${PROJECT_SOURCE_DIR}/controls.extend/SVscrollbar.h
	${PROJECT_SOURCE_DIR}/controls.extend/SSkinNewScrollbar.h
	${PROJECT_SOURCE_DIR}/controls.extend/FileHelper.h
	${PROJECT_SOURCE_DIR}/controls.extend/sipaddressctrl.h
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/SPropertyGrid.h
	${PROJECT_SOURCE_DIR}/controls.extend/sfadeframe.h
	${PROJECT_SOURCE_DIR}/controls.extend/sradiobox2.h
	${PROJECT_SOURCE_DIR}/controls.extend/SChromeTabCtrl.h
	${PROJECT_SOURCE_DIR}/controls.extend/siectrl.h
	${PROJECT_SOURCE_DIR}/controls.extend/SDocHostUIHandler.h
	${PROJECT_SOURCE_DIR}/controls.extend/schatedit.h
	${PROJECT_SOURCE_DIR}/controls.extend/reole/richeditole.h
	${PROJECT_SOURCE_DIR}/controls.extend/reole/dataobject.h
	${PROJECT_SOURCE_DIR}/controls.extend/sscrolltext.h
	${PROJECT_SOURCE_DIR}/controls.extend/scalendar2.h
	${PROJECT_SOURCE_DIR}/controls.extend/slistctrlex.h
	${PROJECT_SOURCE_DIR}/controls.extend/simagemaskwnd.h
	${PROJECT_SOURCE_DIR}/controls.extend/SRatingBar.h
	${PROJECT_SOURCE_DIR}/controls.extend/SFreeMoveWindow.h
	${PROJECT_SOURCE_DIR}/controls.extend/smiley/SSmileyCtrl.h
	${PROJECT_SOURCE_DIR}/controls.extend/TipWnd.h
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/STabCtrlHeaderBinder.h
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/SHeaderCtrlEx.h
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/SMCListViewEx.h
	${PROJECT_SOURCE_DIR}/controls.extend/SProgressRing.h
	${PROJECT_SOURCE_DIR}/controls.extend/SAniWindow.h
	${PROJECT_SOURCE_DIR}/controls.extend/SGroupList.h
)

set(EXTENDCOTROLS ${EXTENDCOTROLS}
	${PROJECT_SOURCE_DIR}/controls.extend/SWkeWebkit.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SGifPlayer.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SSkinGif.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/gif/SSkinAPNG.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SSkinImgFrame3.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/sipaddressctrl.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/spropertygrid.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/spropertyitembase.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-text.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-option.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-color.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-size.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/sfadeframe.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/sradiobox2.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SChromeTabCtrl.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/siectrl.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SDocHostUIHandler.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/schatedit.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/reole/richeditole.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/reole/dataobject.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/sscrolltext.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/scalendar2.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/slistctrlex.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/simagemaskwnd.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SRatingBar.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SFreeMoveWindow.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/TipWnd.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/STabCtrlHeaderBinder.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/SHeaderCtrlEx.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SMcListViewEx/SMCListViewEx.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SProgressRing.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SAniWindow.cpp
	${PROJECT_SOURCE_DIR}/controls.extend/SGroupList.cpp
)
source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})
source_group("SoUI Resouece" FILES ${CURRENT_SOUIS})
source_group("Resource Files" FILES ${CURRENT_RC})
source_group("Extend Cotrols" FILES ${EXTENDCOTROLS})

link_directories(${PROJECT_SOURCE_DIR}/bin)
add_executable(demo WIN32 ${CURRENT_HEADERS} ${CURRENT_SRCS} ${CURRENT_SOUIS} ${CURRENT_RC} ${EXTENDCOTROLS})

add_dependencies(demo soui-sys-resource ${COM_LIBS})
target_compile_definitions(demo
  PRIVATE DEMO_DEBUG_UI_RESOURCES_SEARCH_PATH="${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(demo
  soui
  utilities
  mhook
  smiley
  resprovider-7zip
  TaskLoop
  imgdecoder-gdip
  imgdecoder-png
  imgdecoder-stb
  imgdecoder-wic
  render-gdi
  render-skia
  log4z
  translator
)
set_target_properties (demo PROPERTIES
    FOLDER demos
)
cotire(demo)
