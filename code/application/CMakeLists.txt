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

set(application_files_graphicsfeature__graphicsutil
	graphicsfeature/graphicsutil/lightflickerutil.cc
	graphicsfeature/graphicsutil/lightflickerutil.h
	graphicsfeature/graphicsutil/segmentedgfxutil.cc
	graphicsfeature/graphicsutil/segmentedgfxutil.h
	graphicsfeature/graphicsutil/animutil.cc
	graphicsfeature/graphicsutil/animutil.h
	graphicsfeature/graphicsutil/attachmentutil.cc
	graphicsfeature/graphicsutil/attachmentutil.h		
)
source_group("graphicsfeature\\graphicsutil" FILES ${application_files_graphicsfeature__graphicsutil})

set(application_graphicsfeature__graphicsutil
	${application_files_graphicsfeature__graphicsutil}
)
	  
set(application_files_graphicsfeature__animpath
	graphicsfeature/animpath/animpath.h
	graphicsfeature/animpath/pathanimation.cc
	graphicsfeature/animpath/pathanimation.h
	graphicsfeature/animpath/managedpathanimation.cc
	graphicsfeature/animpath/managedpathanimation.h
	graphicsfeature/animpath/streampathanimationloader.cc
	graphicsfeature/animpath/streampathanimationloader.h
)
source_group("graphicsfeature\\animpath" FILES ${application_files_graphicsfeature__animpath})
	  
set(application_graphicsfeature__animpath
	${application_files_graphicsfeature__animpath}
)

set(application_files_graphicsfeature__managers
	graphicsfeature/managers/attachmentmanager.cc
	graphicsfeature/managers/attachmentmanager.h
)
source_group("graphicsfeature\\managers" FILES ${application_files_graphicsfeature__managers})

set(application_graphicsfeature__managers
	${application_files_graphicsfeature__managers}
)

set(application_files_graphicsfeature__properties
	graphicsfeature/properties/actorgraphicsproperty.cc
	graphicsfeature/properties/actorgraphicsproperty.h
	graphicsfeature/properties/ambiencebubbleproperty.cc
	graphicsfeature/properties/ambiencebubbleproperty.h
	graphicsfeature/properties/animationcontrolproperty.cc
	graphicsfeature/properties/animationcontrolproperty.h
	graphicsfeature/properties/cameraproperty.cc
	graphicsfeature/properties/cameraproperty.h
	graphicsfeature/properties/chasecameraproperty.cc
	graphicsfeature/properties/chasecameraproperty.h
	graphicsfeature/properties/cutscenecameraproperty.cc
	graphicsfeature/properties/cutscenecameraproperty.h
	graphicsfeature/properties/environmentgraphicsproperty.cc
	graphicsfeature/properties/environmentgraphicsproperty.h
	graphicsfeature/properties/graphicsproperty.cc
	graphicsfeature/properties/graphicsproperty.h
	graphicsfeature/properties/inputproperty.cc
	graphicsfeature/properties/inputproperty.h
	graphicsfeature/properties/lightprobeproperty.cc
	graphicsfeature/properties/lightprobeproperty.h
	graphicsfeature/properties/lightproperty.cc
	graphicsfeature/properties/lightproperty.h
	graphicsfeature/properties/mayacameraproperty.cc
	graphicsfeature/properties/mayacameraproperty.h
	graphicsfeature/properties/animpathproperty.cc
	graphicsfeature/properties/animpathproperty.h
	graphicsfeature/properties/pathanimproperty.cc
	graphicsfeature/properties/pathanimproperty.h
)
source_group("graphicsfeature\\properties" FILES ${application_files_graphicsfeature__properties})

set(application_graphicsfeature__properties
	${application_files_graphicsfeature__properties}
)

set(application_files_graphicsfeature
	graphicsfeature/graphicsfeatureproperties.h
	graphicsfeature/graphicsfeatureunit.cc
	graphicsfeature/graphicsfeatureunit.h
	graphicsfeature/gameanimeventhandler.cc
	graphicsfeature/gameanimeventhandler.h
)
source_group("graphicsfeature" FILES ${application_files_graphicsfeature})

set(application_graphicsfeature      
	${application_graphicsfeature__graphicsutil}
	${application_graphicsfeature__animpath}
	${application_graphicsfeature__managers}
	${application_graphicsfeature__properties}
	${application_files_graphicsfeature}
)
set(application_files_physicsfeature__properties
	physicsfeature/properties/actorphysicsproperty.cc
	physicsfeature/properties/actorphysicsproperty.h
	physicsfeature/properties/animatedphysicsproperty.cc
	physicsfeature/properties/animatedphysicsproperty.h
	physicsfeature/properties/environmentcollideproperty.cc
	physicsfeature/properties/environmentcollideproperty.h
	physicsfeature/properties/mousegripperproperty.cc
	physicsfeature/properties/mousegripperproperty.h
	physicsfeature/properties/physicsproperty.cc
	physicsfeature/properties/physicsproperty.h        
	physicsfeature/properties/triggerproperty.cc
	physicsfeature/properties/triggerproperty.h
	physicsfeature/properties/pointnclickinputproperty.cc
	physicsfeature/properties/pointnclickinputproperty.h
)
source_group("physicsfeature\\properties" FILES ${application_files_physicsfeature__properties})

