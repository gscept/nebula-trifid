#-------------------------------------------------------------------------------
# Render
#-------------------------------------------------------------------------------

IF(N_BUILD_DX11)
	INCLUDE_DIRECTORIES(${N3ROOT}/extlibs/fw1)
ELSEIF(N_BUILD_OGL4)
ENDIF()

    set(Render_files_animation
      animation/animeventhandlerbase.cc
      animation/animeventhandlerbase.h
      animation/animeventinfo.h
      animation/animeventserver.cc
      animation/animeventserver.h
      animation/animjob.cc
      animation/animjob.h
      animation/animjobenqueuemode.h
      animation/animsequencer.cc
      animation/animsequencer.h
      animation/playclipjob.cc
      animation/playclipjob.h
    )
    source_group("animation" FILES ${Render_files_animation})

    set(Render_animation
      ${Render_files_animation}
    )

    set(Render_files_apprender
      apprender/renderapplication.cc
      apprender/renderapplication.h
      apprender/viewerapplication.cc
      apprender/viewerapplication.h
    )
    source_group("apprender" FILES ${Render_files_apprender})

    set(Render_apprender
      ${Render_files_apprender}
    )

	set(Render_files_materials
		materials/materialloader.cc
		materials/materialloader.h
		materials/materialpalette.cc
		materials/materialpalette.h
		materials/materialserver.cc
		materials/materialserver.h
		materials/material.cc
		materials/material.h		
		materials/materialfeature.cc
		materials/materialfeature.h		
		materials/materialtype.cc
		materials/materialtype.h
		materials/materialvariable.cc
		materials/materialvariable.h
		materials/materialvariableinstance.cc
		materials/materialvariableinstance.h
		materials/managedsurface.cc
		materials/managedsurface.h
		materials/streamsurfaceloader.cc
		materials/streamsurfaceloader.h
		materials/streamsurfacesaver.cc
		materials/streamsurfacesaver.h
		materials/surface.cc
		materials/surface.h
		materials/surfaceinstance.cc
		materials/surfaceinstance.h
		materials/surfaceconstant.cc
		materials/surfaceconstant.h
		#materials/surfaceconstantinstance.cc
		#materials/surfaceconstantinstance.h
		materials/surfacename.cc
		materials/surfacename.h
		)
	source_group("materials" FILES ${Render_files_materials})
	set(Render_materials ${Render_files_materials})

	  set(Render_files_characters__base
		characters/base/skinnedmeshdrawinfobase.h
		characters/base/skinnedmeshrendererbase.cc
		characters/base/skinnedmeshrendererbase.h
	  )
	  source_group("characters\\base" FILES ${Render_files_characters__base})

	  set(Render_characters__base
		${Render_files_characters__base}
	  )

      set(Render_files_characters__gpu
        characters/gpuskinnedmeshrenderer.cc
        characters/gpuskinnedmeshrenderer.h
      )
      source_group("characters" FILES ${Render_files_characters__gpu})

      set(Render_characters__gpu
        ${Render_files_characters__gpu}
      )


    set(Render_files_characters
      characters/character.cc
      characters/character.h
      characters/characteranimationcontroller.cc
      characters/characteranimationcontroller.h
      characters/characteranimationlibrary.cc
      characters/characteranimationlibrary.h
      characters/characterinstance.cc
      characters/characterinstance.h
      characters/characterjoint.cc
      characters/characterjoint.h
	  characters/characterjointmask.cc
      characters/characterjointmask.h
	  characters/charjointinfo.h
      characters/characternode.cc
      characters/characternode.h
      characters/characternodeinstance.cc
      characters/characternodeinstance.h
      characters/characterserver.cc
      characters/characterserver.h
      characters/characterskeleton.cc
      characters/characterskeleton.h
      characters/characterskeletoninstance.cc
      characters/characterskeletoninstance.h
      characters/characterskin.cc
      characters/characterskin.h
      characters/characterskinlibrary.cc
      characters/characterskinlibrary.h
      characters/characterskinlist.cc
      characters/characterskinlist.h
      characters/characterskinnode.cc
      characters/characterskinnode.h
      characters/characterskinnodeinstance.cc
      characters/characterskinnodeinstance.h
      characters/characterskinset.cc
      characters/characterskinset.h
      characters/charactervariationlibrary.cc
      characters/charactervariationlibrary.h
      characters/charactervariationset.cc
      characters/charactervariationset.h
      characters/charjointcomponents.h
      characters/skinnedmeshdrawinfo.h
      characters/skinnedmeshrenderer.cc
      characters/skinnedmeshrenderer.h
      characters/skinningtechnique.h
    )
    source_group("characters" FILES ${Render_files_characters})

    set(Render_characters
      ${Render_characters__base}
      ${Render_characters__gpu}
      ${Render_files_characters}
    )

	set(Render_files_billboards
		billboards/billboardnode.cc
		billboards/billboardnode.h
		billboards/billboardnodeinstance.cc
		billboards/billboardnodeinstance.h
	)
	source_group("billboards" FILES ${Render_files_billboards})

	set(Render_billboards
		${Render_files_billboards}
	)

    set(Render_files_coreanimation
      coreanimation/animclip.cc
      coreanimation/animclip.h
      coreanimation/animcurve.h
      coreanimation/animevent.h
      coreanimation/animeventemitter.cc
      coreanimation/animeventemitter.h
      coreanimation/animkeybuffer.cc
      coreanimation/animkeybuffer.h
      coreanimation/animresource.cc
      coreanimation/animresource.h
      coreanimation/animsamplebuffer.cc
      coreanimation/animsamplebuffer.h
      coreanimation/animsamplemixinfo.h
      coreanimation/animutil.cc
      coreanimation/animutil.h
      coreanimation/curvetype.cc
      coreanimation/curvetype.h
      coreanimation/infinitytype.cc
      coreanimation/infinitytype.h
      coreanimation/managedanimresource.cc
      coreanimation/managedanimresource.h
      coreanimation/naxfileformatstructs.h
      coreanimation/sampletype.cc
      coreanimation/sampletype.h
      coreanimation/streamanimationloader.cc
      coreanimation/streamanimationloader.h
    )
    source_group("coreanimation" FILES ${Render_files_coreanimation})

    set(Render_coreanimation
      ${Render_files_coreanimation}
    )

      set(Render_files_coregraphics__base
		coregraphics/base/bufferbase.cc
		coregraphics/base/bufferbase.h
		coregraphics/base/bufferlockbase.cc
		coregraphics/base/bufferlockbase.h
		coregraphics/base/constantbufferbase.cc
		coregraphics/base/constantbufferbase.h
        coregraphics/base/displaydevicebase.cc
        coregraphics/base/displaydevicebase.h
		coregraphics/base/depthstenciltargetbase.cc
		coregraphics/base/depthstenciltargetbase.h
		coregraphics/base/feedbackbufferbase.cc
		coregraphics/base/feedbackbufferbase.h
        coregraphics/base/indexbufferbase.cc
        coregraphics/base/indexbufferbase.h
        coregraphics/base/memoryindexbufferloaderbase.cc
        coregraphics/base/memoryindexbufferloaderbase.h
        coregraphics/base/memoryvertexbufferloaderbase.cc
        coregraphics/base/memoryvertexbufferloaderbase.h
        coregraphics/base/meshbase.cc
        coregraphics/base/meshbase.h
        coregraphics/base/mouserenderdevicebase.cc
        coregraphics/base/mouserenderdevicebase.h
        coregraphics/base/multiplerendertargetbase.cc
        coregraphics/base/multiplerendertargetbase.h
        coregraphics/base/renderdevicebase.cc
        coregraphics/base/renderdevicebase.h
		coregraphics/base/rendertargetcubebase.cc
        coregraphics/base/rendertargetcubebase.h
        coregraphics/base/rendertargetbase.cc
        coregraphics/base/rendertargetbase.h
        coregraphics/base/resourcebase.cc
        coregraphics/base/resourcebase.h
        coregraphics/base/shaderbase.cc
      	coregraphics/base/shaderbase.h
		coregraphics/base/shaderreadwritebufferbase.cc
		coregraphics/base/shaderreadwritebufferbase.h
		coregraphics/base/shaderreadwritetexturebase.cc
		coregraphics/base/shaderreadwritetexturebase.h
        coregraphics/base/shaderstatebase.cc
        coregraphics/base/shaderstatebase.h
        coregraphics/base/shaderserverbase.cc
        coregraphics/base/shaderserverbase.h
        coregraphics/base/shadervariablebase.cc
        coregraphics/base/shadervariablebase.h
        coregraphics/base/shadervariableinstancebase.cc
        coregraphics/base/shadervariableinstancebase.h
        coregraphics/base/shadervariationbase.cc
        coregraphics/base/shadervariationbase.h
        coregraphics/base/shaperendererbase.cc
        coregraphics/base/shaperendererbase.h
        coregraphics/base/streamtexturesaverbase.cc
        coregraphics/base/streamtexturesaverbase.h
        coregraphics/base/textrendererbase.cc
        coregraphics/base/textrendererbase.h
        coregraphics/base/texturebase.cc
        coregraphics/base/texturebase.h
        coregraphics/base/transformdevicebase.cc
        coregraphics/base/transformdevicebase.h
        coregraphics/base/vertexbufferbase.cc
        coregraphics/base/vertexbufferbase.h
        coregraphics/base/vertexcomponentbase.h
        coregraphics/base/vertexlayoutbase.cc
        coregraphics/base/vertexlayoutbase.h
        coregraphics/base/vertexlayoutserverbase.cc
        coregraphics/base/vertexlayoutserverbase.h
      )
      source_group("coregraphics\\base" FILES ${Render_files_coregraphics__base})

      set(Render_coregraphics__base
        ${Render_files_coregraphics__base}
      )

      IF(N_BUILD_DX11)
      set(Render_files_coregraphics__d3d11
        coregraphics/d3d11/d3d11displaydevice.cc
        coregraphics/d3d11/d3d11displaydevice.h
		coregraphics/d3d11/d3d11depthstenciltarget.cc
		coregraphics/d3d11/d3d11depthstenciltarget.h
        coregraphics/d3d11/d3d11indexbuffer.cc
        coregraphics/d3d11/d3d11indexbuffer.h
        coregraphics/d3d11/d3d11memoryindexbufferloader.cc
        coregraphics/d3d11/d3d11memoryindexbufferloader.h
        coregraphics/d3d11/d3d11memoryvertexbufferloader.cc
        coregraphics/d3d11/d3d11memoryvertexbufferloader.h
		coregraphics/d3d11/d3d11memorytextureloader.cc
    	coregraphics/d3d11/d3d11memorytextureloader.h
		coregraphics/d3d11/d3d11multiplerendertarget.h
		coregraphics/d3d11/d3d11multiplerendertarget.cc
        coregraphics/d3d11/d3d11renderdevice.cc
        coregraphics/d3d11/d3d11renderdevice.h
		coregraphics/d3d11/d3d11rendertargetcube.cc
        coregraphics/d3d11/d3d11rendertargetcube.h
        coregraphics/d3d11/d3d11rendertarget.cc
        coregraphics/d3d11/d3d11rendertarget.h
        coregraphics/d3d11/d3d11shader.cc
        coregraphics/d3d11/d3d11shader.h
        coregraphics/d3d11/d3d11shaderstate.cc
        coregraphics/d3d11/d3d11shaderstate.h
        coregraphics/d3d11/d3d11shaderserver.cc
        coregraphics/d3d11/d3d11shaderserver.h
        coregraphics/d3d11/d3d11shadervariable.cc
        coregraphics/d3d11/d3d11shadervariable.h
        coregraphics/d3d11/d3d11shadervariation.cc
        coregraphics/d3d11/d3d11shadervariation.h
        coregraphics/d3d11/d3d11shaperenderer.cc
        coregraphics/d3d11/d3d11shaperenderer.h
        coregraphics/d3d11/d3d11streammeshloader.cc
        coregraphics/d3d11/d3d11streammeshloader.h
        coregraphics/d3d11/d3d11streamshaderloader.cc
        coregraphics/d3d11/d3d11streamshaderloader.h
        coregraphics/d3d11/d3d11streamtextureloader.cc
        coregraphics/d3d11/d3d11streamtextureloader.h
        coregraphics/d3d11/d3d11streamtexturesaver.cc
        coregraphics/d3d11/d3d11streamtexturesaver.h
        coregraphics/d3d11/d3d11textrenderer.cc
        coregraphics/d3d11/d3d11textrenderer.h
        coregraphics/d3d11/d3d11texture.cc
        coregraphics/d3d11/d3d11texture.h
        coregraphics/d3d11/d3d11transformdevice.cc
        coregraphics/d3d11/d3d11transformdevice.h
        coregraphics/d3d11/d3d11types.cc
        coregraphics/d3d11/d3d11types.h
        coregraphics/d3d11/d3d11vertexbuffer.cc
        coregraphics/d3d11/d3d11vertexbuffer.h
        coregraphics/d3d11/d3d11vertexlayout.cc
        coregraphics/d3d11/d3d11vertexlayout.h
      )
      source_group("coregraphics\\d3d11" FILES ${Render_files_coregraphics__d3d11})


      set(Render_coregraphics__imp
        ${Render_files_coregraphics__d3d11}
      )
      ELSEIF(N_BUILD_DX9)
      set(Render_files_coregraphics__d3d9
        coregraphics/d3d9/d3d9displaydevice.cc
        coregraphics/d3d9/d3d9displaydevice.h
        coregraphics/d3d9/d3d9renderdevice.cc
        coregraphics/d3d9/d3d9renderdevice.h
        coregraphics/d3d9/d3d9rendertarget.cc
        coregraphics/d3d9/d3d9rendertarget.h
        coregraphics/d3d9/d3d9shader.cc
        coregraphics/d3d9/d3d9shader.h
        coregraphics/d3d9/d3d9shaderstate.cc
        coregraphics/d3d9/d3d9shaderstate.h
        coregraphics/d3d9/d3d9shaderserver.cc
        coregraphics/d3d9/d3d9shaderserver.h
        coregraphics/d3d9/d3d9shadervariable.cc
        coregraphics/d3d9/d3d9shadervariable.h
        coregraphics/d3d9/d3d9shadervariation.cc
        coregraphics/d3d9/d3d9shadervariation.h
        coregraphics/d3d9/d3d9streamshaderloader.cc
        coregraphics/d3d9/d3d9streamshaderloader.h
        coregraphics/d3d9/d3d9streamtextureloader.cc
        coregraphics/d3d9/d3d9streamtextureloader.h
        coregraphics/d3d9/d3d9textrenderer.cc
        coregraphics/d3d9/d3d9textrenderer.h
        coregraphics/d3d9/d3d9texture.cc
        coregraphics/d3d9/d3d9texture.h
        coregraphics/d3d9/d3d9indexbuffer.cc
        coregraphics/d3d9/d3d9indexbuffer.h
        coregraphics/d3d9/d3d9memoryindexbufferloader.cc
        coregraphics/d3d9/d3d9memoryindexbufferloader.h
        coregraphics/d3d9/d3d9memoryvertexbufferloader.cc
        coregraphics/d3d9/d3d9memoryvertexbufferloader.h
		coregraphics/d3d9/d3d9memorytextureloader.cc
    	coregraphics/d3d9/d3d9memorytextureloader.h
        coregraphics/d3d9/d3d9shaperenderer.cc
        coregraphics/d3d9/d3d9shaperenderer.h
        coregraphics/d3d9/d3d9streammeshloader.cc
        coregraphics/d3d9/d3d9streammeshloader.h
        coregraphics/d3d9/d3d9streamtexturesaver.cc
        coregraphics/d3d9/d3d9streamtexturesaver.h
        coregraphics/d3d9/d3d9transformdevice.cc
        coregraphics/d3d9/d3d9transformdevice.h
        coregraphics/d3d9/d3d9types.cc
        coregraphics/d3d9/d3d9types.h
        coregraphics/d3d9/d3d9vertexbuffer.cc
        coregraphics/d3d9/d3d9vertexbuffer.h
        coregraphics/d3d9/d3d9vertexlayout.cc
        coregraphics/d3d9/d3d9vertexlayout.h
      )
      source_group("coregraphics\\d3d9" FILES ${Render_files_coregraphics__d3d9})

      set(Render_coregraphics__imp
        ${Render_files_coregraphics__d3d9}
      )
      ELSEIF(N_BUILD_OGL4)
	  set(Render_files_coregraphics_displaydevice__impl
			coregraphics/glfw/glfwdisplaydevice.cc
			coregraphics/glfw/glfwdisplaydevice.h)			
      set(Render_files_coregraphics__ogl4
		${Render_files_coregraphics_displaydevice__impl}
		coregraphics/ogl4/ogl4bufferlock.cc
		coregraphics/ogl4/ogl4bufferlock.h
		coregraphics/ogl4/ogl4depthstenciltarget.cc
		coregraphics/ogl4/ogl4depthstenciltarget.h
		coregraphics/ogl4/ogl4feedbackbuffer.cc
		coregraphics/ogl4/ogl4feedbackbuffer.h
        coregraphics/ogl4/ogl4indexbuffer.cc
        coregraphics/ogl4/ogl4indexbuffer.h
        coregraphics/ogl4/ogl4memoryindexbufferloader.cc
        coregraphics/ogl4/ogl4memoryindexbufferloader.h
        coregraphics/ogl4/ogl4memoryvertexbufferloader.cc
        coregraphics/ogl4/ogl4memoryvertexbufferloader.h
    	coregraphics/ogl4/ogl4memorytextureloader.cc
		coregraphics/ogl4/ogl4memorytextureloader.h
		coregraphics/ogl4/ogl4multiplerendertarget.cc
        coregraphics/ogl4/ogl4multiplerendertarget.h
        coregraphics/ogl4/ogl4renderdevice.cc
        coregraphics/ogl4/ogl4renderdevice.h
		coregraphics/ogl4/ogl4rendertargetcube.cc
        coregraphics/ogl4/ogl4rendertargetcube.h
        coregraphics/ogl4/ogl4rendertarget.cc
        coregraphics/ogl4/ogl4rendertarget.h
        coregraphics/ogl4/ogl4shader.cc
        coregraphics/ogl4/ogl4shader.h
		coregraphics/ogl4/ogl4shaderstoragebuffer.cc
		coregraphics/ogl4/ogl4shaderstoragebuffer.h
		coregraphics/ogl4/ogl4shaderimage.cc
        coregraphics/ogl4/ogl4shaderimage.h
        coregraphics/ogl4/ogl4shaderstate.cc
        coregraphics/ogl4/ogl4shaderstate.h
        coregraphics/ogl4/ogl4shaderserver.cc
        coregraphics/ogl4/ogl4shaderserver.h
        coregraphics/ogl4/ogl4shadervariable.cc
        coregraphics/ogl4/ogl4shadervariable.h
		coregraphics/ogl4/ogl4shadervariableinstance.cc
        coregraphics/ogl4/ogl4shadervariableinstance.h
        coregraphics/ogl4/ogl4shaderprogram.cc
        coregraphics/ogl4/ogl4shaderprogram.h
        coregraphics/ogl4/ogl4shaperenderer.cc
        coregraphics/ogl4/ogl4shaperenderer.h
        coregraphics/ogl4/ogl4streammeshloader.cc
        coregraphics/ogl4/ogl4streammeshloader.h
        coregraphics/ogl4/ogl4streamshaderloader.cc
        coregraphics/ogl4/ogl4streamshaderloader.h
        coregraphics/ogl4/ogl4streamtextureloader.cc
        coregraphics/ogl4/ogl4streamtextureloader.h
        coregraphics/ogl4/ogl4streamtexturesaver.cc
        coregraphics/ogl4/ogl4streamtexturesaver.h
        coregraphics/ogl4/ogl4textrenderer.cc
        coregraphics/ogl4/ogl4textrenderer.h
        coregraphics/ogl4/ogl4texture.cc
        coregraphics/ogl4/ogl4texture.h
        coregraphics/ogl4/ogl4transformdevice.cc
        coregraphics/ogl4/ogl4transformdevice.h
        coregraphics/ogl4/ogl4types.cc
        coregraphics/ogl4/ogl4types.h
		coregraphics/ogl4/ogl4uniformbuffer.h
		coregraphics/ogl4/ogl4uniformbuffer.cc
        coregraphics/ogl4/ogl4vertexbuffer.cc
        coregraphics/ogl4/ogl4vertexbuffer.h
        coregraphics/ogl4/ogl4vertexlayoutserver.cc
        coregraphics/ogl4/ogl4vertexlayoutserver.h
        coregraphics/ogl4/ogl4vertexlayout.cc
        coregraphics/ogl4/ogl4vertexlayout.h
        #coregraphics/ogl4/glfwdisplaydevice.cc
        #coregraphics/ogl4/glfwdisplaydevice.h
      )
      
      source_group("coregraphics\\ogl4" FILES ${Render_files_coregraphics__ogl4})


      set(Render_coregraphics__imp
        ${Render_files_coregraphics__ogl4}
      )
	  ELSEIF(N_BUILD_VULKAN)
	  set(Render_files_coregraphics_displaydevice__impl
			coregraphics/glfw/glfwdisplaydevice.cc
			coregraphics/glfw/glfwdisplaydevice.h
			coregraphics/vk/vkdisplaydevice.cc
			coregraphics/vk/vkdisplaydevice.h)
	  set(Render_files_coregraphics__vk
		${Render_files_coregraphics_displaydevice__impl}
		coregraphics/vk/vkcmdbufferthread.cc
		coregraphics/vk/vkcmdbufferthread.h
		coregraphics/vk/vkcpugpuinterlockthread.cc
		coregraphics/vk/vkcpugpuinterlockthread.h
		coregraphics/vk/vkcpusyncfence.cc
		coregraphics/vk/vkcpusyncfence.h
		coregraphics/vk/vkdeferredcommand.cc
		coregraphics/vk/vkdeferredcommand.h
		coregraphics/vk/vkdepthstenciltarget.cc
		coregraphics/vk/vkdepthstenciltarget.h
		coregraphics/vk/vkfeedbackbuffer.cc
		coregraphics/vk/vkfeedbackbuffer.h
        coregraphics/vk/vkindexbuffer.cc
        coregraphics/vk/vkindexbuffer.h
        coregraphics/vk/vkmemoryindexbufferloader.cc
        coregraphics/vk/vkmemoryindexbufferloader.h
        coregraphics/vk/vkmemoryvertexbufferloader.cc
        coregraphics/vk/vkmemoryvertexbufferloader.h
    	coregraphics/vk/vkmemorytextureloader.cc
		coregraphics/vk/vkmemorytextureloader.h
		coregraphics/vk/vkmultiplerendertarget.cc
        coregraphics/vk/vkmultiplerendertarget.h
        coregraphics/vk/vkrenderdevice.cc
        coregraphics/vk/vkrenderdevice.h
		coregraphics/vk/vkrendertargetcube.cc
        coregraphics/vk/vkrendertargetcube.h
        coregraphics/vk/vkrendertarget.cc
        coregraphics/vk/vkrendertarget.h
        coregraphics/vk/vkshader.cc
        coregraphics/vk/vkshader.h
		coregraphics/vk/vkshaderstoragebuffer.cc
		coregraphics/vk/vkshaderstoragebuffer.h
		coregraphics/vk/vkshaderimage.cc
        coregraphics/vk/vkshaderimage.h
        coregraphics/vk/vkshaderstate.cc
        coregraphics/vk/vkshaderstate.h
        coregraphics/vk/vkshaderserver.cc
        coregraphics/vk/vkshaderserver.h
        coregraphics/vk/vkshadervariable.cc
        coregraphics/vk/vkshadervariable.h
		coregraphics/vk/vkshadervariableinstance.cc
        coregraphics/vk/vkshadervariableinstance.h
        coregraphics/vk/vkshaderprogram.cc
        coregraphics/vk/vkshaderprogram.h
        coregraphics/vk/vkshaperenderer.cc
        coregraphics/vk/vkshaperenderer.h
        coregraphics/vk/vkstreammeshloader.cc
        coregraphics/vk/vkstreammeshloader.h
        coregraphics/vk/vkstreamshaderloader.cc
        coregraphics/vk/vkstreamshaderloader.h
        coregraphics/vk/vkstreamtextureloader.cc
        coregraphics/vk/vkstreamtextureloader.h
        coregraphics/vk/vkstreamtexturesaver.cc
        coregraphics/vk/vkstreamtexturesaver.h
        coregraphics/vk/vktextrenderer.cc
        coregraphics/vk/vktextrenderer.h
        coregraphics/vk/vktexture.cc
        coregraphics/vk/vktexture.h
        coregraphics/vk/vktransformdevice.cc
        coregraphics/vk/vktransformdevice.h
        coregraphics/vk/vktypes.cc
        coregraphics/vk/vktypes.h
		coregraphics/vk/vkuniformbuffer.h
		coregraphics/vk/vkuniformbuffer.cc
        coregraphics/vk/vkvertexbuffer.cc
        coregraphics/vk/vkvertexbuffer.h
        coregraphics/vk/vkvertexlayoutserver.cc
        coregraphics/vk/vkvertexlayoutserver.h
        coregraphics/vk/vkvertexlayout.cc
        coregraphics/vk/vkvertexlayout.h
        #coregraphics/ogl4/glfwdisplaydevice.cc
        #coregraphics/ogl4/glfwdisplaydevice.h
      )
	  
	  source_group("coregraphics\\vk" FILES ${Render_files_coregraphics__vk})

      set(Render_coregraphics__imp
        ${Render_files_coregraphics__vk}
      )
      ENDIF()

      set(Render_files_coregraphics__debug
        coregraphics/debug/displaypagehandler.cc
        coregraphics/debug/displaypagehandler.h
        coregraphics/debug/meshpagehandler.cc
        coregraphics/debug/meshpagehandler.h
        coregraphics/debug/shaderpagehandler.cc
        coregraphics/debug/shaderpagehandler.h
        coregraphics/debug/texturepagehandler.cc
        coregraphics/debug/texturepagehandler.h
      )
      source_group("coregraphics\\debug" FILES ${Render_files_coregraphics__debug})

      set(Render_coregraphics__debug
        ${Render_files_coregraphics__debug}
      )

      set(Render_files_coregraphics__legacy
        coregraphics/legacy/nvx2streamreader.cc
        coregraphics/legacy/nvx2streamreader.h
		coregraphics/legacy/nvx2fileformatstructs.h
      )
      source_group("coregraphics\\legacy" FILES ${Render_files_coregraphics__legacy})

      set(Render_coregraphics__legacy
        ${Render_files_coregraphics__legacy}
      )

	  # temporarily use GLFW instead of win32displaydevice, so set this to false
      IF(N_BUILD_DX11 OR N_BUILD_DX9)
		set(Render_files_coregraphics__win32
		coregraphics/win32/win32displaydevice.cc
		coregraphics/win32/win32displaydevice.h
			)
		source_group("coregraphics\\win32" FILES ${Render_files_coregraphics__win32})
		  
		set(Render_coregraphics__win32
			${Render_files_coregraphics__win32}
		)
		
	  ENDIF()

    set(Render_files_coregraphics
      coregraphics/adapter.cc
      coregraphics/adapter.h
      coregraphics/adapterinfo.h
      coregraphics/antialiasquality.cc
      coregraphics/antialiasquality.h
      coregraphics/framebatchtype.h
	  coregraphics/bufferlock.cc
	  coregraphics/bufferlock.h
      coregraphics/config.h
	  coregraphics/constantbuffer.cc
	  coregraphics/constantbuffer.h
      coregraphics/displaydevice.cc
      coregraphics/displaydevice.h
      coregraphics/displayevent.h
      coregraphics/displayeventhandler.cc
      coregraphics/displayeventhandler.h
      coregraphics/displaymode.h
	  coregraphics/depthstenciltarget.cc
	  coregraphics/depthstenciltarget.h
	  coregraphics/feedbackbuffer.cc
	  coregraphics/feedbackbuffer.h
      coregraphics/imagefileformat.cc
      coregraphics/imagefileformat.h
      coregraphics/indexbuffer.cc
      coregraphics/indexbuffer.h
      coregraphics/indextype.h
      coregraphics/memoryindexbufferloader.cc
      coregraphics/memoryindexbufferloader.h
      coregraphics/memorymeshloader.cc
      coregraphics/memorymeshloader.h
	  coregraphics/memorytextureloader.cc
      coregraphics/memorytextureloader.h
      coregraphics/memoryvertexbufferloader.cc
      coregraphics/memoryvertexbufferloader.h
      coregraphics/mesh.cc
      coregraphics/mesh.h
      coregraphics/mousepointer.h
      coregraphics/mouserenderdevice.cc
      coregraphics/mouserenderdevice.h
      coregraphics/multiplerendertarget.cc
      coregraphics/multiplerendertarget.h
      coregraphics/pixelformat.cc
      coregraphics/pixelformat.h
      coregraphics/primitivegroup.h
      coregraphics/primitivetopology.cc
      coregraphics/primitivetopology.h
      coregraphics/renderdevice.cc
      coregraphics/renderdevice.h
      coregraphics/renderevent.h
      coregraphics/rendereventhandler.cc
      coregraphics/rendereventhandler.h
      coregraphics/rendershape.cc
      coregraphics/rendershape.h
	  coregraphics/rendertargetcube.cc
      coregraphics/rendertargetcube.h
      coregraphics/rendertarget.cc
      coregraphics/rendertarget.h
      coregraphics/shader.cc
      coregraphics/shader.h
	  coregraphics/shaderreadwritebuffer.cc
	  coregraphics/shaderreadwritebuffer.h
	  coregraphics/shaderreadwritetexture.cc
	  coregraphics/shaderreadwritetexture.h
      coregraphics/shaderfeature.cc
      coregraphics/shaderfeature.h
	  coregraphics/shaderidentifier.cc
	  coregraphics/shaderidentifier.h
      coregraphics/shaderstate.cc
      coregraphics/shaderstate.h
      coregraphics/shadersemantics.h
      coregraphics/shaderserver.cc
      coregraphics/shaderserver.h
      coregraphics/shadervariable.cc
      coregraphics/shadervariable.h
      coregraphics/shadervariableinstance.cc
      coregraphics/shadervariableinstance.h
      coregraphics/shadervariation.cc
      coregraphics/shadervariation.h
      coregraphics/shaperenderer.cc
      coregraphics/shaperenderer.h
      coregraphics/streammeshloader.cc
      coregraphics/streammeshloader.h
      coregraphics/streamshaderloader.cc
      coregraphics/streamshaderloader.h
      coregraphics/streamtextureloader.cc
      coregraphics/streamtextureloader.h
      coregraphics/streamtexturesaver.cc
      coregraphics/streamtexturesaver.h
      coregraphics/textelement.cc
      coregraphics/textelement.h
      coregraphics/textrenderer.cc
      coregraphics/textrenderer.h
      coregraphics/texture.cc
      coregraphics/texture.h
      coregraphics/threadsafedisplayeventhandler.cc
      coregraphics/threadsafedisplayeventhandler.h
      coregraphics/threadsaferendereventhandler.cc
      coregraphics/threadsaferendereventhandler.h
      coregraphics/transformdevice.cc
      coregraphics/transformdevice.h
      coregraphics/vertexbuffer.cc
      coregraphics/vertexbuffer.h
      coregraphics/vertexcomponent.h
      coregraphics/vertexlayout.cc
      coregraphics/vertexlayout.h
      coregraphics/vertexlayoutserver.cc
      coregraphics/vertexlayoutserver.h
    )
    source_group("coregraphics" FILES ${Render_files_coregraphics})

    set(Render_coregraphics
      ${Render_coregraphics__base}
      ${Render_coregraphics__imp}
      ${Render_coregraphics__debug}
      ${Render_coregraphics__legacy}
      ${Render_coregraphics__win32}
      ${Render_files_coregraphics}
    )

	set(Render_files_algorithm
		algorithm/algorithms.h
		algorithm/algorithmbase.h
		algorithm/algorithmbase.cc
		algorithm/algorithmhandler.h
		algorithm/algorithmhandler.cc
		algorithm/aoalgorithm.h
		algorithm/aoalgorithm.cc
		algorithm/ssralgorithm.h
		algorithm/ssralgorithm.cc
		algorithm/volumetriclightalgorithm.h
		algorithm/volumetriclightalgorithm.cc
		algorithm/tonemappingalgorithm.h
		algorithm/tonemappingalgorithm.cc
	)
	source_group("algorithm" FILES ${Render_files_algorithm})

	set(Render_algorithm
		${Render_files_algorithm}
	)

    set(Render_files_debugrender
      debugrender/debuggraphicshandler.cc
      debugrender/debuggraphicshandler.h
      debugrender/debugrender.h
      debugrender/debugshaperenderer.cc
      debugrender/debugshaperenderer.h
      debugrender/debugtextrenderer.cc
      debugrender/debugtextrenderer.h
      debugrender/renderconsolehandler.cc
      debugrender/renderconsolehandler.h
    )
    source_group("debugrender" FILES ${Render_files_debugrender})

    set(Render_debugrender
      ${Render_files_debugrender}
    )

	SET(Render_files_events
		events/event.h
		)
	SOURCE_GROUP("events" FILES ${Render_files_events})

	IF(WIN32)
	SET(Render_files_events_imp
		events/win32/win32event.h
		events/win32/win32event.cc
	)
	SOURCE_GROUP("events\\win32" FILES ${Render_files_events_imp})
	ELSEIF(UNIX)
	SET(Render_files_events_imp
		events/posix/posixevent.h
		events/posix/posixevent.cc
	)
	SOURCE_GROUP("events\\posix" FILES ${Render_files_events_imp})
	ENDIF()

	SET(Render_events
		${Render_files_events}
		${Render_files_events_imp}
	)

    set(Render_files_frame
	  frame/batchgroup.cc
	  frame/batchgroup.h
	  frame/framealgorithm.cc
	  frame/framealgorithm.h
	  frame/framecompute.cc
	  frame/framecompute.h
      frame/framebatch.cc
      frame/framebatch.h
	  frame/frameexternal.cc
      frame/frameexternal.h
      frame/framepass.cc
      frame/framepass.h
      frame/framepassbase.cc
      frame/framepassbase.h
      frame/frameposteffect.cc
      frame/frameposteffect.h
	  frame/framerendertargetblit.cc
	  frame/framerendertargetblit.h
      frame/frameserver.cc
      frame/frameserver.h
      frame/frameshader.cc
      frame/frameshader.h
      frame/frameshaderloader.cc
      frame/frameshaderloader.h
	  frame/frametexclear.cc
	  frame/frametexclear.h
      frame/lightingmode.cc
      frame/lightingmode.h
      frame/sortingmode.cc
      frame/sortingmode.h
    )
    source_group("frame" FILES ${Render_files_frame})

    set(Render_frame
      ${Render_files_frame}
    )
	
	set(Render_files_graphics__debug
		graphics/debug/graphicspagehandler.cc
		graphics/debug/graphicspagehandler.h
	)
	source_group("graphics\\debug" FILES ${Render_files_graphics__debug})

	set(Render_graphics__debug
		${Render_files_graphics__debug}
	)

	set(Render_files_graphics__handlers
		graphics/handlers/cameraentityhandler.cc
		graphics/handlers/graphicsentityhandler.cc
		graphics/handlers/billboardentityhandler.cc
		graphics/handlers/staticgraphicshandler.cc
		graphics/handlers/modelentityhandler.cc
	)
	source_group("graphics\\handlers" FILES ${Render_files_graphics__handlers})

	set(Render_graphics__handlers
		${Render_files_graphics__handlers}
	)

    set(Render_files_graphics
      graphics/abstractlightentity.cc
      graphics/abstractlightentity.h	  
      graphics/cameraentity.cc
      graphics/cameraentity.h
	  graphics/camerasettings.cc
      graphics/camerasettings.h
      graphics/display.cc
      graphics/display.h
      graphics/displaysettings.cc
      graphics/displaysettings.h
	  graphics/billboardentity.cc
	  graphics/billboardentity.h
      graphics/globallightentity.cc
      graphics/globallightentity.h
      graphics/graphicsentity.cc
      graphics/graphicsentity.h
      graphics/graphicsentitytype.h
      graphics/graphicshandler.cc
      graphics/graphicshandler.h
      graphics/graphicsserver.cc
      graphics/graphicsserver.h
	  graphics/graphicsinterface.cc
	  graphics/graphicsinterface.h
	  graphics/lightprobeentity.cc
	  graphics/lightprobeentity.h
      graphics/modelentity.cc
      graphics/modelentity.h
      graphics/mouserenderer.cc
      graphics/mouserenderer.h
      graphics/pointlightentity.cc
      graphics/pointlightentity.h
      graphics/spotlightentity.cc
      graphics/spotlightentity.h
      graphics/stage.cc
      graphics/stage.h
      graphics/stagebuilders.h
      graphics/view.cc
      graphics/view.h
    )
    source_group("graphics" FILES ${Render_files_graphics})

    set(Render_graphics
      ${Render_files_graphics}
	  ${Render_graphics__debug}
	  ${Render_graphics__handlers}
    )

	set(Render_files_image
		image/imagefactory.cc
		image/imagefactory.h
		image/imagebase.cc
		image/imagebase.h
		image/ddsimage.cc
		image/ddsimage.h
		image/bitmapimage.cc
		image/bitmapimage.h
		image/jpegimage.cc
		image/jpegimage.h
		image/pngimage.cc
		image/pngimage.h
	)
	source_group("image" FILES ${Render_files_image})

	set(Render_image
		${Render_files_image}
	)

      set(Render_files_input__base
        input/base/gamepadbase.cc
        input/base/gamepadbase.h
        input/base/inputserverbase.cc
        input/base/inputserverbase.h
        input/base/keyboardbase.cc
        input/base/keyboardbase.h
        input/base/mousebase.cc
        input/base/mousebase.h
      )
      source_group("input\\base" FILES ${Render_files_input__base})

      set(Render_input__base
        ${Render_files_input__base}
      )
	  
	set(Render_files_input__xinput
		input/xinput/xinputgamepad.cc
		input/xinput/xinputgamepad.h
		)
		source_group("input\\xinput" FILES ${Render_files_input__xinput})

	IF(WIN32)
	set(Render_input__xinput
	#	${Render_files_input__xinput}
		)
	ENDIF()

	  IF(N_BUILD_OGL4 OR N_BUILD_VULKAN)
	  set(Render_files_input_platform 
			input/glfw/glfwinputdisplayeventhandler.cc
			input/glfw/glfwinputdisplayeventhandler.h
			input/glfw/glfwinputserver.h
			input/glfw/glfwinputserver.cc
			${Render_input__xinput}
			)
			source_group("input\\glfw" FILES ${Render_files_input_platform})
	   ELSEIF(N_BUILD_DX11 OR N_BUILD_DX9)
	   set(Render_files_input__win32
			input/win32/win32inputdisplayeventhandler.cc
			input/win32/win32inputdisplayeventhandler.h
			input/win32/win32inputserver.h
			input/win32/win32inputserver.cc
			input/win32/win32mouse.h
			input/win32/win32mouse.cc
			)
			source_group("input\\win32" FILES ${Render_files_input__win32})
			
	   set(Render_input__win32
			${Render_input__xinput}
			${Render_files_input__win32}
			)	   		
	   ENDIF()	   
    set(Render_files_input
      input/char.h
      input/gamepad.cc
      input/gamepad.h
      input/inputevent.h
      input/inputhandler.cc
      input/inputhandler.h
      input/inputpriority.h
      input/inputserver.cc
      input/inputserver.h
      input/key.cc
      input/key.h
      input/keyboard.cc
      input/keyboard.h
      input/mouse.cc
      input/mouse.h
      input/mousebutton.cc
      input/mousebutton.h
    )
    source_group("input" FILES ${Render_files_input})

    set(Render_input
      ${Render_input__base}
      ${Render_files_input_platform}      
      ${Render_files_input}
    )

	set(Render_files_instancing
		instancing/instancerenderer.h
		instancing/instancerenderer.cc
		instancing/instanceserver.h
		instancing/instanceserver.cc
	)
	source_group("instancing" FILES ${Render_files_instancing})

	set(Render_instancing__base
		instancing/base/instancerendererbase.h
		instancing/base/instancerendererbase.cc
		instancing/base/instanceserverbase.h
		instancing/base/instanceserverbase.cc
	)
	source_group("instancing\\base" FILES ${Render_instancing__base})

	IF(N_BUILD_DX11)
	set(Render_instancing__imp
		instancing/d3d11/d3d11instancerenderer.h
		instancing/d3d11/d3d11instancerenderer.cc
		instancing/d3d11/d3d11instanceserver.h
		instancing/d3d11/d3d11instanceserver.cc
	)
	source_group("instancing\\d3d11" FILES ${Render_instancing__imp})
	ELSEIF(N_BUILD_OGL4)
	set(Render_instancing__imp
		instancing/ogl4/ogl4instancerenderer.h
		instancing/ogl4/ogl4instancerenderer.cc
		instancing/ogl4/ogl4instanceserver.h
		instancing/ogl4/ogl4instanceserver.cc
	)
	source_group("instancing\\ogl4" FILES ${Render_instancing__imp})
	ELSEIF(N_BUILD_VULKAN)
	set(Render_instancing__imp
		instancing/vk/vkinstancerenderer.h
		instancing/vk/vkinstancerenderer.cc
		instancing/vk/vkinstanceserver.h
		instancing/vk/vkinstanceserver.cc
	)
	source_group("instancing\\vk" FILES ${Render_instancing__imp})
	ENDIF()

	set(Render_instancing
		${Render_instancing__base}
		${Render_instancing__imp}
		${Render_files_instancing}
	)
	
	set(Render_files_lighting__base
		lighting/base/lightserverbase.cc
		lighting/base/lightserverbase.h
		lighting/base/shadowserverbase.cc
		lighting/base/shadowserverbase.h
	)
	source_group("lighting\\base" FILES ${Render_files_lighting__base})

	set(Render_lighting__base
		${Render_files_lighting__base}
	)

	set(Render_files_lighting__handlers
		lighting/handlers/abstractlightentityhandler.cc
		lighting/handlers/globallightentityhandler.cc
		lighting/handlers/pointlightentityhandler.cc
		lighting/handlers/spotlightentityhandler.cc
	)
	source_group("lighting\\handlers" FILES ${Render_files_lighting__handlers})

	set(Render_lighting__handlers
		${Render_files_lighting__handlers}
	)
	
	IF(N_BUILD_OGL4)
		set(Render_files_lighting__ogl4
		lighting/ogl4/ogl4lightserver.cc
		lighting/ogl4/ogl4lightserver.h
		lighting/ogl4/ogl4shadowserver.cc
		lighting/ogl4/ogl4shadowserver.h
	)
	source_group("lighting\\ogl4" FILES ${Render_files_lighting__ogl4})
	
	set(Render_lighting__impl
		${Render_files_lighting__ogl4}
	)
	ELSEIF(N_BUILD_VULKAN)
		set(Render_files_lighting__vk
		lighting/vk/vklightserver.cc
		lighting/vk/vklightserver.h
		lighting/vk/vkshadowserver.cc
		lighting/vk/vkshadowserver.h
	)
	source_group("lighting\\vk" FILES ${Render_files_lighting__vk})
	
	set(Render_lighting__impl
		${Render_files_lighting__vk}
	)
	ENDIF()

    set(Render_files_lighting
		lighting/lightserver.cc
		lighting/lightserver.h
		lighting/lighttype.cc
		lighting/lighttype.h
		lighting/pssmutil.cc
		lighting/pssmutil.h
		lighting/csmutil.cc
		lighting/csmutil.h
		lighting/shadowserver.cc
		lighting/shadowserver.h
		lighting/environmentprobe.cc
		lighting/environmentprobe.h
    )
    source_group("lighting" FILES ${Render_files_lighting})

    set(Render_lighting
		${Render_lighting__base}
		${Render_lighting__handlers}
		#${Render_lighting__lightprepass}
		#${Render_files_lighting__ao}
		#${Render_lighting__sm30}
		#${Render_lighting__sm50}
		${Render_lighting__impl}
		${Render_files_lighting}
    )


      set(Render_files_models__nodes
        models/nodes/animatornode.cc
        models/nodes/animatornode.h
        models/nodes/animatornodeinstance.cc
        models/nodes/animatornodeinstance.h
        models/nodes/animkey.h
        models/nodes/animkeyarray.h
        models/nodes/animlooptype.h
        models/nodes/shapenode.cc
        models/nodes/shapenode.h
        models/nodes/shapenodeinstance.cc
        models/nodes/shapenodeinstance.h
		models/nodes/statenode.cc
        models/nodes/statenode.h
        models/nodes/statenodeinstance.cc
        models/nodes/statenodeinstance.h
        models/nodes/transformnode.cc
        models/nodes/transformnode.h
        models/nodes/transformnodeinstance.cc
        models/nodes/transformnodeinstance.h
      )
      source_group("models\\nodes" FILES ${Render_files_models__nodes})

      set(Render_models__nodes
        ${Render_files_models__nodes}
      )

    set(Render_files_models
      models/managedmodel.cc
      models/managedmodel.h
      models/model.cc
      models/model.h
      models/modelinstance.cc
      models/modelinstance.h
      models/modelnode.cc
      models/modelnode.h
      models/modelnodeinstance.cc
      models/modelnodeinstance.h
      models/modelserver.cc
      models/modelserver.h
      models/streammodelloader.cc
      models/streammodelloader.h
      models/visresolvecontainer.h
      models/visresolver.cc
      models/visresolver.h
    )
    source_group("models" FILES ${Render_files_models})

    set(Render_models
      ${Render_models__nodes}
      ${Render_files_models}
    )

      set(Render_files_particles__base
        particles/base/particlerendererbase.cc
        particles/base/particlerendererbase.h
        particles/base/particlerenderinfobase.h
        particles/base/particlesysteminstancebase.cc
        particles/base/particlesysteminstancebase.h
      )
      source_group("particles\\base" FILES ${Render_files_particles__base})

      set(Render_particles__base
        ${Render_files_particles__base}
      )

      IF(N_BUILD_DX11)
      set(Render_files_particles__d3d11
        particles/d3d11/d3d11particlerenderer.cc
        particles/d3d11/d3d11particlerenderer.h
        particles/d3d11/d3d11particlesysteminstance.cc
        particles/d3d11/d3d11particlesysteminstance.h
      )
      source_group("particles\\d3d11" FILES ${Render_files_particles__d3d11})

      set(Render_particles__imp
        ${Render_files_particles__d3d11}
      )
      ELSEIF(N_BUILD_DX9)
      set(Render_files_particles__d3d9
        particles/d3d9/d3d9particlerenderer.cc
        particles/d3d9/d3d9particlerenderer.h
        particles/d3d9/d3d9particlesysteminstance.cc
        particles/d3d9/d3d9particlesysteminstance.h
      )
      source_group("particles\\d3d9" FILES ${Render_files_particles__d3d9})

      set(Render_particles__imp
        ${Render_files_particles__d3d9}
      )

	  ELSEIF(N_BUILD_OGL4)
      set(Render_files_particles__ogl4
        particles/ogl4/ogl4particlerenderer.cc
        particles/ogl4/ogl4particlerenderer.h
        particles/ogl4/ogl4particlesysteminstance.cc
        particles/ogl4/ogl4particlesysteminstance.h
      )
      source_group("particles\\ogl4" FILES ${Render_files_particles__ogl4})

      set(Render_particles__imp
        ${Render_files_particles__ogl4}
      )
	  ELSEIF(N_BUILD_VULKAN)
	  set(Render_files_particles__vk
        particles/vk/vkparticlerenderer.cc
        particles/vk/vkparticlerenderer.h
        particles/vk/vkparticlesysteminstance.cc
        particles/vk/vkparticlesysteminstance.h
      )
      source_group("particles\\vk" FILES ${Render_files_particles__vk})

      set(Render_particles__imp
        ${Render_files_particles__vk}
      )
      ENDIF()

    set(Render_files_particles
      particles/emitterattrs.cc
      particles/emitterattrs.h
      particles/emittermesh.cc
      particles/emittermesh.h
      particles/envelopecurve.cc
      particles/envelopecurve.h
      particles/envelopesamplebuffer.cc
      particles/envelopesamplebuffer.h
      particles/particle.h
      particles/particlerenderer.cc
      particles/particlerenderer.h
      particles/particlerenderinfo.h
      particles/particleserver.cc
      particles/particleserver.h
      particles/particlesystem.cc
      particles/particlesystem.h
      particles/particlesysteminstance.cc
      particles/particlesysteminstance.h
      particles/particlesystemnode.cc
      particles/particlesystemnode.h
      particles/particlesystemnodeinstance.cc
      particles/particlesystemnodeinstance.h
      particles/particlesystemstate.h
    )
    source_group("particles" FILES ${Render_files_particles})

    set(Render_particles
      ${Render_particles__base}
      ${Render_particles__imp}
      ${Render_files_particles}
    )

      set(Render_files_rendermodules__rt
        rendermodules/rt/rtplugin.cc
        rendermodules/rt/rtplugin.h
        rendermodules/rt/rtpluginregistry.cc
        rendermodules/rt/rtpluginregistry.h
      )
      source_group("rendermodules\\rt" FILES ${Render_files_rendermodules__rt})

      set(Render_rendermodules__rt
        ${Render_files_rendermodules__rt}
      )

    set(Render_files_rendermodules
      rendermodules/rendermodule.cc
      rendermodules/rendermodule.h
    )
    source_group("rendermodules" FILES ${Render_files_rendermodules})

    set(Render_rendermodules
      ${Render_rendermodules__rt}
      ${Render_files_rendermodules}
    )

    set(Render_files_renderutil
      renderutil/drawfullscreenquad.cc
      renderutil/drawfullscreenquad.h
	  renderutil/drawquad.cc
      renderutil/drawquad.h
      renderutil/mayacamerautil.cc
      renderutil/mayacamerautil.h
	  renderutil/freecamerautil.cc
      renderutil/freecamerautil.h
      renderutil/mouserayutil.cc
      renderutil/mouserayutil.h
      renderutil/nodelookuputil.cc
      renderutil/nodelookuputil.h
    )
    source_group("renderutil" FILES ${Render_files_renderutil})

    set(Render_renderutil
      ${Render_files_renderutil}
    )

	IF(N_BUILD_DX11)
      set(Render_files_resources__d3d11streaming
        resources/d3d11streaming/d3d11texturecreator.cc
        resources/d3d11streaming/d3d11texturecreator.h
        resources/d3d11streaming/d3d11textureinfo.cc
        resources/d3d11streaming/d3d11textureinfo.h
        resources/d3d11streaming/d3d11texturestreamer.cc
        resources/d3d11streaming/d3d11texturestreamer.h
      )
      source_group("resources\\d3d11streaming" FILES ${Render_files_resources__d3d11streaming})

      set(Render_resources__streaming__imp
        ${Render_files_resources__d3d11streaming}
      )
    ELSEIF(N_BUILD_DX9)
      set(Render_files_resources__d3d9streaming
		resources/d3d9streaming/d3d9texturecreator.cc
		resources/d3d9streaming/d3d9texturecreator.h
		resources/d3d9streaming/d3d9textureinfo.cc
		resources/d3d9streaming/d3d9textureinfo.h
		resources/d3d9streaming/d3d9texturestreamer.cc
		resources/d3d9streaming/d3d9texturestreamer.h
      )
      source_group("resources\\d3d9streaming" FILES ${Render_files_resources__d3d9streaming})

      set(Render_resources__streaming__imp
        ${Render_files_resources__d3d9streaming}
      )
      ELSEIF(N_BUILD_OGL4)
      set(Render_files_resources__ogl4streaming
        resources/ogl4streaming/ogl4texturecreator.cc
        resources/ogl4streaming/ogl4texturecreator.h
        resources/ogl4streaming/ogl4textureinfo.cc
        resources/ogl4streaming/ogl4textureinfo.h
        resources/ogl4streaming/ogl4texturestreamer.cc
        resources/ogl4streaming/ogl4texturestreamer.h
      )
      source_group("resources\\ogl4streaming" FILES ${Render_files_resources__ogl4streaming})

      set(Render_resources__streaming__imp
        ${Render_files_resources__ogl4streaming}
      )
	  ELSEIF(N_BUILD_VULKAN)
      set(Render_files_resources__vkstreaming
        resources/vkstreaming/vktexturecreator.cc
        resources/vkstreaming/vktexturecreator.h
        resources/vkstreaming/vktextureinfo.cc
        resources/vkstreaming/vktextureinfo.h
        resources/vkstreaming/vktexturestreamer.cc
        resources/vkstreaming/vktexturestreamer.h
      )
      source_group("resources\\vkstreaming" FILES ${Render_files_resources__vkstreaming})

      set(Render_resources__streaming__imp
        ${Render_files_resources__vkstreaming}
      )
      ENDIF()

      set(Render_files_resources__streaming
        resources/streaming/loadingresource.cc
        resources/streaming/loadingresource.h
        resources/streaming/poolloadingresource.cc
        resources/streaming/poolloadingresource.h
        resources/streaming/poolresourcemapper.cc
        resources/streaming/poolresourcemapper.h
        resources/streaming/poolscheduler.cc
        resources/streaming/poolscheduler.h
        resources/streaming/resourcecreator.cc
        resources/streaming/resourcecreator.h
        resources/streaming/resourcedictionaryreader.cc
        resources/streaming/resourcedictionaryreader.h
        resources/streaming/resourceinfo.cc
        resources/streaming/resourceinfo.h
        resources/streaming/resourcepool.cc
        resources/streaming/resourcepool.h
        resources/streaming/resourcerequestinfo.cc
        resources/streaming/resourcerequestinfo.h
        resources/streaming/resourcescheduler.cc
        resources/streaming/resourcescheduler.h
        resources/streaming/resourceslot.cc
        resources/streaming/resourceslot.h
        resources/streaming/streamingresourcemapper.cc
        resources/streaming/streamingresourcemapper.h
        resources/streaming/streamingtexturepagehandler.cc
        resources/streaming/streamingtexturepagehandler.h
        resources/streaming/texturecreator.cc
        resources/streaming/texturecreator.h
        resources/streaming/textureinfo.h
        resources/streaming/texturepoolmapperscheduler.cc
        resources/streaming/texturepoolmapperscheduler.h
        resources/streaming/texturerequestinfo.h
        resources/streaming/texturestreamer.cc
        resources/streaming/texturestreamer.h
      )
      source_group("resources\\streaming" FILES ${Render_files_resources__streaming})

      set(Render_resources__streaming
        ${Render_files_resources__streaming}
      )

    set(Render_files_resources
      resources/managedmesh.cc
      resources/managedmesh.h
      resources/managedresource.cc
      resources/managedresource.h
      resources/managedtexture.cc
      resources/managedtexture.h
      resources/resource.cc
      resources/resource.h
      resources/resourceallocator.cc
      resources/resourceallocator.h
      resources/resourcedictionary.cc
      resources/resourcedictionary.h
      resources/resourceid.h
      resources/resourceloader.cc
      resources/resourceloader.h
      resources/resourcelump.cc
      resources/resourcelump.h
      resources/resourcemanager.cc
      resources/resourcemanager.h
      resources/resourcemapper.cc
      resources/resourcemapper.h
      resources/resourcesaver.cc
      resources/resourcesaver.h
      resources/simpleresourcemapper.cc
      resources/simpleresourcemapper.h
      resources/streamresourceloader.cc
      resources/streamresourceloader.h
    )
    source_group("resources" FILES ${Render_files_resources})

    set(Render_resources
      ${Render_resources__streaming__imp}
      ${Render_resources__streaming}
      ${Render_files_resources}
    )


      set(Render_files_visibility__handler
		visibility/handler/visibilityhandler.cc
        visibility/handler/visibilityboxhandler.cc
        visibility/handler/visibilityclusterhandler.cc
      )
      source_group("visibility\\handler" FILES ${Render_files_visibility__handler})

      set(Render_visibility__handler
        ${Render_files_visibility__handler}
      )

      set(Render_files_visibility__visibilitysystems
        visibility/visibilitysystems/visibilitybox.cc
        visibility/visibilitysystems/visibilitybox.h
        visibility/visibilitysystems/visibilityboxsystem.cc
        visibility/visibilitysystems/visibilityboxsystem.h
        visibility/visibilitysystems/visibilitycell.cc
        visibility/visibilitysystems/visibilitycell.h
        visibility/visibilitysystems/visibilitycluster.cc
        visibility/visibilitysystems/visibilitycluster.h
        visibility/visibilitysystems/visibilityclustersystem.cc
        visibility/visibilitysystems/visibilityclustersystem.h
        visibility/visibilitysystems/visibilityquadtree.cc
        visibility/visibilitysystems/visibilityquadtree.h
        visibility/visibilitysystems/visibilitysystembase.cc
        visibility/visibilitysystems/visibilitysystembase.h
      )
      source_group("visibility\\visibilitysystems" FILES ${Render_files_visibility__visibilitysystems})

      set(Render_visibility__visibilitysystems
        ${Render_files_visibility__visibilitysystems}
      )

    set(Render_files_visibility
      visibility/observercontext.cc
      visibility/observercontext.h
      visibility/visibilitychecker.cc
      visibility/visibilitychecker.h
      visibility/visibilitycontainer.cc
      visibility/visibilitycontainer.h
      visibility/visibilitycontext.cc
      visibility/visibilitycontext.h
      visibility/visibilityquery.cc
      visibility/visibilityquery.h
    )
    source_group("visibility" FILES ${Render_files_visibility})

    set(Render_visibility
      ${Render_visibility__handler}
      ${Render_visibility__visibilitysystems}
      ${Render_files_visibility}
    )

	set(SOURCES_files_physics

		physics/physicsserver.cc
		physics/physicsserver.h

		physics/visualdebuggerserver.cc
		physics/visualdebuggerserver.h

		physics/scene.h
		physics/scene.cc

		physics/physicsbody.h
		physics/physicsbody.cc

		physics/physicsobject.h
		physics/physicsobject.cc

		physics/collider.h
		physics/collider.cc

		physics/contactlistener.h
		physics/contactlistener.cc

		physics/staticobject.h
		physics/staticobject.cc

		physics/filterset.h
		physics/filterset.cc

		physics/materialtable.h
		physics/materialtable.cc

		physics/contact.h
		physics/contact.cc

		physics/physicsprobe.h
		physics/physicsprobe.cc

		physics/model/templates.h
		physics/model/templates.cc

		physics/joint.h
		physics/joint.cc

		physics/character.h
		physics/character.cc

		#controller.h
		#controller.cc

		#vehicle.h
		#vehicle.cc

		#material.h
		#material.cc
    )
    set(SOURCES_files_physics_util
		physics/util/areaimpulse.h
		physics/util/areaimpulse.cc

		physics/util/explosionareaimpulse.h
		physics/util/explosionareaimpulse.cc

		physics/util/mousegripper.h
		physics/util/mousegripper.cc
	)

    set(SOURCES_files_physics_resource
		physics/resource/streamphysicsmeshloader.h
		physics/resource/streamphysicsmeshloader.cc
		physics/resource/physicsmesh.h
		physics/resource/physicsmesh.cc
		physics/resource/physicsmeshbase.h
		physics/resource/physicsmeshbase.cc
		physics/resource/managedphysicsmesh.h
		physics/resource/managedphysicsmesh.cc
		physics/resource/managedphysicsmodel.h
		physics/resource/managedphysicsmodel.cc
		physics/model/physicsmodel.h
		physics/model/physicsmodel.cc
		physics/resource/streamphysicsmodelloader.h
		physics/resource/streamphysicsmodelloader.cc
	)

    set(SOURCES_files_physics_base
    	physics/base/basescene.h
		physics/base/basescene.cc
		physics/base/basebody.h
		physics/base/basebody.cc
		physics/base/basecollider.h
		physics/base/basecollider.cc
		physics/base/basecontact.h
		physics/base/basecontact.cc
		physics/base/basecontactlistener.h
		physics/base/basecontactlistener.cc
		physics/base/basestatic.h
		physics/base/basestatic.cc
		physics/base/baseprobe.h
		physics/base/baseprobe.cc
		physics/base/basecharacter.h
		physics/base/basecharacter.cc
		#base/basecontroller.h
		#base/basecontroller.cc
		#base/basevehicle.h
		#base/basevehicle.cc
		physics/base/physicsserverbase.h
		physics/base/physicsserverbase.cc
		physics/base/basevisualdebuggerserver.h
		physics/base/basevisualdebuggerserver.cc
		#base/basematerial.h
		#base/basematerial.cc
    )

    SET(SOURCES_files_bullet 
		physics/bullet/bulletcharacter.h
		physics/bullet/bulletcharacter.cc
		physics/bullet/bulletbody.cc
		physics/bullet/bulletbody.h
		physics/bullet/bulletcollider.cc
		physics/bullet/bulletcollider.h
		physics/bullet/bulletphysicsmesh.cc
		physics/bullet/bulletphysicsmesh.h
		physics/bullet/bulletphysicsserver.cc
		physics/bullet/bulletphysicsserver.h
		physics/bullet/bulletscene.cc
		physics/bullet/bulletscene.h
		physics/bullet/bulletstatic.h
		physics/bullet/bulletstatic.cc
		physics/bullet/bulletprobe.h
		physics/bullet/bulletprobe.cc
		physics/bullet/bulletjoint.h
		physics/bullet/bulletjoint.cc
		physics/bullet/conversion.h
		physics/bullet/nebulamotionstate.h
		physics/bullet/debugdrawer.h
		physics/bullet/debugdrawer.cc
		physics/bullet/bullethinge.h
		physics/bullet/bulletuniversaljoint.h
    	physics/bullet/bullethinge.cc
		physics/bullet/bulletuniversaljoint.cc
		physics/bullet/bulletslider.h
		physics/bullet/bulletslider.cc
		physics/bullet/bulletpointtopoint.h
		physics/bullet/bulletpointtopoint.cc
		physics/bullet/raycallbacks.h
	)

	SET(SOURCES_files_havok
		physics/havok/conversion.h
		physics/havok/havokutil.cc
		physics/havok/havokutil.h
		physics/havok/havokdebugdrawer.cc
		physics/havok/havokdebugdrawer.h

		physics/havok/havokphysicsserver.cc
		physics/havok/havokphysicsserver.h
		physics/havok/havokvisualdebuggerserver.cc
		physics/havok/havokvisualdebuggerserver.h
		physics/havok/havokstatic.cc
		physics/havok/havokstatic.h
		physics/havok/havokscene.cc
		physics/havok/havokscene.h
		physics/havok/havokprobe.cc
		physics/havok/havokprobe.h
		physics/havok/havokbody.cc
		physics/havok/havokbody.h
		physics/havok/havokjoint.cc
		physics/havok/havokjoint.h
		physics/havok/havokcollider.cc
		physics/havok/havokcollider.h
		physics/havok/havokcontact.cc
		physics/havok/havokcontact.h
		physics/havok/havokcontactlistener.cc
		physics/havok/havokcontactlistener.h
		physics/havok/havokcharacterbase.cc
		physics/havok/havokcharacterbase.h
		physics/havok/havokcharacterphantom.cc
		physics/havok/havokcharacterphantom.h
		physics/havok/havokcharacterrigidbody.cc
		physics/havok/havokcharacterrigidbody.h
		physics/havok/havokphysicsmesh.cc
		physics/havok/havokphysicsmesh.h
		physics/havok/havokhinge.cc
		physics/havok/havokhinge.h
		physics/havok/havokpointtopoint.cc
		physics/havok/havokpointtopoint.h
		physics/havok/havokslider.cc
		physics/havok/havokslider.h
		physics/havok/havokuniversaljoint.cc
		physics/havok/havokuniversaljoint.h
		physics/havok/havokragdolljoint.cc
		physics/havok/havokragdolljoint.h
		physics/havok/havokchainjoint.cc
		physics/havok/havokchainjoint.h
	)

    SET(SOURCES_files_physics_joints
			physics/joints.h
			physics/base/basejoint.h
			physics/base/basejoint.cc
    		physics/joints/basehinge.h
    		physics/joints/basehinge.cc
			physics/joints/baseuniversaljoint.h   
			physics/joints/baseuniversaljoint.cc  
			physics/joints/baseragdolljoint.h   
			physics/joints/baseragdolljoint.cc  
			physics/joints/basechainjoint.h   
			physics/joints/basechainjoint.cc  
			physics/joints/baseslider.h         
			physics/joints/baseslider.cc 			
			physics/joints/basepointtopoint.h       
			physics/joints/basepointtopoint.cc			
			physics/joints/sliderjoint.h
			physics/joints/sliderjoint.cc
			physics/joints/universaljoint.h
			physics/joints/universaljoint.cc
			physics/joints/ragdolljoint.h
			physics/joints/ragdolljoint.cc
			physics/joints/chainjoint.h         
			physics/joints/chainjoint.cc
			physics/joints/hingejoint.h           
			physics/joints/hingejoint.cc 			
			physics/joints/pointtopointjoint.h       
			physics/joints/pointtopointjoint.cc

	)


    source_group("physics" FILES ${SOURCES_files_physics})
    source_group("physics\\resource" FILES ${SOURCES_files_physics_resource})
    source_group("physics\\bullet" FILES ${SOURCES_files_bullet})
	source_group("physics\\havok" FILES ${SOURCES_files_havok})    
    source_group("physics\\base" FILES ${SOURCES_files_physics_base})
	source_group("physics\\util" FILES ${SOURCES_files_physics_util})
	source_group("physics\\joints" FILES ${SOURCES_files_physics_joints})

    set(SOURCES_physics_core
      ${SOURCES_files_physics}
      ${SOURCES_files_physics_base}
      ${SOURCES_files_physics_resource}
	  ${SOURCES_files_physics_util}
	  ${SOURCES_files_physics_joints}
    )

