Model = Renderable:clone()
Model.group = true 


function Model:generate(time)
    Renderable:pushState()
    if(self.body) then
        self:body(time)
    end
    return Renderable:popState()
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
    o.body = nil
	o.children = {}
    function o:__call()
        self:create()
    end
    self.children[name] = o
    return o
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

return Model

