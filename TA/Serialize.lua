function fixupScene(model)
	if model.children then
		for k,v in pairs(model.children) do
			if v.nodeType == "model" then
				v = Model:clone(v)
			elseif v.nodeType == "camera" then
				v = Camera:clone(v)
			end
			for k,v in pairs(v.avars) do
				setmetatable(v, Avar)
			end
			fixupScene(v)
		end
	end
end

function loadAll(name)
	local m = table.load(name)
	if m.Cameras and m.World then
		-- Now that we have the basic shell
		-- tables for all the items, we need
		-- to relink them into the framework
		-- to get access to the common functionality.
		-- This basically involves repairing the 
		-- metatables for the cameras/models/avars
		-- and the world.
		for k,v in pairs(m.Cameras) do
			v = Camera:clone(v)
			for k,v in pairs(v.avars) do
				setmetatable(v, Avar)
			end
		end
		-- If this is all successful, then we can
		-- update the current values for the Models
		-- Cameras and World with the loaded data.
		m.World = World:clone(m.World)
		fixupScene(m.World)
		Cameras = m.Cameras
		World = m.World
		io.stderr:write(World.bodySource)
	else
		error("This doesn't appear to be a valid system save")
	end
end


function saveAll(name)
	local m = {}
	m.Models = Models
	m.Cameras = Cameras
	m.World = World
	table.save(m, name)
end
