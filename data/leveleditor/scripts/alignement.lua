require('vecmath')
function __init()
	registerscript("Align bottom", "alignbottom()")	
	registerscript("Align top", "aligntop()")	
end

function alignbottom()
	selection = getselection()
	if #selection > 0 then		
		low = getposition(selection[0])
		for i = 0, #selection do
			v = getposition(selection[i])		
			if v[2] < low[2] then
				low = v
			end
		end
		for i = 0, #selection do			
			mat = mat4(gettransform(selection[i]))			
			pos = mat:getposition()
			pos[2] = low[2]
			mat:setposition(pos)
			setentitytransform(selection[i],mat)			
		end		
	end
end

function aligntop()
	selection = getselection()
	if #selection > 0 then
		low = vec4()
		low = getposition(selection[0])
		for i = 0, #selection do
			v = getposition(selection[i])		
			if v[2] > low[2] then
				low = v
			end
		end
		for i = 0, #selection do			
			mat = mat4(gettransform(selection[i]))			
			pos = mat:getposition()
			pos[2] = low[2]
			mat:setposition(pos)
			setentitytransform(selection[i],mat)			
		end		
	end
end