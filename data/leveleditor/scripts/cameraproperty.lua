function __property_init()
	registerpropertycallback("GraphicsFeature::MayaCameraProperty", "Apply Editor Camera", "applyEditorCamera(%d)")
end

function applyEditorCamera(entityId)
	id = geteditorcamera()
	trans = gettransform(id)
	focus = getattribute(id,"MayaCameraCenterOfInterest")
	setentitytransform(entityId, trans)
	seteditorentityattribute(entityId,"MayaCameraCenterOfInterest", focus)
end