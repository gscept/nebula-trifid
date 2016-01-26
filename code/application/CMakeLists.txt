#-------------------------------------------------------------------------------
# Application
#-------------------------------------------------------------------------------

set(application_files_appgame
	appgame/appconfig.h
	appgame/gameapplication.cc
	appgame/gameapplication.h
	appgame/statehandler.cc
	appgame/statehandler.h
)
source_group("appgame" FILES ${application_files_appgame})

set(application_appgame
	${application_files_appgame}
)


set(application_files_commandhandlers
	commandhandlers/createentitycommand.cc
	commandhandlers/createentitycommand.h
)
source_group("commandhandlers" FILES ${application_files_commandhandlers})

set(application_commandhandlers
	${application_files_commandhandlers}
)

set(application_files_game
	game/entity.cc
	game/entity.h
	game/featureunit.cc
	game/featureunit.h
	game/gameserver.cc
	game/gameserver.h
	game/manager.cc
	game/manager.h
	game/property.cc
	game/property.h
)
source_group("game" FILES ${application_files_game})

set(application_game
	${application_files_game}
)


set(application_files_basegamefeature__basegametiming
	basegamefeature/basegametiming/gametimesource.cc
	basegamefeature/basegametiming/gametimesource.h
	basegamefeature/basegametiming/inputtimesource.cc
	basegamefeature/basegametiming/inputtimesource.h
	basegamefeature/basegametiming/systemtimesource.cc
	basegamefeature/basegametiming/systemtimesource.h
	basegamefeature/basegametiming/timemanager.cc
	basegamefeature/basegametiming/timemanager.h
	basegamefeature/basegametiming/timesource.cc
	basegamefeature/basegametiming/timesource.h
)
source_group("basegamefeature\\basegametiming" FILES ${application_files_basegamefeature__basegametiming})

set(application_basegamefeature__basegametiming
	${application_files_basegamefeature__basegametiming}
)

set(application_files_basegamefeature__debug
	basegamefeature/debug/objectinspectorhandler.cc
	basegamefeature/debug/objectinspectorhandler.h
)
source_group("basegamefeature\\debug" FILES ${application_files_basegamefeature__debug})

set(application_basegamefeature__debug
	${application_files_basegamefeature__debug}
)

set(application_files_basegamefeature__loader
	basegamefeature/loader/entityloader.cc
	basegamefeature/loader/entityloader.h
	basegamefeature/loader/entityloaderbase.cc
	basegamefeature/loader/entityloaderbase.h
	basegamefeature/loader/environmentloader.cc
	basegamefeature/loader/environmentloader.h
	basegamefeature/loader/levelloader.cc
	basegamefeature/loader/levelloader.h
	basegamefeature/loader/loaderserver.cc
	basegamefeature/loader/loaderserver.h
	basegamefeature/loader/userprofile.cc
	basegamefeature/loader/userprofile.h
)
source_group("basegamefeature\\loader" FILES ${application_files_basegamefeature__loader})

set(application_basegamefeature__loader
	${application_files_basegamefeature__loader}
)

set(application_files_basegamefeature__managers        
	basegamefeature/managers/audiomanager.cc
	basegamefeature/managers/audiomanager.h
	basegamefeature/managers/categorymanager.cc
	basegamefeature/managers/categorymanager.h
	basegamefeature/managers/entitymanager.cc
	basegamefeature/managers/entitymanager.h
	basegamefeature/managers/enventitymanager.cc
	basegamefeature/managers/enventitymanager.h
	basegamefeature/managers/envquerymanager.cc
	basegamefeature/managers/envquerymanager.h
	basegamefeature/managers/factorymanager.cc
	basegamefeature/managers/factorymanager.h
	basegamefeature/managers/focusmanager.cc
	basegamefeature/managers/focusmanager.h
	basegamefeature/managers/globalattrsmanager.cc
	basegamefeature/managers/globalattrsmanager.h
)
source_group("basegamefeature\\managers" FILES ${application_files_basegamefeature__managers})

set(application_basegamefeature__managers
	${application_files_basegamefeature__managers}
)

set(application_files_basegamefeature__properties
	basegamefeature/properties/timeproperty.cc
	basegamefeature/properties/timeproperty.h
	basegamefeature/properties/transformableproperty.cc
	basegamefeature/properties/transformableproperty.h
)
source_group("basegamefeature\\properties" FILES ${application_files_basegamefeature__properties})

set(application_basegamefeature__properties
	${application_files_basegamefeature__properties}
)

set(application_files_basegamefeature__statehandlers
	basegamefeature/statehandlers/gamestatehandler.cc
	basegamefeature/statehandlers/gamestatehandler.h
)
source_group("basegamefeature\\statehandlers" FILES ${application_files_basegamefeature__statehandlers})

set(application_basegamefeature__statehandlers
	${application_files_basegamefeature__statehandlers}
)

set(application_files_basegamefeature
	basegamefeature/basegamefeatureunit.cc
	basegamefeature/basegamefeatureunit.h
)
source_group("basegamefeature" FILES ${application_files_basegamefeature})

set(application_basegamefeature      
	${application_basegamefeature__basegametiming}
	${application_basegamefeature__debug}
	${application_basegamefeature__loader}
	${application_basegamefeature__managers}
	${application_basegamefeature__properties}
	${application_basegamefeature__statehandlers}
	${application_files_basegamefeature}
)


 


set(application_precompiled_src
	stdneb.h
	stdneb.cc
)
SOURCE_GROUP("Precompiled" FILES ${application_precompiled_src})
set(P_NIDL_FILES
	basegamefeature/basegameprotocol.nidl
	basegamefeature/basegameattr/basegameattributes.nidl
	basegamefeature/properties/basegameproperties.nidl	
	graphicsfeature/graphicsattr/graphicsattributes.nidl
	physicsfeature/physicsattr/physicsattributes.nidl
	scriptfeature/scriptattr/scriptattributes.nidl
)




set(application_SRCS
	${application_appgame}	
	${application_basegamefeature}
	${application_commandhandlers}
	${application_game}	
	${application_precompiled_src}
)

add_library( application STATIC ${application_SRCS})


N_WRAP_NIDL_FILES(application_NIDLS ${P_NIDL_FILES})

TARGET_LINK_LIBRARIES(application PUBLIC application_NIDLS posteffect attr db framecapture multiplayer faudio remote  vibration)
TARGET_INCLUDE_DIRECTORIES(application PUBLIC  ${Nebula3_SOURCE_DIR}/code/application ${Nebula3_SOURCE_DIR}/code/application/basegamefeature ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature)
TARGET_INCLUDE_DIRECTORIES(application PRIVATE ${Nebula3_SOURCE_DIR}/code) 

SET_TARGET_PROPERTIES(application PROPERTIES FOLDER "N3SDK/N3")

ADD_DEPENDENCIES(application application_NIDLS multiplayer_NIDLS foundation render attr db sqlite tinyxml zlib ${PHYSICS_DEPENDENCIES})
TARGET_INCLUDE_DIRECTORIES(application_NIDLS PUBLIC $<TARGET_PROPERTY:application,INTERFACE_INCLUDE_DIRECTORIES>)


N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(application stdneb.h stdneb.cc)
ENDIF()

ADD_SUBDIRECTORY(graphicsfeature)
ADD_SUBDIRECTORY(physicsfeature)
ADD_SUBDIRECTORY(scriptfeature)
ADD_SUBDIRECTORY(scriptingfeature)
ADD_SUBDIRECTORY(stateobjectfeature)