IF(N_BUILD_BULLET)

	INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/
					${Nebula3_SOURCE_DIR}/code/extlibs/bullet/bullet/src
					)
	set(SOURCES_physics
		${SOURCES_physics_core}
		${SOURCES_files_bullet}
	)
ENDIF()

IF(N_BUILD_HAVOK)					
	set(SOURCES_physics
		${SOURCES_physics_core}
		${SOURCES_files_havok_source}
		${SOURCES_files_havok}
	)

	# need to enable SIMD (2 means disabled, is the default) else there will be some linker error
	ADD_DEFINITIONS(-DHK_CONFIG_SIMD=1)
ENDIF()

	set(Render_files_picking
		picking/pickingserver.h
		picking/pickingserver.cc
	)
	SOURCE_GROUP("picking" FILES ${Render_files_picking})

	set(Render_picking
		${Render_files_picking}
	)

	set(Render_precompiled_src
	    stdneb.cc
		stdneb.h
	)
	SOURCE_GROUP("Precompiled" FILES ${Render_precompiled_src})

	set(Render_NIDLS
		debugrender/debugrenderprotocol.nidl
		graphics/graphicsprotocol.nidl
		algorithm/algorithmprotocol.nidl
		visibility/visibilityprotocol.nidl
	)
	



  set(Render_SRCS
	${Render_algorithm}
    ${Render_animation}
    ${Render_apprender}
	${Render_billboards}
    ${Render_characters}
    ${Render_coreanimation}
    ${Render_coregraphics}
    ${Render_debugrender}
	${Render_events}
    ${Render_frame}
    ${Render_graphics}
	${Render_image}
    ${Render_input}
	${Render_picking}
	${Render_instancing}
    ${Render_lighting}
    ${Render_materials}
    ${Render_models}
    ${Render_particles}
    ${Render_rendermodules}
    ${Render_renderutil}
    ${Render_resources}
    ${Render_visibility}
    ${Render_precompiled_src}
	${SOURCES_physics}
  )

