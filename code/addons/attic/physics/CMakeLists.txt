#-------------------------------------------------------------------------------
# physics (ode)
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/extlibs/
					${Nebula3_SOURCE_DIR}/extlibs/ode/
					${Nebula3_SOURCE_DIR}/extlibs/ode/ode
					${Nebula3_SOURCE_DIR}/extlibs/opcode
					${Nebula3_SOURCE_DIR}/render/	
					
					)


    set(SOURCES_files_physics
      amotor.cc
      amotor.h
      areaimpulse.cc
      areaimpulse.h
      balljoint.cc
      balljoint.h
      boxshape.cc
      boxshape.h
      capsuleshape.cc
      capsuleshape.h
      charentity.cc
      charentity.h
      collidecategory.h
      collideshapeloader.cc
      collideshapeloader.h
      composite.cc
      composite.h
      compositeloader.cc
      compositeloader.h
      contactpoint.cc
      contactpoint.h
      explosionareaimpulse.cc
      explosionareaimpulse.h
      filterset.cc
      filterset.h
      hinge2joint.cc
      hinge2joint.h
      hingejoint.cc
      hingejoint.h
      joint.cc
      joint.h
      jointaxis.h
      level.cc
      level.h
      materialtable.cc
      materialtable.h
      meshcache.cc
      meshcache.h
      meshshape.cc
      meshshape.h
      mousegripper.cc
      mousegripper.h
      physicsentity.cc
      physicsentity.h
      physicsmesh.cc
      physicsmesh.h
      physicsserver.cc
      physicsserver.h
      physicsutil.cc
      physicsutil.h
      probesphere.cc
      probesphere.h
      ray.cc
      ray.h
      rigidbody.cc
      rigidbody.h
      shape.cc
      shape.h
      sliderjoint.cc
      sliderjoint.h
      sphereshape.cc
      sphereshape.h
      stateentity.cc
      stateentity.h
      universaljoint.cc
      universaljoint.h
    )
    source_group("physics" FILES ${SOURCES_files_physics})

    set(SOURCES_physics
      ${SOURCES_files_physics}
    )

set(SOURCES_files
	stdneb.h
    ../stdneb.cc
  )
  
  set(SOURCES_SRCS
    ${SOURCES_physics}
    ${SOURCES_files}
  )


add_library( physics STATIC ${SOURCES_SRCS} )
SET_TARGET_PROPERTIES(physics PROPERTIES FOLDER "addons")

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(physics stdneb.h stdneb.cc ../)
ENDIF()
