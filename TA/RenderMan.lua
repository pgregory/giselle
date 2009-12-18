RenderMan = Object:clone()

function RenderMan:create(name)
	local r, tab = Renderer:create(name)

	function tab:WorldBegin(framestate, pass)
		Renderer.initTransform(r)
		if pass < 3 then return end
		ri.WorldBegin()
	end
	function tab:WorldEnd(framestate, pass)
		if pass < 3 then return end
		ri.WorldEnd()
	end
	function tab:TransformBegin(framestate, pass)
		Renderer.pushTransform(r)
		if pass < 2 then return end
		ri.TransformBegin()
	end
	function tab:TransformEnd(framestate, pass)
		Renderer.popTransform(r)
		if pass < 2 then return end
		ri.TransformEnd()
	end
	function tab:Translate(framestate, pass)
		Renderer.translate(r, self.x, self.y, self.z)
		if pass < 2 then return end
		ri.Translate(self.x, self.y, self.z)
	end
    function tab:Scale(framestate, pass)
		Renderer.scale(r, self.x, self.y, self.z)
		if pass < 2 then return end
        ri.Scale(self.x, self.y, self.z)
    end
    function tab:Rotate(framestate, pass)
		Renderer.rotate(r, self.angle, self.x, self.y, self.z)
		if pass < 2 then return end
		ri.Rotate(self.angle, self.x, self.y, self.z)
	end
	function tab:Cylinder(framestate, pass)
		ri.Cylinder(self.radius, self.zmin, self.zmax, self.thetamax)
	end
	function tab:Sphere(framestate, pass)
		ri.Sphere(self.radius, self.zmin, self.zmax, self.thetamax)
	end
	function tab:Disk(framestate, pass)
		ri.Disk(self.height, self.radius, self.thetamax)
	end
	function tab:PatchMesh(framestate, pass)
		ri.PatchMesh(self.type, self.nu, self.uwrap, self.nv, self.vwrap, self.paramList)
	end
	function tab:Projection(framestate, pass)
		ri.Projection(self.type, self.paramList)
	end
	function tab:Polygon(framestate, pass)
		ri.Polygon(#self.paramList["P"]/3, self.paramList)
	end
	function tab:CoordinateSystem(framestate, pass)
		ri.CoordinateSystem(self.name)
	end
	function tab:CoordSysTransform(framestate, pass)
		ri.CoordSysTransform(self.name)
	end
	function tab:LightSource(framestate, pass)
		ri.LightSource(self.shadername, self.paramList)
	end
	function tab:RecordTransform(framestate, pass)
		framestate["transforms"][self.name] = r.matrixStack[#r.matrixStack]
	end
	function tab:RestoreTransform(framestate, pass)
		local mat = framestate["transforms"][self.name]
		Renderer.setTransform(r, mat)
		if pass < 2 then return end
		ri.Transform(mat or matrix(4, "I"))
	end

	function r:start(options)
		local file = "test.rib"
		if type(options) == "table" then
			if type(options.ribfile) == "string" then
				file = options.ribfile
			end
		end
		self.currentFrame = 0
			
		ri.Begin(file)
	end

	function r:finish()
		ri.End()
	end

	function r:format(options)
		if not options then
			error("Must specify options to format")
			return
		end
		local xres = options.xres or 640
		local yres = options.yres or 480
		local aspect_ratio = options.aspect_ratio or 1

		ri.Format(xres, yres, aspect_ratio)
	end

	function r:frameStart(frameNo)
		self.currentFrame = frameNo
		ri.FrameBegin(frameNo)
		ri.Identity()
	end

	function r:frameEnd()
		ri.FrameEnd()
	end

	function r:display(options)
		if not options then
			error("Must specify options to format")
			return
		end
		local display_name = options.display_name or "test.tif"
		local display_type = options.display_type or "file" 
		local display_mode = options.display_mode or "rgb"
		display_name = string.gsub(display_name, "~s", tostring(self.currentFrame))
		ri.Display(display_name, display_type, display_mode)
	end

	function r:shutter(open, close)
		ri.Shutter(open, close)
	end

	return r, tab
end


