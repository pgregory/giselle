World = Model:clone()
World.time = 0
World.startTime = 0
World.endTime = 50

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

function World:setBody(string)
    local func = assert(loadstring("return function(self, time) "..string.." end"))
    self.body = func()
    self.bodySource = string
end
