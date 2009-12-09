Renderer = Object:clone()

local function errmeta(name)
	local meta = {}
	function meta:__index(k)
		error("No definition for renderop '"..tostring(k).."' in renderer '"..name.."'")
	end
	return meta
end
local function errtable(name)
	return setmetatable({}, errmeta(name))
end

function Renderer:start(options)
	print("Renderer Begin")
end

function Renderer:finish()
	print("Renderer End")
end

function Renderer:format(xres, yres, aspect_ratio)
	print("Format "..xres.." "..yres.." "..aspect_ratio)
end

function Renderer:frameStart(frameNo)
	print("Frame "..frameNo)
end

function Renderer:frameEnd()
	print("Frame Done")
end

function Renderer:display(options)
	print("Display "..tostring(options))
end

function Renderer:shutter(open, close)
	print("Shutter "..open.." "..close)
end

function Renderer:renderIt(options)
	if type(options.world) ~= "table" then
		error("no world")
	elseif type(options.camera) ~= "table" then
		error("no camera")
	end

	local start = options.start or World.startTime 
	local stop = options.stop or World.endTime 
	local incr = options.incr or 1
	self:start(options)
	self:format(options)
	for frame = start, stop, incr do
		self:frameStart(frame)
		self:display(options)
		local f1 = frame
		local times = { f1 }
		local framestates = { { transforms = {} } }
		if options.motion_blur then
			self:shutter(frame, frame+options.motion_blur)
			local f2 = frame+1
			times = { f1, f2 }
			-- TODO: These should really be objects
			framestates[2] = { transforms = {} }
		end

		-- Pass 0 : Prep transforms
		do
			-- Camera
			local renderables = options.camera:getRenderables(times)
			self:renderRenderables(renderables, times, framestates, 0, function(node) return Renderable.transformNodes[node.renderop] end)

			-- World
			renderables = options.world:getRenderables(times)
			self:renderRenderables(renderables, times, framestates, 0, function(node) return Renderable.transformNodes[node.renderop] end)
		end

		-- Pass 1 : Render
		do
			-- Camera
			local renderables = options.camera:getRenderables(times)
			self:renderRenderables(renderables, times, framestates, 1)

			-- World
			renderables = options.world:getRenderables(times)
			self:renderRenderables(renderables, times, framestates, 1)
		end
		self:frameEnd()
	end
	self:finish()
end


--[[ Render a set of frames.
--   parameter: renderables   a list of lists of Renderables, one list for
--   						  each frame.
--   parameter: times		  a list of times, one time for each frame.
--]]
function Renderer:renderRenderables(renderables, times, framestates, pass, filter)
	for i,ro in ipairs(renderables[1]) do
		if ro.group then
			local subRenderables = {}
			local f0 = ro:generate(times[1])
			table.insert(subRenderables, f0)
			for frame = 2, #renderables do
				local fn = renderables[frame][i]:generate(times[frame])
				table.insert(subRenderables, fn)
			end
			self:renderRenderables(subRenderables, times, framestates, pass, filter)
		else
			if ( filter == nil or type(filter) ~= "function" ) or filter(ro) then
				local same = true
				for frame = 2, #renderables do
					if not ro:isEquivalent(renderables[frame][i]) then
						print("Error: the rob lists for frames 1 and " .. frame .. " aren't compatible")
					end
					if not ro:isEqual(renderables[frame][i]) then
						same = false
						break
					end
				end
				if same then
					self:render(ro, framestates[1], pass)
				else
					ri.MotionBegin(times)
					self:render(ro, framestates[1])
					for frame = 2, #renderables do
						self:render(renderables[frame][i], framestates[frame], pass)
					end
					ri.MotionEnd()
				end
			end
		end
	end
end

function Renderer:initTransform()
	self.matrixStack = {}
	table.insert(self.matrixStack, matrix(4, "I"))
