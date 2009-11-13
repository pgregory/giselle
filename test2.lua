
function createBlade(length, width, thickness)
	blade = Model("blade")
	blade:setBody(
        [[
local left = - width / 2
local right = width / 2
local top = - thickness / 2
local bottom = thickness / 2
TransformBegin()
        Translate(0, 0, -length/2)
        TransformBegin()
                Rotate(90, 1, 0, 0)
                Rotate(180, 0, 0, 1)
                Cylinder(width/2, top, bottom, 180)
                Disk(top, width/2, 180)
                Disk(bottom, width/2, 180)
        TransformEnd()
        Polygon({ P = {left, top, 0, left, top, length, right, top, length, right, top, 0}})
        Polygon({ P = {right, top, 0, right, top, length, right, bottom, length, right, bottom, 0}})
        Polygon({ P = {right, bottom, 0, right, bottom, length, left, bottom, length, left, bottom, 0}})
        Polygon({ P = {left, bottom, 0, left, bottom, length, left, top, length, left, top, 0}})
        TransformBegin()
                Translate(0,0,length)
                Rotate(90, 1, 0, 0)
                Cylinder(width/2, top, bottom, 180)
                Disk(top, width/2, 180)
                Disk(bottom, width/2, 180)
        TransformEnd()
TransformEnd()
]])
	return blade
end

scissor = Model("scissor")
scissor.extension = scissor:avar("extension", {{0.0, 0.0}, {10.0, 0.0}, {20.0, 0.5}, {25.0, 0.9}, {30.0, 1.0}})
scissor.segments = 5
scissor.bladelength = 2
scissor.bladewidth = 0.3
scissor.bladethickness = 0.1
scissor.blade = createBlade(scissor.bladelength, scissor.bladewidth, scissor.bladethickness)

scissor:setBody(
[[
local minangle = (self.bladewidth / self.bladelength) * (180/math.pi)
local maxangle = 90 - minangle
for segment = 0, self.segments do
        local angle = 90 - math.min(math.max((90 * self.extension(time)), minangle), maxangle)
        local offset = self.bladelength * math.sin(math.rad(90 - angle))
        TransformBegin()
                Rotate(-angle, 0, 1, 0)
                self.blade()
                Rotate(2 * angle, 0, 1, 0)
                Translate(0, self.bladethickness, 0)
                self.blade()
        TransformEnd()
        Translate(0, 0, offset)
end
]])

theWorld = World()
function theWorld:body(time)
	scissor()
end

theCamera = Camera("main")
theCamera.pan = theCamera:avar("pan", {{0.0, 180.0}, {30.0, 270.0}})
theCamera.dolly = theCamera:avar("dolly", {{0.0, 4.0}, {15.0, 4.0}, {30.0, 8.0}})
theCamera:setBody(
[[
Projection("perspective", {fov = 50})
Translate(0,0,self.dolly(time))
Rotate(-50, 1,1,0)
Rotate(self.pan(time),0,1,0)
Translate(0,0,-7)
]])


theRenderer = RenderMan:create("aqsis")

theRenderer:renderIt{world=theWorld, camera=theCamera, 
					 start=0, stop=30, incr=1,-- motion_blur=0.5,
					 ribfile="scissor.rib",
					 display_name="scissor_~s.tif",
					 display_type="file",
					 display_mode="rgb",
					 xres = 320,
					 yres = 240}

print(os.clock())
