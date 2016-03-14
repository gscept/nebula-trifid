#-------------------------------------------------------------------------------
# ui
#-------------------------------------------------------------------------------

SET(N_BUILD_LIBROCKET OFF)
SET(N_BUILD_CEUI OFF)
SET(N_DEFAULT_UI "N_BUILD_LIBROCKET")
SET(N_UI ${N_DEFAULT_UI} CACHE STRING "UI backend chosen by CMake")
SET_PROPERTY(CACHE N_UI PROPERTY STRINGS "N_BUILD_LIBROCKET" "N_BUILD_CEUI")
SET(${N_UI} ON)


set(SOURCES_files_ui__base
	base/uilayoutbase.cc
	base/uilayoutbase.h
	base/uiserverbase.cc
	base/uiserverbase.h
	base/uielementbase.cc
	base/uielementbase.h		
	base/uiplugin.cc
	base/uiplugin.h		
)
source_group("ui\\base" FILES ${SOURCES_files_ui__base})

set(SOURCES_ui__base
	${SOURCES_files_ui__base}
)

set(SOURCES_files_ui__ceui
	ceui/ceuigeometrybuffer.cc
	ceui/ceuigeometrybuffer.h
	ceui/ceuilayout.cc
	ceui/ceuilayout.h
	ceui/ceuirawdatacontainer.cc
	ceui/ceuirawdatacontainer.h
	ceui/ceuirenderer.cc
	ceui/ceuirenderer.h
	ceui/ceuirendertarget.cc
	ceui/ceuirendertarget.h
	ceui/ceuiresourceprovider.cc
	ceui/ceuiresourceprovider.h
	ceui/ceuiserver.cc
	ceui/ceuiserver.h
	ceui/ceuitexture.cc
	ceui/ceuitexture.h
	ceui/ceuiviewporttarget.cc
	ceui/ceuiviewporttarget.h
 )
source_group("ui\\ceui" FILES ${SOURCES_files_ui__ceui})

set(SOURCES_ui__ceui
	${SOURCES_files_ui__ceui}
)
  
set(SOURCES_files_ui__librocket
	rocket/rocketlayout.cc
	rocket/rocketlayout.h
	rocket/rocketeventinstancer.cc
	rocket/rocketeventinstancer.h
	rocket/rocketeventlistenerinstancer.cc
	rocket/rocketeventlistenerinstancer.h
	rocket/rocketeventlistener.h
	rocket/rocketeventlistener.cc
	rocket/rocketrenderer.cc
	rocket/rocketrenderer.h
	rocket/rocketinterface.cc
	rocket/rocketinterface.h
	rocket/rocketserver.cc
	rocket/rocketserver.h
	rocket/rocketelement.cc
	rocket/rocketelement.h
)
source_group("ui\\librocket" FILES ${SOURCES_files_ui__librocket})

set(SOURCES_ui__librocket
	${SOURCES_files_ui__librocket}
)

set(SOURCES_files_ui
	uievent.h
	uielementgroup.cc
	uielementgroup.h
	uieventhandler.cc
	uieventhandler.h
	uifeatureunit.cc
	uifeatureunit.h
	uiinputhandler.cc
	uiinputhandler.h  
	uiserver.cc
	uiserver.h
	uilayout.cc
	uilayout.h
	uielement.cc
	uielement.h
	uirtplugin.cc
	uirtplugin.h
)
source_group("ui" FILES ${SOURCES_files_ui})

set(SOURCES_ui
  ${SOURCES_ui__base}
  ${SOURCES_ui__librocket}
  ${SOURCES_files_ui}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})

set(SOURCES_
	${SOURCES_ui}
	${SOURCES_files}		
)
	
set(ui_nidls uicommands.nidl uiattrs/uiattributes.nidl)


N_CREATE_FEATURE(ui SOURCES_ ui_nidls )

TARGET_LINK_LIBRARIES(ui PUBLIC render application)

ADD_DEPENDENCIES(ui faudio render)

TARGET_INCLUDE_DIRECTORIES(ui PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs/librocket/ ${Nebula3_SOURCE_DIR}/code/addons)
TARGET_COMPILE_DEFINITIONS(ui PUBLIC $<$<BOOL:${N_BUILD_LIBROCKET}>:__LIBROCKET__> $<$<BOOL:${N_BUILD_CEUI}>:__CEUI__>)
TARGET_LINK_LIBRARIES(ui PUBLIC $<$<BOOL:${N_BUILD_LIBROCKET}>:librocket $<$<PLATFORM_ID:Linux>:freetype> $<$<PLATFORM_ID:Windows>:libfreetype>> )

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(ui stdneb.h stdneb.cc ../)
ENDIF()
