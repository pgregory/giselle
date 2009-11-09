Camera = Model:clone()
Camera.cameras = {}

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

function Camera:render(renderer, atTime)
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

function Camera:__call(name)
	if type(name) ~= "string" then
		error("Must specify a name for the camera")
		return
	end
	local c = Camera:clone()
	c.name = name
	c.avars = {}
	Camera.cameras[name] = c
	return c
end

