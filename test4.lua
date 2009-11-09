ball = Model("ball")
ball.radius = ball:avar("radius", {{0,0}, {30,1}})

function ball:body(time)
	TransformBegin()
		local r = self.radius(time)
		Sphere(r, -r, r, 360)
	TransformEnd()
end

dumbell = Model("dumbell")
dumbell.length = 2
function dumbell:body(time)
	TransformBegin()
		Translate(0,0,-self.length/2)
		ball()
	TransformEnd()
	Cylinder(0.1, -self.length/2, self.length/2, 360)
	TransformBegin()
		Translate(0,0,self.length/2)
		ball()
	TransformEnd()
end
	

theCamera = Camera("main")

function theCamera:body(time)
	Projection("perspective", {fov = 50})
	Translate(0,0,5)
	Rotate(90, 1, 0, 0)
end

theWorld = World()
function theWorld:body(time)
	dumbell()
end

renderer = RenderMan:create("aqsis")

renderer:renderIt{world = theWorld, camera=theCamera, start=15, stop=15, ribfile="dumbell.rib", display_type="framebuffer", display_mode="rgb", xres=320, yres=240}
