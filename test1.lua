require('Model')
require('World')


-- A function to create a new model based on some control parameters.
function createTube(radius, zmin, zmax) 
	local tube = Model("tube")
	-- Create a single avar for the thetamax, initialised with 2 keyframes.
	tube:createAvar("thetamax", {{0.0, 0.0}, {1.0, 360.0}})

	-- The body of the model, referencing the control params, and the avar.
	function tube:body(time)
		TransformBegin()
			Cylinder(radius, zmin, zmax, self.thetamax(time))
		TransformEnd()
	end

	-- 'call' the model to get it instantiated in the render tree.
	return tube()
end

-- Create a simple model with no control parameters, stored in a global variable.
ball = Model("ball")
-- A single avar for the thetamax, with 3 keyframes.
ball:createAvar("thetamax", {{0.0, 0.0}, {0.5, 0.0}, {1.0, 360.0}})
-- The body, referencing the avar.
function ball:body(time)
	TransformBegin()
		Sphere(radius, zmin, zmax, self.thetamax(time))
	TransformEnd()
end

-- Create a world.
theWorld = World()
-- The body of the world, this will be called by the rendering machinery to
-- produce the renderable objects that get passed onto the renderer.
function theWorld:body(time)
	for o = 0, 10 do 
		TransformBegin()
			Translate(o,0,0)
			Rotate(180 + time * 90, 1, 0, 0)
			-- Call our function to create a new 'tube' object with specific settings.
			createTube(1.0, -2, 2)
			-- again, with different settings.
			createTube(0.1, -5, 0)
			-- Simply instance our ball model, again, 'call' it to get it instantiated in the tree.
			ball()
			-- And again, note they are different instances, and their avars can be controlled
			-- separately.
			ball()
		TransformEnd()
	end
end

-- Render a single frame of the world, with motion keys at 0.2, 0.5 and 0.8.
theWorld:render({0.2, 0.5, 0.8})

print(os.clock())