set(application_physicsfeature__properties
	${application_files_physicsfeature__properties}
)

set(application_files_physicsfeature
	physicsfeature/physicsfeatureunit.cc
	physicsfeature/physicsfeatureunit.h
	physicsfeature/gamefilterset.cc
	physicsfeature/gamefilterset.h
)
source_group("physicsfeature" FILES ${application_files_physicsfeature})

set(application_physicsfeature      
	${application_physicsfeature__properties}
${application_files_physicsfeature}
)
    
set(scripting_files
	scriptingfeature/scriptingfeature.cc
	scriptingfeature/scriptingfeature.h
	scriptingfeature/scriptinginputhandler.cc
	scriptingfeature/scriptinginputhandler.h
	scriptingfeature/properties/scriptingproperty.cc
	scriptingfeature/properties/scriptingproperty.h
	scriptingfeature/properties/scriptingtriggerproperty.cc
	scriptingfeature/properties/scriptingtriggerproperty.h
)
source_group("scriptingfeature" FILES ${scripting_files})
	

set(application_files_scriptfeature__actions__behaviour
	scriptfeature/actions/behaviour/fsmaction.cc
	scriptfeature/actions/behaviour/fsmaction.h
)
source_group("scriptfeature\\actions\\behaviour" FILES ${application_files_scriptfeature__actions__behaviour})

set(application_scriptfeature__actions__behaviour
	${application_files_scriptfeature__actions__behaviour}
)

set(application_files_scriptfeature__actions
	scriptfeature/actions/action.cc
	scriptfeature/actions/action.h
	scriptfeature/actions/actionlist.cc
	scriptfeature/actions/actionlist.h
	scriptfeature/actions/ifthenelseaction.cc
	scriptfeature/actions/ifthenelseaction.h
	scriptfeature/actions/sequenceaction.cc
	scriptfeature/actions/sequenceaction.h
)
source_group("scriptfeature\\actions" FILES ${application_files_scriptfeature__actions})

set(application_scriptfeature__actions
	${application_scriptfeature__actions__behaviour}
	${application_files_scriptfeature__actions}
)

set(application_files_scriptfeature__conditions__behaviour
	scriptfeature/conditions/behaviour/fsmcondition.cc
	scriptfeature/conditions/behaviour/fsmcondition.h
)
source_group("scriptfeature\\conditions\\behaviour" FILES ${application_files_scriptfeature__conditions__behaviour})

set(application_scriptfeature__conditions__behaviour
	${application_files_scriptfeature__conditions__behaviour}
)

set(application_files_scriptfeature__conditions
	scriptfeature/conditions/and.cc
	scriptfeature/conditions/and.h
	scriptfeature/conditions/condition.cc
	scriptfeature/conditions/condition.h
	scriptfeature/conditions/not.cc
	scriptfeature/conditions/not.h
	scriptfeature/conditions/operator.h
	scriptfeature/conditions/or.cc
	scriptfeature/conditions/or.h
)
source_group("scriptfeature\\conditions" FILES ${application_files_scriptfeature__conditions})

set(application_scriptfeature__conditions
	${application_scriptfeature__conditions__behaviour}
	${application_files_scriptfeature__conditions}
)

set(application_files_scriptfeature__dialog
	scriptfeature/dialog/dialog.cc
	scriptfeature/dialog/dialog.h
	scriptfeature/dialog/dialogdesc.cc
	scriptfeature/dialog/dialogdesc.h
	scriptfeature/dialog/dialogtake.cc
	scriptfeature/dialog/dialogtake.h
)
source_group("scriptfeature\\dialog" FILES ${application_files_scriptfeature__dialog})

set(application_scriptfeature__dialog
	${application_files_scriptfeature__dialog}
)

set(application_files_scriptfeature__fsm
	scriptfeature/fsm/behaviourproperty.cc
	scriptfeature/fsm/behaviourproperty.h
	scriptfeature/fsm/state.cc
	scriptfeature/fsm/state.h
	scriptfeature/fsm/statemachine.cc
	scriptfeature/fsm/statemachine.h
	scriptfeature/fsm/transition.cc
	scriptfeature/fsm/transition.h
)
source_group("scriptfeature\\fsm" FILES ${application_files_scriptfeature__fsm})

set(application_scriptfeature__fsm
	${application_files_scriptfeature__fsm}
)

set(application_files_scriptfeature__log
	scriptfeature/log/infolog.cc
	scriptfeature/log/infolog.h
	scriptfeature/log/infologfilter.cc
	scriptfeature/log/infologfilter.h
)
source_group("scriptfeature\\log" FILES ${application_files_scriptfeature__log})

