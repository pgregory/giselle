Lightsource = Model:clone()
Lightsources = {}
Lightsource.group = false

Lightsource.renderop = "Lightsource"


function Lightsource:generate(time)
	Renderable:pushState()
	if(self.body) then
		self:body(time or self.time)
	end
	return Renderable:popState()
end

function Lightsource:getRenderables(atTime)
	local renderables = {}
	for i,t in ipairs(atTime) do
		local fn = self:generate(t)
		table.insert(renderables, fn)
	end
	return renderables
end

--[[
Static utility functions.
]]--

function Lightsource.new(name)
	if type(name) ~= "string" then
		error("Must specify a name for the lightsource")
		return
	end
	local l = Lightsource:clone()
	l.name = name
	l.avars = {}
        Lightsources[name] = l
	return l
end


function Lightsource.aim(from, to)
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


Lightsources_meta = {}

function Lightsources_meta:__index(key)
    light = rawget(self, key)
    if light and type(light) == "table" then
        return light
    else
        error("Lightsource \""..tostring(key).."\" does not exist")
    end
end

setmetatable(Lightsources, Lightsources_meta)
