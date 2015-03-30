SET(BULLET_VERSION 2.81)
SET(BULLET_PHYSICS_SOURCE_DIR ${Nebula3_SOURCE_DIR}/code/extlibs/bullet/bullet)
OPTION(USE_DOUBLE_PRECISION "Use double precision"	OFF)


ADD_DEFINITIONS(-DUSE_SYSTEM_VECTORMATH)
SET (VECTOR_MATH_INCLUDE ${BULLET_PHYSICS_SOURCE_DIR}/src/vectormath/sse CACHE PATH "Vector Math library include path.")

add_subdirectory(BulletSoftBody) 
add_subdirectory(BulletCollision) 
add_subdirectory(BulletDynamics) 
add_subdirectory(LinearMath )

IF(BUILD_MULTITHREADING)
	add_subdirectory(MiniCL) 
        add_subdirctory( BulletMultiThreaded)
ENDIF()

IF(INSTALL_LIBS)
	#INSTALL of other files requires CMake 2.6
	IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
		IF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
			# Don't actually need to install any common files, the frameworks include everything
		ELSE (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
			INSTALL(FILES btBulletCollisionCommon.h btBulletDynamicsCommon.h Bullet-C-Api.h DESTINATION ${INCLUDE_INSTALL_DIR})
			INSTALL(FILES vectormath/vmInclude.h DESTINATION ${INCLUDE_INSTALL_DIR}/vectormath)
			INSTALL(FILES vectormath/scalar/boolInVec.h 
					vectormath/scalar/floatInVec.h 
					vectormath/scalar/mat_aos.h 
					vectormath/scalar/quat_aos.h 
					vectormath/scalar/vec_aos.h 
					vectormath/scalar/vectormath_aos.h
				DESTINATION ${INCLUDE_INSTALL_DIR}/vectormath/scalar)
			INSTALL(FILES vectormath/sse/boolInVec.h 
					vectormath/sse/floatInVec.h 
					vectormath/sse/mat_aos.h 
					vectormath/sse/quat_aos.h 
					vectormath/sse/vec_aos.h 
					vectormath/sse/vecidx_aos.h
					vectormath/sse/vectormath_aos.h
				DESTINATION ${INCLUDE_INSTALL_DIR}/vectormath/sse)
		ENDIF (APPLE AND BUILD_SHARED_LIBS AND FRAMEWORK)
	ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
ENDIF(INSTALL_LIBS)
