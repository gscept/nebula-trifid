require('vecmath')
require('io')
require('string')
function __init()
	registerscript("Load MIKT level", "loadmiktlevel()")		
end

function loadmiktlevel()
	local f = io.open(uri("proj:data/TransformData.txt"), "r")
	local text = f:read("*all")
	for row in string.gmatch(text, '[^\n]*\n') do
	
		local thisrow = row
		thisrow = string.sub(thisrow, 1, -2)
		
		-- load gfx part
		local gfx = string.gmatch(thisrow, '[^%[]*')()
		thisrow = string.sub(thisrow, string.len(gfx)+1, string.len(thisrow))
		
		-- now load position
		local pos = string.gmatch(thisrow, '%[.-%]')()
		thisrow = string.sub(thisrow, string.len(pos)+1, string.len(thisrow))
		pos = string.sub(pos, 2, -2)		
		posvec = parsevec(pos)
		posvec[4] = 1
		
		
		-- last load rotation
		rot = string.sub(thisrow, 2, -2)
		rotvec = parsevec(rot)
		rotvec[4] = 0
		
		createenvironmententity(posvec, rotvec, 'VRENV_assets/' .. gfx)
	end
	f:close()
end

function parsevec(str)
	local arr = {}
	local ctr = 0
	str = string.gsub(str, '%s', "")
	printconsole("Full row: " .. str)
	
	for flt in string.gmatch(str, '[^,]*') do
		if (string.len(flt) > 0) then
			arr[ctr] = tonumber(flt)
			ctr = ctr + 1
			printconsole("Value: " .. tostring(ctr) .. " " .. flt)
		end
	end
	
	vec = vec4(arr[0], arr[1], arr[2], arr[3])
	return vec
end