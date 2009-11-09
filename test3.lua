
ball = Model("ball")

function ball:body(time)
	Sphere(1, -1, 1, 360)
end

theWorld = World()
function theWorld:body(time)
	for i=0, 10  do
		Translate(0.2,0,0)
		ball()
	end
end

theCamera = Camera()
function theCamera:body(time)
	Projection("perspective", {fov = 50})
	Translate(0,0,7)
end


theRenderer = RenderMan:create("aqsis")

theRenderer:renderIt{world=theWorld, camera=theCamera, 
					 start=0, stop=5, incr=1, motion_blur=0.5,
					 ribfile="scissor.rib",
					 display_name="scissor_~s.tif",
					 display_type="framebuffer",
					 display_mode="rgb",
					 xres = 320,
					 yres = 240}

print(os.clock())
