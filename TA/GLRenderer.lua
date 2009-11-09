GLRenderer = Object:clone()

function GLRenderer:create(name, canvas)
	local r, tab = Renderer:create(name)

	r.canvas = canvas

	function tab:WorldBegin()
		gl.MatrixMode('MODELVIEW')
		gl.LoadIdentity()
	end
	function tab:WorldEnd()
	end
	function tab:TransformBegin()
		iup.GLMakeCurrent(r.canvas)
		gl.PushMatrix()
	end
	function tab:TransformEnd()
		iup.GLMakeCurrent(r.canvas)
		gl.PopMatrix()
	end
	function tab:Translate()
		iup.GLMakeCurrent(r.canvas)
		gl.Translate(self.x, self.y, self.z)
	end
	function tab:Rotate()
		iup.GLMakeCurrent(r.canvas)
		gl.Rotate(self.angle, self.x, self.y, self.z)
	end
	function tab:Cylinder()
		iup.GLMakeCurrent(r.canvas)
		local quad = glu.NewQuadric()
		gl.PushMatrix()
		gl.Translate(0, 0, self.zmin)
		glu.Cylinder(quad, self.radius, self.radius, self.zmax-self.zmin, 12, 1)
		gl.PopMatrix()
	end
	function tab:Sphere()
		iup.GLMakeCurrent(r.canvas)
		local quad = glu.NewQuadric()
		glu.Sphere(quad, self.radius, 12, 6)
	end
	function tab:Disk()
		iup.GLMakeCurrent(r.canvas)
		local quad = glu.NewQuadric()
		glu.Disk(quad, 0, self.radius, 12, 1)
	end
	function tab:PatchMesh()
		iup.GLMakeCurrent(r.canvas)
		gl.Begin('TRIANGLES')         -- Drawing Using Triangles
		gl.Vertex( 0, 0.3, 0)         -- Move Up One Unit From Center (Top Point)
		gl.Vertex(-0.3,-0.3, 0)         -- Left And Down One Unit (Bottom Left)
		gl.Vertex( 0.3,-0.3, 0)         -- Right And Down One Unit (Bottom Right)
		gl.End()                      -- Done Drawing A Triangle
	end
	function tab:Polygon()
		iup.GLMakeCurrent(r.canvas)
		gl.Begin('POLYGON')
		if self.paramList["P"] then
			for i=1, #self.paramList["P"], 3 do
				gl.Vertex(self.paramList["P"][i], self.paramList["P"][i+1], self.paramList["P"][i+2])
			end
		end
		gl.End()
	end
	function tab:Projection()
		iup.GLMakeCurrent(r.canvas)
		gl.MatrixMode('PROJECTION')
		glu.Perspective(40, 320/240, 1, 5000)
		gl.Scale(1, 1, -1)
	end

	function r:start(options)
		iup.GLMakeCurrent(self.canvas)

		gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT') -- Clear Screen And Depth Buffer
		gl.MatrixMode('PROJECTION')   -- Select The Projection Matrix
		gl.LoadIdentity()             -- Reset The Projection Matrix
	end

	function r:finish()
		iup.GLSwapBuffers(self.canvas)
	end

	function r:format(options)
	end

	function r:frameStart(frameNo)
	end

	function r:frameEnd()
	end

	function r:display(options)
	end

	function r:shutter(open, close)
	end

	return r, tab
end