IF(WIN32)
	ADD_DEFINITIONS(-DGLFW_EXPOSE_NATIVE_WIN32 -DGLFW_EXPOSE_NATIVE_WGL)
ELSE()
	ADD_DEFINITIONS(-DGLFW_EXPOSE_NATIVE_X11 -DGLFW_EXPOSE_NATIVE_GLX)
ENDIF()
  
N_WRAP_NIDL_FILES(render_NIDLS ${Render_NIDLS})

	
add_library(render STATIC ${Render_SRCS} )



TARGET_INCLUDE_DIRECTORIES(render 	PUBLIC ${Nebula3_SOURCE_DIR}/code/render/ 
									${Nebula3_SOURCE_DIR}/code/extlibs/DevIL/DevIL/include)

TARGET_LINK_LIBRARIES(render PUBLIC foundation render_NIDLS libjpeg libpng api  )
SET_TARGET_PROPERTIES(render PROPERTIES FOLDER "N3SDK/N3")

ADD_DEPENDENCIES(render foundation_NIDLS render_NIDLS ${PHYSICS_DEPENDENCIES})

TARGET_INCLUDE_DIRECTORIES(render_NIDLS PUBLIC $<TARGET_PROPERTY:render,INTERFACE_INCLUDE_DIRECTORIES>)
ADD_SUBDIRECTORY(jobs)
TARGET_LINK_LIBRARIES(render PUBLIC render_animjobsample render_animjobsamplemix render_charevalskeletonjob render_particlejob render_visibilityboxsystemjob render_visibilityquadtreejob EASTL)


