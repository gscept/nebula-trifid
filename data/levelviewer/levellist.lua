levelsLoaded = false
layoutsloaded = false

function _filllevels()
	if not levelsLoaded then
		levels = getlevels()
		size = table.getn(levels)
		for i=1,size do
			commandstring = "scr.loadlevel('" .. levels[i] .. "')"
			addelement("_levellist","levellist","dummy","button",levels[i],commandstring)
		end
	levelsLoaded = true
	end
end;
function _filllayouts()
	if not layoutsloaded then
		layouts = getlayouts()
		size = table.getn(layouts)
		for i=1,size do
			commandstring = "scr.togglelayout('" .. layouts[i] .. "')"
			addelement("_layoutlist","levellist","dummy","button","Toggle " .. layouts[i],commandstring)			
		end
	layoutsloaded = true
	end
end;