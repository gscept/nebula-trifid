#-------------------------------------------------------------------------------
# qt
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code
                    ${Nebula3_SOURCE_DIR}/code/addons
                    ${Nebula3_SOURCE_DIR}/code/foundation
                    ${Nebula3_SOURCE_DIR}/code/render
                    ${Nebula3_SOURCE_DIR}/code/application
                    ${Nebula3_SOURCE_DIR}/code/extlibs
                    ${Nebula3_SOURCE_DIR}/code/libs
)


    set(SOURCES_files_qt
        qtnebulawidget.h
        qtnebulawidget.cc
        qtfeatureunit.h
        qtfeatureunit.cc
        qtserver.h
        qtserver.cc
        qtinputproxy.h
        qtinputproxy.cc
    )
    source_group("qtfeature" FILES ${SOURCES_files_qt})

set(SOURCES_files
    ../stdneb.h
    ../stdneb.cc
     ${SOURCES_files_qt}
)

SET(QTMOCFILES  "qtnebulawidget.h")

IF(N_QT4)
	INCLUDE(${QT_USE_FILE})
	ADD_DEFINITIONS(${QT_DEFINITIONS})
ENDIF()

PCH_QT_WRAP_CPP(qtfeature_HEADERS_MOC ${QTMOCFILES})


ADD_LIBRARY( qtfeature STATIC ${SOURCES_files} ${qtfeature_HEADERS_MOC})
ADD_DEPENDENCIES(qtfeature application_NIDLS)
SET_TARGET_PROPERTIES(qtfeature PROPERTIES FOLDER "N3SDK/addons")
TARGET_LINK_LIBRARIES(qtfeature PUBLIC qtsupport)

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(qtfeature stdneb.h stdneb.cc ../)
ENDIF()
 
