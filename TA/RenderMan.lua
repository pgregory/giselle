RenderMan = Object:clone()

function RenderMan:create(name)
	local r, tab = Renderer:create(name)

	function tab:WorldBegin()
		ri.WorldBegin()
	end
	function tab:WorldEnd()
		ri.WorldEnd()
	end
	function tab:TransformBegin()
		ri.TransformBegin()
	end
	function tab:TransformEnd()
		ri.TransformEnd()
	end
	function tab:Translate()
		ri.Translate(self.x, self.y, self.z)
	end
    function tab:Scale()
        ri.Scale(self.x, self.y, self.z)
    end
    function tab:Rotate()
		ri.Rotate(self.angle, self.x, self.y, self.z)
	end
	function tab:Cylinder()
		ri.Cylinder(self.radius, self.zmin, self.zmax, self.thetamax)
	end
	function tab:Sphere()
		ri.Sphere(self.radius, self.zmin, self.zmax, self.thetamax)
	end
	function tab:Disk()
		ri.Disk(self.height, self.radius, self.thetamax)
	end
	function tab:PatchMesh()
		ri.PatchMesh(self.type, self.nu, self.uwrap, self.nv, self.vwrap, self.paramList)
	end
	function tab:Projection()
		ri.Projection(self.type, self.paramList)
	end
	function tab:Polygon()
		ri.Polygon(#self.paramList["P"]/3, self.paramList)
	end
	function tab:CoordinateSystem()
		ri.CoordinateSystem(self.name)
	end
	function tab:CoordSysTransform()
		ri.CoordSysTransform(self.name)
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


