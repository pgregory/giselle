Model = Renderable:clone()
Model.group = true 


Models = {}

function Model:generate(time)
    Renderable:pushState()
    if(self.body) then
        self:body(time)
    end
    return Renderable:popState()
end 

function Model:avar(name, keyframes)
    local o = Avar:create(name, keyframes or {})
    self.avars[name] = o
    return o
end

function Model:setBody(string)
    local func = assert(loadstring("return function(self, time) "..string.." end"))
    self.body = func()
    self.bodySource = string
end

function Model:__call(name)
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
    Models[name] = o
    return o
end

Models_meta = {}
function Models_meta:__index(key)
    model = rawget(Models, key)
    if model and type(model) == "table" then
        return model
    else
        error("Model \""..tostring(key).."\" does not exist")
    end
end

setmetatable(Models, Models_meta)

return Model

