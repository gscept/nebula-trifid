#-------------------------------------------------------------------------------
# QtToolkit
#-------------------------------------------------------------------------------

SET(QTTOOLS ${Nebula3_SOURCE_DIR}/qttoolkit/qttoolkitutil)
SET(QTADDONS ${Nebula3_SOURCE_DIR}/qttoolkit/qtaddons)
SET(NODY_FOLDER "N3SDK/QtToolkit/Nody")

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/foundation ${Nebula3_SOURCE_DIR}/qttoolkit)


add_subdirectory(batchexporter)
add_subdirectory(contentbrowser)
add_subdirectory(leveleditor2)
if(MSVC)
add_subdirectory(projectwizard)
endif()
add_subdirectory(nody)
add_subdirectory(qtaddons)
add_subdirectory(qttools)
