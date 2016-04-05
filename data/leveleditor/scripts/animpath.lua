require('vecmath')
function __init()
	registerscript("Create Animpath", "animpath()")		
end

function animpath()
	selection = getselection()
	printconsole("<NebulaT>\n<Track name=\"track\" rotationInfinity=\"Cycle\" scalingInfinity=\"Cycle\" translationInfinity=\"Constant\">")
	count = 0.0
	for i = 0, #selection do			
		mat = mat4(gettransform(selection[i]))			
		pos = mat:getposition()
		rot = getorientation(selection[i])	
		msg = "<Translate to=\"" .. pos[1] .. "," .. pos[2] .. "," .. pos[3] .. ",0.0\" start=\"" .. count .."\" end=\"" .. count+1.0 .. "\"/>"
		rmsg = "<Rotate to=\"" .. rot[1] .. ", " .. rot[2] .. ", " .. rot[3] .. ",0.0\" start=\"" .. count .."\" end=\"" .. count+1.0 .. "\"/>"
		printconsole(msg)				
		printconsole(rmsg)	
		count = count + 1.0
	end
	printconsole("</Track>\n</NebulaT>")
end