set(application_scriptfeature__log
	${application_files_scriptfeature__log}
)

set(application_files_scriptfeature__managers
	scriptfeature/managers/dialogmanager.cc
	scriptfeature/managers/dialogmanager.h
	scriptfeature/managers/logmanager.cc
	scriptfeature/managers/logmanager.h
	scriptfeature/managers/questmanager.cc
	scriptfeature/managers/questmanager.h
	scriptfeature/managers/scriptmanager.cc
	scriptfeature/managers/scriptmanager.h
	scriptfeature/managers/scripttemplatemanager.cc
	scriptfeature/managers/scripttemplatemanager.h
	scriptfeature/managers/substitutionmanager.cc
	scriptfeature/managers/substitutionmanager.h
)
source_group("scriptfeature\\managers" FILES ${application_files_scriptfeature__managers})

set(application_scriptfeature__managers
	${application_files_scriptfeature__managers}
)

set(application_files_scriptfeature__quest
	scriptfeature/quest/task.cc
	scriptfeature/quest/task.h
)
source_group("scriptfeature\\quest" FILES ${application_files_scriptfeature__quest})

set(application_scriptfeature__quest
	${application_files_scriptfeature__quest}
)     

set(application_files_scriptfeature
	scriptfeature/actionreader.cc
	scriptfeature/actionreader.h
	scriptfeature/scriptfeatureunit.cc
	scriptfeature/scriptfeatureunit.h
	scriptfeature/scripttable.cc
	scriptfeature/scripttable.h
)
source_group("scriptfeature" FILES ${application_files_scriptfeature})

set(application_scriptfeature
	${application_scriptfeature__actions}
	${application_scriptfeature__conditions}
	${application_scriptfeature__dialog}
	${application_scriptfeature__fsm}
	${application_scriptfeature__log}
	${application_scriptfeature__managers}
	${application_scriptfeature__quest}      
	${application_files_scriptfeature}
)

set(application_files_stateobjectfeature__properties
	stateobjectfeature/properties/stategraphicsproperty.cc
	stateobjectfeature/properties/stategraphicsproperty.h
	stateobjectfeature/properties/stateinfo.h
	stateobjectfeature/properties/stateproperty.cc
	stateobjectfeature/properties/stateproperty.h
)
source_group("stateobjectfeature\\properties" FILES ${application_files_stateobjectfeature__properties})

set(application_stateobjectfeature__properties
	${application_files_stateobjectfeature__properties}
)

set(application_files_stateobjectfeature
	stateobjectfeature/stateobjectfeatureunit.h
)
source_group("stateobjectfeature" FILES ${application_files_stateobjectfeature})

set(application_stateobjectfeature
	${application_stateobjectfeature__properties}
	${application_files_stateobjectfeature}
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
	graphicsfeature/graphicsfeatureprotocol.nidl
	graphicsfeature/graphicsattr/graphicsattributes.nidl
	physicsfeature/physicsprotocol.nidl
	physicsfeature/physicsattr/physicsattributes.nidl
	scriptfeature/scriptprotocol.nidl
	stateobjectfeature/stateobjectprotocol.nidl
	scriptingfeature/scriptingcommands.nidl
	scriptingfeature/scriptingprotocol.nidl
	scriptfeature/scriptattr/scriptattributes.nidl
)

N_WRAP_NIDL_FILES(application_NIDLS ${P_NIDL_FILES})

set(application_SRCS
	${application_appgame}
	${application_basegamefeature}
	${application_commandhandlers}
	${application_game}
	${application_graphicsfeature}
	${scripting_files}
	${application_physicsfeature}
	${application_scriptfeature}
	${application_stateobjectfeature}
	${application_precompiled_src}
)

add_library( application STATIC ${application_SRCS} )
TARGET_LINK_LIBRARIES(application PUBLIC application_NIDLS render posteffect scripting ui attr db framecapture navigation multiplayer faudio remote effects vibration)
TARGET_INCLUDE_DIRECTORIES(application PUBLIC ${Nebula3_SOURCE_DIR}/code/application ${Nebula3_SOURCE_DIR}/code/application/basegamefeature ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature ${Nebula3_SOURCE_DIR}/code/application/physicsfeature)
TARGET_INCLUDE_DIRECTORIES(application_NIDLS PUBLIC $<TARGET_PROPERTY:application,INTERFACE_INCLUDE_DIRECTORIES>)
SET_TARGET_PROPERTIES(application PROPERTIES FOLDER "N3SDK/N3")

ADD_DEPENDENCIES(application application_NIDLS multiplayer_NIDLS foundation render attr db sqlite tinyxml zlib ${PHYSICS_DEPENDENCIES})
ADD_DEPENDENCIES(application_NIDLS render_NIDLS)

TARGET_LINK_LIBRARIES(application_NIDLS render)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(application stdneb.h stdneb.cc)
ENDIF()
