World = Model:clone()
World.time = 0

function World:generate(time)
	Renderable:pushState()
	WorldBegin()
	if(self.body) then
		self:body(time or self.time)
	end
	WorldEnd()
	return Renderable:popState()
end

function World:render(renderer, atTime)
	if type(atTime) == "table" then
		local renderables = {}
		for i,t in ipairs(atTime) do
			local fn = self:generate(t)
			table.insert(renderables, fn)
		end
		renderer:renderRenderables(renderables, atTime)
	else
		local f1 = self:generate(atTime or self.time)
		renderer:renderRenderables({f1}, {atTime or self.time})
	end
end

function World:__call()
	local w = World:clone()
	w.avars = {}
	return w
end