IF(N_BUILD_HAVOK)
	
	#FIXME fugly, need a real havok detection
	FIND_PATH(HAVOC_DIR "Havok Limited Use License Agreement for PC XS 03-20-2013.txt"	${Nebula3_SOURCE_DIR}/libs/hk2013_1_0_r1)				
	SET(Havok_SOURCE_DIR ${HAVOC_DIR}/Source)	
	if(MSVC90)
		SET(Havok_LIB_DIR ${HAVOC_DIR}/Lib/win32_net_9-0)
	ELSEIF(MSVC10)
		SET(Havok_LIB_DIR ${HAVOC_DIR}/Lib/win32_vs2010)
	ELSEIF(MSVC11)
		SET(Havok_LIB_DIR ${HAVOC_DIR}/Lib/win32_vs2012_win7)
	ENDIF()

	SET(HAVOK_LIBS hkaAnimation.lib hkaInternal.lib hkaPhysics2012Bridge.lib hkBase.lib hkcdCollide.lib hkcdInternal.lib hkCompat.lib
					hkgBridge.lib hkgCommon.lib hkGeometryUtilities.lib hkInternal.lib hkpCollide.lib hkpConstraint.lib hkpConstraintSolver.lib
					hkpDynamics.lib hkpInternal.lib hkpUtilities.lib hkpVehicle.lib hkSceneData.lib hkSerialize.lib hkVisualize.lib)
	
	SET(HAVOK_DBG_LIBS)	
	FOREACH(CUR_LIB ${HAVOK_LIBS})
		LIST(APPEND HAVOK_DBG_LIBS ${Havok_LIB_DIR}/debug/${CUR_LIB})
	ENDFOREACH()
	
	SET(HAVOK_RELEASE_LIBS)
	FOREACH(CUR_LIB ${HAVOK_LIBS})
		LIST(APPEND HAVOK_RELEASE_LIBS ${Havok_LIB_DIR}/dev/${CUR_LIB})
	ENDFOREACH()
	SET(HAVOK_PUBLIC_LIBS)
	FOREACH(CUR_LIB ${HAVOK_LIBS})
		LIST(APPEND HAVOK_PUBLIC_LIBS ${Havok_LIB_DIR}/release/${CUR_LIB})
	ENDFOREACH()

	TARGET_LINK_LIBRARIES(render PUBLIC $<$<CONFIG:Debug>:${HAVOK_DBG_LIBS}> $<$<CONFIG:Release>:${HAVOK_PUBLIC_LIBS}> $<$<CONFIG:RelWithDebInfo>:${HAVOK_RELEASE_LIBS}>)

ELSEIF(N_BUILD_BULLET)
	ADD_DEPENDENCIES(render BulletDynamics BulletCollision LinearMath)
	TARGET_LINK_LIBRARIES(render PUBLIC $<$<BOOL:${N_BUILD_BULLET}>:BulletDynamics BulletCollision LinearMath> )
ENDIF()
TARGET_INCLUDE_DIRECTORIES(render PUBLIC $<$<BOOL:${N_BUILD_HAVOK}>:${Havok_SOURCE_DIR}>)


TARGET_LINK_LIBRARIES(render PUBLIC $<$<BOOL:${N_BUILD_OGL4} OR ${N_BUILD_VULKAN}>: DevIL ILU ILUT SOIL> )

N_SET_COMPILER_DEFAULTS()
IF(N_USE_PRECOMPILED_HEADERS)
    N_SET_PCH(render stdneb.h stdneb.cc)
ENDIF()
