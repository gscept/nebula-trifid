#-------------------------------------------------------------------------------
# GraphicsFeature
#-------------------------------------------------------------------------------

set(application_files_graphicsfeature__graphicsutil
	graphicsutil/lightflickerutil.cc
	graphicsutil/lightflickerutil.h
	graphicsutil/segmentedgfxutil.cc
	graphicsutil/segmentedgfxutil.h
	graphicsutil/animutil.cc
	graphicsutil/animutil.h
	graphicsutil/attachmentutil.cc
	graphicsutil/attachmentutil.h		
)
source_group("graphicsfeature\\graphicsutil" FILES ${application_files_graphicsfeature__graphicsutil})

set(application_graphicsfeature__graphicsutil
	${application_files_graphicsfeature__graphicsutil}
)
	  
set(application_files_graphicsfeature__animpath
	animpath/animpath.h
	animpath/pathanimation.cc
	animpath/pathanimation.h
	animpath/managedpathanimation.cc
	animpath/managedpathanimation.h
	animpath/streampathanimationloader.cc
	animpath/streampathanimationloader.h
)
source_group("graphicsfeature\\animpath" FILES ${application_files_graphicsfeature__animpath})
	  
set(application_graphicsfeature__animpath
	${application_files_graphicsfeature__animpath}
)

set(application_files_graphicsfeature__loaders
	loader/environmentloader.cc
	loader/environmentloader.h
)
source_group("graphicsfeature\\loader" FILES ${application_files_graphicsfeature__loaders})	

set(application_files_graphicsfeature__managers
	managers/attachmentmanager.cc
	managers/attachmentmanager.h
	managers/enventitymanager.cc
	managers/enventitymanager.h
)
source_group("graphicsfeature\\managers" FILES ${application_files_graphicsfeature__managers})

set(application_graphicsfeature__managers
	${application_files_graphicsfeature__managers}
)

set(application_files_graphicsfeature__properties
	properties/actorgraphicsproperty.cc
	properties/actorgraphicsproperty.h
	properties/ambiencebubbleproperty.cc
	properties/ambiencebubbleproperty.h
	properties/animationcontrolproperty.cc
	properties/animationcontrolproperty.h
	properties/cameraproperty.cc
	properties/cameraproperty.h
	properties/fpscameraproperty.cc
	properties/fpscameraproperty.h
	properties/chasecameraproperty.cc
	properties/chasecameraproperty.h
	properties/cutscenecameraproperty.cc
	properties/cutscenecameraproperty.h
	properties/environmentgraphicsproperty.cc
	properties/environmentgraphicsproperty.h
	properties/graphicsproperty.cc
	properties/graphicsproperty.h
	properties/inputproperty.cc
	properties/inputproperty.h
	properties/lightprobeproperty.cc
	properties/lightprobeproperty.h
	properties/lightproperty.cc
	properties/lightproperty.h
	properties/mayacameraproperty.cc
	properties/mayacameraproperty.h
	properties/animpathproperty.cc
	properties/animpathproperty.h
	properties/pathanimproperty.cc
	properties/pathanimproperty.h
	properties/freemoveproperty.cc
	properties/freemoveproperty.h
)
source_group("graphicsfeature\\properties" FILES ${application_files_graphicsfeature__properties})

set(application_graphicsfeature__properties
	${application_files_graphicsfeature__properties}
)

set(application_files_graphicsfeature
	graphicsfeatureproperties.h
	graphicsfeatureunit.cc
	graphicsfeatureunit.h
	gameanimeventhandler.cc
	gameanimeventhandler.h
)
source_group("graphicsfeature" FILES ${application_files_graphicsfeature})

set(graphicsfeature_precompiled_src
	../stdneb.h
	../stdneb.cc
)
set(application_graphicsfeature      
	${application_graphicsfeature__graphicsutil}
	${application_graphicsfeature__animpath}
	${application_files_graphicsfeature__loaders}
	${application_graphicsfeature__managers}
	${application_graphicsfeature__properties}
	${application_files_graphicsfeature}
	${graphicsfeature_precompiled_src}
)



set(graphics_nidls 
	
)

N_CREATE_FEATURE(graphicsfeature application_graphicsfeature graphics_nidls)
TARGET_INCLUDE_DIRECTORIES(graphicsfeature PUBLIC ${Nebula3_SOURCE_DIR}/code/application/graphicsfeature)
TARGET_LINK_LIBRARIES(graphicsfeature PUBLIC application render posteffect effects physics)
ADD_DEPENDENCIES(graphicsfeature application_NIDLS render_NIDLS posteffect)

N_SET_COMPILER_DEFAULTS(graphicsfeature)
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(graphicsfeature stdneb.h stdneb.cc ../)
ENDIF()