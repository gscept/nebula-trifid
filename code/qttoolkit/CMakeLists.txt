#-------------------------------------------------------------------------------
# QtToolkit
#-------------------------------------------------------------------------------

SET(QTTOOLS ${Nebula3_SOURCE_DIR}/qttoolkit/qttoolkitutil)
SET(QTADDONS ${Nebula3_SOURCE_DIR}/qttoolkit/qtaddons)
SET(NODY_FOLDER "N3SDK/QtToolkit/Nody")

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/foundation ${Nebula3_SOURCE_DIR}/qttoolkit)

IF(N_QT4)	
	add_subdirectory(batchexporter2)
	add_subdirectory(contentbrowser)
	add_subdirectory(leveleditor2)
	add_subdirectory(particleconverter)
	add_subdirectory(nody)
	if(MSVC)
		add_subdirectory(projectwizard)
	endif()
ELSE()
	add_subdirectory(leveleditor)
ENDIF()
add_subdirectory(batchexporter)
add_subdirectory(qtaddons)
add_subdirectory(qttools)
add_subdirectory(surfacegenerator)

