GLRenderer = Object:clone()
GLRenderer.mode = 'LINES'

function GLRenderer:create(name)
	local r, tab = Renderer:create(name)

	function tab:WorldBegin()
		gl.MatrixMode('MODELVIEW')
		gl.LoadIdentity()
	end
	function tab:WorldEnd()
	end
	function tab:TransformBegin()
		gl.PushMatrix()
	end
	function tab:TransformEnd()
		gl.PopMatrix()
	end
	function tab:Translate()
		gl.Translate(self.x, self.y, self.z)
	end
	function tab:Rotate()
		gl.Rotate(self.angle, self.x, self.y, self.z)
	end
	function tab:Cylinder()
        local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
        gl.PushMatrix()
		gl.Translate(0, 0, self.zmin)
		glu.Cylinder(quad, self.radius, self.radius, self.zmax-self.zmin, 12, 1)
		gl.PopMatrix()
	end
	function tab:Sphere()
		local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
        glu.Sphere(quad, self.radius, 12, 6)
	end
	function tab:Disk()
		local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
        glu.Disk(quad, 0, self.radius, 12, 1)
	end
	function tab:PatchMesh()
		gl.Begin('TRIANGLES')         -- Drawing Using Triangles
		gl.Vertex( 0, 0.3, 0)         -- Move Up One Unit From Center (Top Point)
		gl.Vertex(-0.3,-0.3, 0)         -- Left And Down One Unit (Bottom Left)
		gl.Vertex( 0.3,-0.3, 0)         -- Right And Down One Unit (Bottom Right)
		gl.End()                      -- Done Drawing A Triangle
	end
	function tab:Polygon()
        if GLRenderer.mode == 'LINES' then
            gl.Begin('LINES')
        else
            gl.Begin('POLYGON')
        end
		if self.paramList["P"] then
			for i=1, #self.paramList["P"], 3 do
				gl.Vertex(self.paramList["P"][i], self.paramList["P"][i+1], self.paramList["P"][i+2])
			end
		end
		gl.End()
	end
	function tab:Projection()
		gl.MatrixMode('PROJECTION')
		glu.Perspective(40, 320/240, 1, 5000)
		gl.Scale(1, 1, -1)
	end

	function r:start(options)
		gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT') -- Clear Screen And Depth Buffer
		gl.MatrixMode('PROJECTION')   -- Select The Projection Matrix
		gl.LoadIdentity()             -- Reset The Projection Matrix
	end

	function r:finish()
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


