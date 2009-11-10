Model = Renderable:clone()
Model.group = true 
Model.models = {}

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
	loadstring("self.body = function(time) "..string.."end")
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
		return self:create()
	end
	Model.models[name] = o
	return o
end

return Model

