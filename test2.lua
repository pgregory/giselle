
function createBlade(length, width, thickness)
	blade = Model("blade")
        blade.width = width
        blade.length = length
        blade.thickness = thickness
	blade:setBody(
        [[
local left = -self.width / 2
local right = self.width / 2
local top = -self.thickness / 2
local bottom = self.thickness / 2
TransformBegin()
        Translate(0, 0, -self.length/2)
        TransformBegin()
                Rotate(90, 1, 0, 0)
                Rotate(180, 0, 0, 1)
                Cylinder(self.width/2, top, bottom, 180)
                Disk(top, self.width/2, 180)
                Disk(bottom, self.width/2, 180)
        TransformEnd()
        Polygon({ P = {left, top, 0, left, top, self.length, right, top, self.length, right, top, 0}})
        Polygon({ P = {right, top, 0, right, top, self.length, right, bottom, self.length, right, bottom, 0}})
        Polygon({ P = {right, bottom, 0, right, bottom, self.length, left, bottom, self.length, left, bottom, 0}})
        Polygon({ P = {left, bottom, 0, left, bottom, self.length, left, top, self.length, left, top, 0}})
        TransformBegin()
                Translate(0,0,self.length)
                Rotate(90, 1, 0, 0)
                Cylinder(self.width/2, top, bottom, 180)
                Disk(top, self.width/2, 180)
                Disk(bottom, self.width/2, 180)
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
createBlade(scissor.bladelength, scissor.bladewidth, scissor.bladethickness)

scissor:setBody(
[[
local minangle = (self.bladewidth / self.bladelength) * (180/math.pi)
local maxangle = 90 - minangle
for segment = 0, self.segments do
    local angle = 90 - math.min(math.max((90 * self.extension(time)), minangle), maxangle)
    local offset = self.bladelength * math.sin(math.rad(90 - angle))
    TransformBegin()
        Rotate(-angle, 0, 1, 0)
        Models["blade"]()
        Rotate(2 * angle, 0, 1, 0)
        Translate(0, self.bladethickness, 0)
        Models["blade"]()
    TransformEnd()
    Translate(0, 0, offset)
end
]])

theWorld = World()
function theWorld:body(time)
    Models["scissor"]()
end

cam = Camera("main")
cam.pan = cam:avar("pan", {{0.0, 180.0}, {30.0, 270.0}})
cam.dolly = cam:avar("dolly", {{0.0, 4.0}, {15.0, 4.0}, {30.0, 8.0}})
cam:setBody(
[[
Projection("perspective", {fov = 50})
Translate(0,0,self.dolly(time))
Rotate(-50, 1,1,0)
Rotate(self.pan(time),0,1,0)
Translate(0,0,-7)
]])

theGLRenderer = GLRenderer:create("test")
