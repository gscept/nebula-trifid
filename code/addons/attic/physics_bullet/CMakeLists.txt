#-------------------------------------------------------------------------------
# physics (bullet)
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}
					${Nebula3_SOURCE_DIR}/addons/
					${Nebula3_SOURCE_DIR}/foundation/
					${Nebula3_SOURCE_DIR}/extlibs/
					${Nebula3_SOURCE_DIR}/extlibs/bullet/bullet/src
					${Nebula3_SOURCE_DIR}/render/
					${Nebula3_SOURCE_DIR}/application/						
					${Nebula3_SOURCE_DIR}/application/basegamefeature/
					)
   

    set(SOURCES_files_physics
      
      level.cc
      level.h
      
      physicsentity.cc
      physicsentity.h
      actorphysicsentity.cc
      actorphysicsentity.h
      
      physicsserver.cc
      physicsserver.h
            
      composite.h
      composite.cc
      
      mousegripper.cc
      mousegripper.h
	  rigidbody.cc
	  rigidbody.h
	  filterset.cc
	  filterset.h
	  collidecategory.h
	  
	  conversion.h
	  
	  joint.cc
	  joint.h
	  
	  shape.cc
	  shape.h	  
	  
	  contactpoint.cc
	  contactpoint.h
	  nebulamotionstate.h
	  debugdrawer.h
	  debugdrawer.cc
	 
    )
    source_group("physics" FILES ${SOURCES_files_physics})

    set(SOURCES_physics
      ${SOURCES_files_physics}
    )

set(SOURCES_files
	stdneb.h
    ../stdneb.cc
  )
  SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
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

