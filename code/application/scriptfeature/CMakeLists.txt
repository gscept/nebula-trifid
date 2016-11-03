#-------------------------------------------------------------------------------
# ScriptFeature
#-------------------------------------------------------------------------------


set(application_files_scriptfeature__actions__behaviour
	actions/behaviour/fsmaction.cc
	actions/behaviour/fsmaction.h
)
source_group("scriptfeature\\actions\\behaviour" FILES ${application_files_scriptfeature__actions__behaviour})

set(application_scriptfeature__actions__behaviour
	${application_files_scriptfeature__actions__behaviour}
)

set(application_files_scriptfeature__actions
	actions/action.cc
	actions/action.h
	actions/actionlist.cc
	actions/actionlist.h
	actions/ifthenelseaction.cc
	actions/ifthenelseaction.h
	actions/sequenceaction.cc
	actions/sequenceaction.h
)
source_group("scriptfeature\\actions" FILES ${application_files_scriptfeature__actions})

set(application_scriptfeature__actions
	${application_scriptfeature__actions__behaviour}
	${application_files_scriptfeature__actions}
)

set(application_files_scriptfeature__conditions__behaviour
	conditions/behaviour/fsmcondition.cc
	conditions/behaviour/fsmcondition.h
)
source_group("scriptfeature\\conditions\\behaviour" FILES ${application_files_scriptfeature__conditions__behaviour})

set(application_scriptfeature__conditions__behaviour
	${application_files_scriptfeature__conditions__behaviour}
)

set(application_files_scriptfeature__conditions
	conditions/and.cc
	conditions/and.h
	conditions/condition.cc
	conditions/condition.h
	conditions/not.cc
	conditions/not.h
	conditions/operator.h
	conditions/or.cc
	conditions/or.h
)
source_group("scriptfeature\\conditions" FILES ${application_files_scriptfeature__conditions})

set(application_scriptfeature__conditions
	${application_scriptfeature__conditions__behaviour}
	${application_files_scriptfeature__conditions}
)

set(application_files_scriptfeature__dialog
	dialog/dialog.cc
	dialog/dialog.h
	dialog/dialogdesc.cc
	dialog/dialogdesc.h
	dialog/dialogtake.cc
	dialog/dialogtake.h
)
source_group("scriptfeature\\dialog" FILES ${application_files_scriptfeature__dialog})

set(application_scriptfeature__dialog
	${application_files_scriptfeature__dialog}
)

set(application_files_scriptfeature__fsm
	fsm/behaviourproperty.cc
	fsm/behaviourproperty.h
	fsm/state.cc
	fsm/state.h
	fsm/statemachine.cc
	fsm/statemachine.h
	fsm/transition.cc
	fsm/transition.h
)
source_group("scriptfeature\\fsm" FILES ${application_files_scriptfeature__fsm})

set(application_scriptfeature__fsm
	${application_files_scriptfeature__fsm}
)

set(application_files_scriptfeature__log
	log/infolog.cc
	log/infolog.h
	log/infologfilter.cc
	log/infologfilter.h
)
source_group("scriptfeature\\log" FILES ${application_files_scriptfeature__log})

set(application_scriptfeature__log
	${application_files_scriptfeature__log}
)

set(application_files_scriptfeature__managers
	managers/dialogmanager.cc
	managers/dialogmanager.h
	managers/logmanager.cc
	managers/logmanager.h
	managers/questmanager.cc
	managers/questmanager.h
	managers/scriptmanager.cc
	managers/scriptmanager.h
	managers/scripttemplatemanager.cc
	managers/scripttemplatemanager.h
	managers/substitutionmanager.cc
	managers/substitutionmanager.h
)
source_group("scriptfeature\\managers" FILES ${application_files_scriptfeature__managers})

set(application_scriptfeature__managers
	${application_files_scriptfeature__managers}
)

set(application_files_scriptfeature__quest
	quest/task.cc
	quest/task.h
)
source_group("scriptfeature\\quest" FILES ${application_files_scriptfeature__quest})

set(application_scriptfeature__quest
	${application_files_scriptfeature__quest}
)     

set(application_files_scriptfeature
	actionreader.cc
	actionreader.h
	scriptfeatureunit.cc
	scriptfeatureunit.h
	scripttable.cc
	scripttable.h
)
source_group("scriptfeature" FILES ${application_files_scriptfeature})


set(_precompiled_src
	../stdneb.h
	../stdneb.cc
)

set(feature_sources
	${application_scriptfeature__actions}
	${application_scriptfeature__conditions}
	${application_scriptfeature__dialog}
	${application_scriptfeature__fsm}
	${application_scriptfeature__log}
	${application_scriptfeature__managers}
	${application_scriptfeature__quest}      
	${application_files_scriptfeature}
	${_precompiled_src}
)

set(feature_nidls 
	scriptprotocol.nidl
	
)

N_CREATE_FEATURE(scriptfeature feature_sources feature_nidls)
TARGET_INCLUDE_DIRECTORIES(scriptfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/scriptfeature)
TARGET_LINK_LIBRARIES(scriptfeature PUBLIC application)
ADD_DEPENDENCIES(scriptfeature application_NIDLS)

N_SET_COMPILER_DEFAULTS(scriptfeature)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(scriptfeature stdneb.h stdneb.cc ../)
ENDIF()