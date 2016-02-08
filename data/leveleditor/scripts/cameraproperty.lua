function __property_init()
	registerpropertycallback("GraphicsFeature::MayaCameraProperty", "Apply Editor Camera", "applyEditorCameraToMaya(%d)")
	registerpropertycallback("GraphicsFeature::CameraProperty", "Apply Editor Camera", "applyEditorCamera(%d)")	
end

function applyEditorCameraToMaya(entityId)
	id = geteditorcamera()
	trans = gettransform(id)
	focus = getattribute(id,"MayaCameraCenterOfInterest")
	setentitytransform(entityId, trans)
	seteditorentityattribute(entityId,"MayaCameraCenterOfInterest", focus)
end


function applyEditorCamera(entityId)
	id = geteditorcamera()
	trans = gettransform(id)	
	setentitytransform(entityId, trans)	
end