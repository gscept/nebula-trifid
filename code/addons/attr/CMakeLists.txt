#-------------------------------------------------------------------------------
# attr
#-------------------------------------------------------------------------------

set(SOURCES_files_attr
	accessmode.h
	attrexithandler.cc
	attrexithandler.h
	attribute.h
	attributecontainer.cc
	attributecontainer.h
	attributedefinition.h
	attributedefinitionbase.cc
	attributedefinitionbase.h
	attributetable.cc
	attributetable.h
	attrid.cc
	attrid.h
	blobattrid.h
	boolattrid.h
	float4attrid.h
	floatattrid.h
	guidattrid.h
	intattrid.h
	matrix44attrid.h
	transform44attrid.h
	stringattrid.h
	valuetype.h
	io/attrcontainerstoragebase.cc
	io/attrcontainerstoragebase.h
	io/attrcontainerdbstorage.cc
	io/attrcontainerdbstorage.h
	io/attrcontainerxmlstorage.cc
	io/attrcontainerxmlstorage.h	
)
source_group("attr" FILES ${SOURCES_files_attr})

set(SOURCES_attr
  ${SOURCES_files_attr}
)

set(SOURCES_files
	../stdneb.h
	../stdneb.cc
)

SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
set(SOURCES_SRCS
	${SOURCES_attr}
	${SOURCES_files}
)

add_library( attr STATIC ${SOURCES_SRCS} )

TARGET_INCLUDE_DIRECTORIES(attr PUBLIC ${Nebula3_SOURCE_DIR}/code/addons/)
TARGET_LINK_LIBRARIES(attr foundation)

SET_TARGET_PROPERTIES(attr PROPERTIES FOLDER "N3SDK/addons")

N_SET_COMPILER_DEFAULTS()

IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(attr stdneb.h stdneb.cc ../)
ENDIF()
