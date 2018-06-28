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
	${CMAKE_SOURCE_DIR}/controls.extend/SWkeWebkit.h
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SAniImgFrame.h
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SGifPlayer.h
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SSkinGif.h
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SSkinAPNG.h
	${CMAKE_SOURCE_DIR}/controls.extend/SSkinImgFrame3.h
	${CMAKE_SOURCE_DIR}/controls.extend/SVscrollbar.h
	${CMAKE_SOURCE_DIR}/controls.extend/SSkinNewScrollbar.h
	${CMAKE_SOURCE_DIR}/controls.extend/FileHelper.h
	${CMAKE_SOURCE_DIR}/controls.extend/sipaddressctrl.h
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/SPropertyGrid.h
	${CMAKE_SOURCE_DIR}/controls.extend/sfadeframe.h
	${CMAKE_SOURCE_DIR}/controls.extend/sradiobox2.h
	${CMAKE_SOURCE_DIR}/controls.extend/SChromeTabCtrl.h
	${CMAKE_SOURCE_DIR}/controls.extend/siectrl.h
	${CMAKE_SOURCE_DIR}/controls.extend/SDocHostUIHandler.h
	${CMAKE_SOURCE_DIR}/controls.extend/schatedit.h
	${CMAKE_SOURCE_DIR}/controls.extend/reole/richeditole.h
	${CMAKE_SOURCE_DIR}/controls.extend/reole/dataobject.h
	${CMAKE_SOURCE_DIR}/controls.extend/sscrolltext.h
	${CMAKE_SOURCE_DIR}/controls.extend/scalendar2.h
	${CMAKE_SOURCE_DIR}/controls.extend/slistctrlex.h
	${CMAKE_SOURCE_DIR}/controls.extend/simagemaskwnd.h
	${CMAKE_SOURCE_DIR}/controls.extend/SRatingBar.h
	${CMAKE_SOURCE_DIR}/controls.extend/SFreeMoveWindow.h
	${CMAKE_SOURCE_DIR}/controls.extend/smiley/SSmileyCtrl.h
	${CMAKE_SOURCE_DIR}/controls.extend/TipWnd.h
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/STabCtrlHeaderBinder.h
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/SHeaderCtrlEx.h
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/SMCListViewEx.h
)

set(EXTENDCOTROLS ${EXTENDCOTROLS}
	${CMAKE_SOURCE_DIR}/controls.extend/SWkeWebkit.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SGifPlayer.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SSkinGif.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/gif/SSkinAPNG.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SSkinImgFrame3.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/sipaddressctrl.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/spropertygrid.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/spropertyitembase.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-text.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-option.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-color.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/propgrid/propitem/spropertyitem-size.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/sfadeframe.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/sradiobox2.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SChromeTabCtrl.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/siectrl.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SDocHostUIHandler.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/schatedit.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/reole/richeditole.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/reole/dataobject.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/sscrolltext.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/scalendar2.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/slistctrlex.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/simagemaskwnd.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SRatingBar.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SFreeMoveWindow.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/TipWnd.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/STabCtrlHeaderBinder.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/SHeaderCtrlEx.cpp
	${CMAKE_SOURCE_DIR}/controls.extend/SMcListViewEx/SMCListViewEx.cpp
)
source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})
source_group("SoUI Resouece" FILES ${CURRENT_SOUIS})
source_group("Resource Files" FILES ${CURRENT_RC})
source_group("Extend Cotrols" FILES ${EXTENDCOTROLS})

add_executable(demo WIN32 ${CURRENT_HEADERS} ${CURRENT_SRCS} ${CURRENT_SOUIS} ${CURRENT_RC} ${EXTENDCOTROLS})

set_target_properties(demo PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
add_dependencies(demo soui-sys-resource)
target_link_libraries(demo soui utilities ${COM_LIBS} mhook smiley lua ScriptModule-Lua)
set_target_properties (demo PROPERTIES
    FOLDER demos
)
cotire(demo)
