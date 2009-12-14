World = Model:clone()
World.time = 0
World.startTime = 0
World.endTime = 50
World.children = {}
World.name = "World"

function World:generate(time)
	Renderable:pushState()
	WorldBegin()
	if(self.body) then
		self:body(time or self.time)
	end
	WorldEnd()
	return Renderable:popState()
end

function World:getRenderables(atTime)
	local renderables = {}
	for i,t in ipairs(atTime) do
		local fn = self:generate(t)
		table.insert(renderables, fn)
	end
	return renderables
end

function World:setBody(string)
    local func = assert(loadstring("return function(self, time) "..string.." end"))
    self.body = func()
    self.bodySource = string
end
