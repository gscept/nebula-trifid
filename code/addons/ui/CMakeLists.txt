#-------------------------------------------------------------------------------
# ui
#-------------------------------------------------------------------------------



INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/
					${Nebula3_SOURCE_DIR}/code/addons/
					${Nebula3_SOURCE_DIR}/code/foundation/
					${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/extlibs/librocket/
					${Nebula3_SOURCE_DIR}/code/render/	
					${Nebula3_SOURCE_DIR}/code/application
					${Nebula3_SOURCE_DIR}/code/application/basegamefeature					
					${Nebula3_SOURCE_DIR}/code/application/graphicsfeature					
					)

      set(SOURCES_files_ui__base
        base/uilayoutbase.cc
        base/uilayoutbase.h
        base/uiserverbase.cc
        base/uiserverbase.h
		base/uielementbase.cc
		base/uielementbase.h		
		base/uiplugin.cc
		base/uiplugin.h		
      )
      source_group("ui\\base" FILES ${SOURCES_files_ui__base})

      set(SOURCES_ui__base
        ${SOURCES_files_ui__base}
      )

      set(SOURCES_files_ui__ceui
        ceui/ceuigeometrybuffer.cc
        ceui/ceuigeometrybuffer.h
        ceui/ceuilayout.cc
        ceui/ceuilayout.h
        ceui/ceuirawdatacontainer.cc
        ceui/ceuirawdatacontainer.h
        ceui/ceuirenderer.cc
        ceui/ceuirenderer.h
        ceui/ceuirendertarget.cc
        ceui/ceuirendertarget.h
        ceui/ceuiresourceprovider.cc
        ceui/ceuiresourceprovider.h
        ceui/ceuiserver.cc
        ceui/ceuiserver.h
        ceui/ceuitexture.cc
        ceui/ceuitexture.h
        ceui/ceuiviewporttarget.cc
        ceui/ceuiviewporttarget.h
      )
      source_group("ui\\ceui" FILES ${SOURCES_files_ui__ceui})

      set(SOURCES_ui__ceui
        ${SOURCES_files_ui__ceui}
      )
	  
	  set(SOURCES_files_ui__librocket
        rocket/rocketlayout.cc
        rocket/rocketlayout.h
		rocket/rocketeventinstancer.cc
		rocket/rocketeventinstancer.h
		rocket/rocketeventlistenerinstancer.cc
		rocket/rocketeventlistenerinstancer.h
		rocket/rocketeventlistener.h
		rocket/rocketeventlistener.cc
		rocket/rocketrenderer.cc
		rocket/rocketrenderer.h
		rocket/rocketinterface.cc
		rocket/rocketinterface.h
        rocket/rocketserver.cc
        rocket/rocketserver.h
		rocket/rocketelement.cc
		rocket/rocketelement.h
      )
      source_group("ui\\librocket" FILES ${SOURCES_files_ui__librocket})

      set(SOURCES_ui__librocket
        ${SOURCES_files_ui__librocket}
      )

    set(SOURCES_files_ui
      uievent.h
	  uielementgroup.cc
	  uielementgroup.h
      uieventhandler.cc
      uieventhandler.h
      uifeatureunit.cc
      uifeatureunit.h
      uiinputhandler.cc
      uiinputhandler.h  
	  uiserver.cc
	  uiserver.h
	  uilayout.cc
	  uilayout.h
	  uielement.cc
	  uielement.h
	  uirtplugin.cc
	  uirtplugin.h
    )
    source_group("ui" FILES ${SOURCES_files_ui})

    set(SOURCES_ui
      ${SOURCES_ui__base}
	  ${SOURCES_ui__librocket}
      ${SOURCES_files_ui}
    )

	set(SOURCES_files
		../stdneb.h
		../stdneb.cc
	)
	SOURCE_GROUP("Precompiled" FILES ${SOURCES_files})
	
	set(SOURCES_
		${SOURCES_ui}
		${SOURCES_files}		
	)
	
  N_WRAP_NIDL_FILES(ui_NIDLS uicommands.nidl uiattrs/uiattributes.nidl)

  add_library( ui STATIC ${SOURCES_} )

  ADD_DEPENDENCIES(ui ui_NIDLS)
  

  SET_TARGET_PROPERTIES(ui PROPERTIES FOLDER "N3SDK/addons")
  
  set_property(TARGET ui APPEND PROPERTY COMPILE_DEFINITIONS    
    __LIBROCKET__    
  )

  ADD_DEPENDENCIES(ui ui_NIDLS)
N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(ui stdneb.h stdneb.cc ../)
ENDIF()
