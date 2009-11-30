Camera = Model:clone()
Cameras = {}

function Camera:first()
	if #self.cameras == 0 then
		return nil
	else
		local i, cam = next(self.cameras)
		return cam
	end
end

function Camera:generate(time)
	Renderable:pushState()
	if(self.body) then
		self:body(time or self.time)
	end
	return Renderable:popState()
end

function Camera:getRenderables(atTime)
	if type(atTime) == "table" then
		local renderables = {}
		for i,t in ipairs(atTime) do
			local fn = self:generate(t)
			table.insert(renderables, fn)
		end
		return renderables
	else
		local f1 = self:generate(atTime or self.time)
		return {f1}
	end
end

--[[
Static utility functions.
]]--

function Camera.new(name)
	if type(name) ~= "string" then
		error("Must specify a name for the camera")
		return
	end
	local c = Camera:clone()
	c.name = name
	c.avars = {}
        Cameras[name] = c
	return c
end


function Camera.aim(from, to)
    if type(from) ~= "table" or type(to) ~= "table" then
        error("Parameters not valid")
    end
    local dirx = to[1] - from[1]
    local diry = to[2] - from[2]
    local dirz = to[3] - from[3]

    local xzlen = math.sqrt(dirx*dirx + dirz*dirz)
    local yrot
    if xzlen == 0 then
            yrot = diry > 0 and 180 or 0
    else
            yrot = 180*math.acos(dirz/xzlen)/math.pi
    end

    local yzlen = math.sqrt(diry*diry + xzlen*xzlen)
    local xrot = 180*math.acos(xzlen/yzlen)/math.pi

    if diry > 0 then
            Rotate(xrot, 1, 0, 0)
    else
            Rotate(-xrot, 1, 0, 0)
    end

    if dirx > 0 then
            Rotate(-yrot, 0, 1, 0)
    else
            Rotate(yrot, 0, 1, 0)
    end

    Translate(-from[1], -from[2], -from[3])
end


Cameras_meta = {}

function Cameras_meta:__index(key)
    camera = rawget(self, key)
    if camera and type(camera) == "table" then
        return camera
    else
        error("Camera \""..tostring(key).."\" does not exist")
    end
end

setmetatable(Cameras, Cameras_meta)
