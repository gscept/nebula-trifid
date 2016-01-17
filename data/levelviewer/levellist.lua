levelsLoaded = false

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