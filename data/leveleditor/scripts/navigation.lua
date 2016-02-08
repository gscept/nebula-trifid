function __property_init()
	registerpropertycallback("Navigation Mesh", "Generate", "updateNavmeshProperty(%d)")	
end

function updateNavmeshProperty(entityId)
	updatenavmesh(entityId)
end