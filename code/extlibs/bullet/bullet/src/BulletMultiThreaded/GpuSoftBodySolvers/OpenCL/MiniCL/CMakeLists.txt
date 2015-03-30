
INCLUDE_DIRECTORIES(
${BULLET_PHYSICS_SOURCE_DIR}/src
)

ADD_DEFINITIONS(-DUSE_MINICL)




SET(BulletSoftBodyOpenCLSolvers_SRCS
	../btSoftBodySolver_OpenCL.cpp
)

SET(BulletSoftBodyOpenCLSolvers_HDRS
	../btSoftBodySolver_OpenCL.h
	../../Shared/btSoftBodySolverData.h
	../btSoftBodySolverVertexData_OpenCL.h
	../btSoftBodySolverTriangleData_OpenCL.h
	../btSoftBodySolverLinkData_OpenCL.h
	../btSoftBodySolverBuffer_OpenCL.h
)

# OpenCL and HLSL Shaders.
# Build rules generated to stringify these into headers
# which are needed by some of the sources
SET(BulletSoftBodyOpenCLSolvers_Shaders
#	OutputToVertexArray
	UpdateNormals
	Integrate
	UpdatePositions
	UpdateNodes
	SolvePositions
	UpdatePositionsFromVelocities
	ApplyForces
	PrepareLinks
	VSolveLinks
)

foreach(f ${BulletSoftBodyOpenCLSolvers_Shaders})
    LIST(APPEND BulletSoftBodyOpenCLSolvers_OpenCLC "../OpenCLC10/${f}.cl")
endforeach(f) 

# normal n3 optimization flags kill bullet
SET(CMAKE_CXX_FLAGS_RELEASE	"/Ox /FD /MT /GS- /Zi" FORCE)
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"/Ox /FD /MT /GS- /Zi" FORCE)


ADD_LIBRARY(BulletSoftBodySolvers_OpenCL_Mini
	${BulletSoftBodyOpenCLSolvers_SRCS} 
	${BulletSoftBodyOpenCLSolvers_HDRS} 
	${BulletSoftBodyOpenCLSolvers_OpenCLC}
)

SET_TARGET_PROPERTIES(BulletSoftBodySolvers_OpenCL_Mini PROPERTIES VERSION ${BULLET_VERSION})
SET_TARGET_PROPERTIES(BulletSoftBodySolvers_OpenCL_Mini PROPERTIES SOVERSION ${BULLET_VERSION})
IF (BUILD_SHARED_LIBS)
	TARGET_LINK_LIBRARIES(BulletSoftBodySolvers_OpenCL_Mini MiniCL BulletMultiThreaded BulletSoftBody)
ENDIF (BUILD_SHARED_LIBS)


# N3 Related stuff to get rid of warnings
set_property(TARGET BulletSoftBodySolvers_OpenCL_Mini APPEND PROPERTY COMPILE_DEFINITIONS    
	_CRT_NONSTDC_NO_DEPRECATE=1
    _CRT_SECURE_NO_DEPRECATE=1  
)
SET_TARGET_PROPERTIES(BulletSoftBodySolvers_OpenCL_Mini PROPERTIES FOLDER "extlibs/bullet")


IF (INSTALL_LIBS)
	IF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
		IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
			IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
				INSTALL(TARGETS BulletSoftBodySolvers_OpenCL_Mini DESTINATION .)
			ELSE (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
				INSTALL(TARGETS BulletSoftBodySolvers_OpenCL_Mini DESTINATION lib${LIB_SUFFIX})
#headers are already installed by BulletMultiThreaded library
			ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
		ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)

		IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
			SET_TARGET_PROPERTIES(BulletSoftBodySolvers_OpenCL_Mini PROPERTIES FRAMEWORK true)
			SET_TARGET_PROPERTIES(BulletSoftBodySolvers_OpenCL_Mini PROPERTIES PUBLIC_HEADER "${BulletSoftBodyOpenCLSolvers_HDRS}")
		ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	ENDIF (NOT INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES)
ENDIF (INSTALL_LIBS)