end
function Renderer:pushTransform()
	currMat = self.matrixStack[#self.matrixStack]
	table.insert(self.matrixStack, matrix(currMat))
end
function Renderer:popTransform()
	table.remove(self.matrixStack)
end
function Renderer:setTransform(mat)
	self.matrixStack[#self.matrixStack] = mat or matrix(4, "I")
end
function Renderer:translate(x, y, z)
	local transMat = matrix(4, "I")
	transMat[4][1] = x
	transMat[4][2] = y
	transMat[4][3] = z
	self.matrixStack[#self.matrixStack] = matrix.mul(transMat, self.matrixStack[#self.matrixStack])
end
function Renderer:rotate(angle, x, y, z)
	local rotMat = matrix(4, "I")
	local c = math.cos(math.rad(angle))
	local s = math.sin(math.rad(angle))
	local l = math.sqrt(x*x+y*y+z*z)
	local ux = x / l
	local uy = y / l
	local uz = z / l
	local ux2 = ux*ux
	local uy2 = uy*uy
	local uz2 = uz*uz
	rotMat[1][1] = ux2+(1.0-ux2)*c
	rotMat[2][1] = ux*uy*(1.0-c)-uz*s
	rotMat[3][1] = ux*uz*(1.0-c)+uy*s
	rotMat[1][2] = ux*uy*(1.0-c)+uz*s
	rotMat[2][2] = uy2+(1.0-uy2)*c
	rotMat[3][2] = uy*uz*(1.0-c)-ux*s
	rotMat[1][3] = ux*uz*(1.0-c)-uy*s
	rotMat[2][3] = uy*uz*(1.0-c)+ux*s
	rotMat[3][3] = uz2+(1.0-uz2)*c
	self.matrixStack[#self.matrixStack] = matrix.mul(rotMat, self.matrixStack[#self.matrixStack])
end
function Renderer:scale(x, y, z)
	local scaleMat = matrix(4, "I")
	scaleMat[1][1] = x
	scaleMat[2][2] = y
	scaleMat[3][3] = z
	self.matrixStack[#self.matrixStack] = matrix.mul(scaleMat, self.matrixStack[#self.matrixStack])
end

function Renderer:create(name)
	local r = self:clone()
	local tab = errtable(name)

	r.matrixStack = {}
	table.insert(r.matrixStack, matrix(4, "I"))


	function r:render(nn, framestate, pass)
		return tab[nn.renderop](nn, framestate, pass)
	end
	function tab:WorldBegin(framestate, pass)
		print(name.." -- WorldBegin")
	end
	function tab:WorldEnd(framestate, pass)
		print("WorldEnd")
	end
	function tab:TransformBegin(framestate, pass)
		print("TransformBegin")
	end
	function tab:TransformEnd(framestate, pass)
		print("TransformEnd")
	end
	function tab:Translate(framestate, pass)
		print("Translate "..self.x.." "..self.y.." "..self.z)
	end
    function tab:Scale(framestate, pass)
        print("Scale "..self.x.." "..self.y.." "..self.z)
    end
    function tab:Rotate(framestate, pass)
		print("Rotate "..self.angle.." "..self.x.." "..self.y.." "..self.z)
	end
	function tab:Cylinder(framestate, pass)
		print("Cylinder "..self.radius.." "..self.zmin.." "..self.zmax.." "..self.thetamax)
	end
	function tab:Sphere(framestate, pass)
		print("Sphere "..self.radius.." "..self.zmin.." "..self.zmax.." "..self.thetamax)
	end
	function tab:Disk(framestate, pass)
		print("Disk "..self.height.." "..self.radius.." "..self.thetamax)
	end
	function tab:PatchMesh(framestate, pass)
		print("PatchMesh "..self.type.." "..self.nu.." "..self.uwrap.." "..self.nv.." "..self.vwrap.." "..tostring(self.paramList))
	end
	function tab:Polygon(framestate, pass)
		print("Polygon "..tostring(self.paramList))
	end
	function tab:Projection(framestate, pass)
		print("Projection "..self.type.." "..tostring(self.paramList))
	end
	function tab:CoordinateSystem(framestate, pass)
		print("CoordinateSystem "..self.name)
	end
	function tab:CoordSysTransform(framestate, pass)
		print("CoordSysTransform "..self.name)
	end
	function tab:RecordTransform(framestate, pass)
		print("RecordTransform "..self.name)
	end
	function tab:RestoreTransform(framestate, pass)
		print("RestoreTransform "..self.name)
	end
	return r, tab
end

