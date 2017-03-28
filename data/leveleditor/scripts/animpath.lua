require('vecmath')
function __init()
	registerscript("Create Animpath", "animpath()")		
end

function animpath()
	selection = getselection()
	printconsole("\n<NebulaT>\n<Track name=\"track\" rotationInfinity=\"Cycle\" scalingInfinity=\"Cycle\" translationInfinity=\"Cycle\">")
	count = 0.0
	--rot = vec4(getorientation(selection[0]))
	for i = 0, #selection do			
		mat = mat4(gettransform(selection[i]))			
		pos = mat:getposition()
		rot = vec4(getorientation(selection[i]))
		nextindex = (i + 1) % (#selection)
		nextmat = mat4(gettransform(selection[nextindex]))
		--pos = nextmat:getposition() - pos
		--rot = vec4(getorientation(selection[nextindex])) - rot
		msg = "<Translate to=\"" .. pos[1] .. "," .. pos[2] .. "," .. pos[3] .. ",0.0\" start=\"" .. count .."\" end=\"" .. count+10.0 .. "\"/>"
		rmsg = "<Rotate to=\"" .. rot[1] .. ", " .. rot[2] .. ", " .. rot[3] .. ",0.0\" start=\"" .. count .."\" end=\"" .. count+10.0 .. "\"/>"
		printconsole(msg)				
		printconsole(rmsg)	
		count = count + 10.0
	end
	printconsole("</Track>\n</NebulaT>")
end
