Model = Renderable:clone()
Model.nodeType = "model"
Model.group = true 


function Model:generate(time)
    Renderable:pushState()
    if(self.body) then
        self:body(time)
    end
    return Renderable:popState()
end 

function Model:getRenderables(atTime)
	local renderables = {}
	for i,t in ipairs(atTime) do
		local fn = self:generate(t)
		table.insert(renderables, fn)
	end
	return renderables
end

function Model:avar(name, keyframes)
    local o = self.avars[name] or Avar:create(name, keyframes or {})
    self.avars[name] = o
    return o
end

function Model:setBody(string)
    local func = assert(loadstring("return function(self, time) "..string.." end"))
    self.body = func()
    self.bodySource = string
end

function Model:model(name)
    if type(name) ~= "string" then
        error("Must specify a name for a model")
        return
    end
	if self.children[name] then
		return self.children[name]
	end
    local o = Model:clone()
    o.name = name
    o.avars = {}
	o:setBody("TransformBegin()\nTransformEnd()")
	o.passes = { true, true, true }
	o.children = {}
	o.parent = self
	o.nodeType = "model"
    function o:__call()
        self:create()
    end
    self.children[name] = o
    return o
end


function Model:camera(name)
    if type(name) ~= "string" then
        error("Must specify a name for a camera")
        return
    end
	if self.children[name] then
		return self.children[name]
	end
    local o = Camera:clone()
    o.name = name
    o.avars = {}
    o:setBody("Projection(\"perspective\", {fov = 45})")
	o.passes = { true, true, false }
	o.nodeType = "camera"
	o.parent = self
    function o:__call()
        self:create()
    end
    self.children[name] = o
	Cameras[name] = o
    return o
end

function Model:location()
	local l = ""
	if self.parent then
		l = self.parent:location()
	end
	return l .. "/" .. self.name
end

function Model.find(...)
	local node
	local locator
	if select('#', ...) == 1 then
		node = root
		locator = ... 
	elseif select('#', ...) == 2 then
		node = ...
		locator = select(1, ...)
	else
		error("Invalid arguments to find")
	end
	-- If the locator begins with '/' then search from the world root.
	if string.find(locator, '/') == 1 then
		locator = string.sub(locator, 2)
		node = root
	end
	for n in string.gmatch(locator, "(%w+)/?") do
		if node.children[n] then
			node = node.children[n]
		else
			error("Invalid locator ("..locator..") node "..node.name.." doesn't have a child named "..n)
		end
	end
	return node	
end


--[[
Static utility functions
]]--

function Model.new(name, parent)
    if type(name) ~= "string" then
        error("Must specify a name for a model")
        return
    end
    local o = Model:clone()
    -- Record where the model was generated from.
    deb = debug.getinfo(2)
    o.source = deb['short_src']
    o.line = deb['currentline']
    o.name = name
    o.avars = {}
    o.body = nil
    function o:__call()
        self:create()
    end
    parent.children[name] = o
    return o
end


function Model.aim(from, to)
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

    Translate(from[1], from[2], from[3])

    if dirx <= 0 then
            Rotate(-yrot, 0, 1, 0)
    else
            Rotate(yrot, 0, 1, 0)
    end

    if diry <= 0 then
            Rotate(xrot, 1, 0, 0)
    else
            Rotate(-xrot, 1, 0, 0)
    end
end

return Model

