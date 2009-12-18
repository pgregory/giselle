GLRenderer = Object:clone()
GLRenderer.mode = 'LINES'

function GLRenderer:create(name)
	local r, tab = Renderer:create(name)

	function r:printMVP()
		local a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p = gl.Get('MODELVIEW_MATRIX')	
		local matx = matrix(4, 4)
		matx[1][1] = a
		matx[1][2] = b
		matx[1][3] = c
		matx[1][4] = d
		matx[2][1] = e
		matx[2][2] = f
		matx[2][3] = g
		matx[2][4] = h
		matx[3][1] = i
		matx[3][2] = j
		matx[3][3] = k
		matx[3][4] = l
		matx[4][1] = m
		matx[4][2] = n
		matx[4][3] = o
		matx[4][4] = p
		io.stderr:write(matrix.tostring(matx).."\n")
	end

	function r:printProjMat()
		local a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p = gl.Get('PROJECTION_MATRIX')	
		local matx = matrix(4, 4)
		matx[1][1] = a
		matx[1][2] = b
		matx[1][3] = c
		matx[1][4] = d
		matx[2][1] = e
		matx[2][2] = f
		matx[2][3] = g
		matx[2][4] = h
		matx[3][1] = i
		matx[3][2] = j
		matx[3][3] = k
		matx[3][4] = l
		matx[4][1] = m
		matx[4][2] = n
		matx[4][3] = o
		matx[4][4] = p
		io.stderr:write(matrix.tostring(matx).."\n")
	end

	function tab:WorldBegin(framestate, pass)
		Renderer.initTransform(r)
		if pass < 3 then return end
		gl.MatrixMode('MODELVIEW')
		gl.LoadIdentity()
	end
	function tab:WorldEnd(framestate, pass)
	end
	function tab:TransformBegin(framestate, pass)
		Renderer.pushTransform(r)
		if pass < 2 then return end
		gl.PushMatrix()
	end
	function tab:TransformEnd(framestate, pass)
		Renderer.popTransform(r)
		if pass < 2 then return end
		gl.PopMatrix()
	end
	function tab:Translate(framestate, pass)
		Renderer.translate(r, self.x, self.y, self.z)
		if pass < 2 then return end
		gl.Translate(self.x, self.y, self.z)
	end
    function tab:Scale(framestate, pass)
		Renderer.scale(r, self.x, self.y, self.z)
		if pass < 2 then return end
        gl.Scale(self.x, self.y, self.z)
    end
    function tab:Rotate(framestate, pass)
		Renderer.rotate(r, self.angle, self.x, self.y, self.z)
		if pass < 2 then return end
		gl.Rotate(self.angle, self.x, self.y, self.z)
	end
	function tab:ConcatTransform(framestate, pass)
		Renderer.concatTransform(r, self.matrix)
		if pass < 2 then return end
		gl.MultMatrix(self.matrix)
	end
	function tab:Cylinder(framestate, pass)
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
	function tab:Sphere(framestate, pass)
		local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
        glu.Sphere(quad, self.radius, 12, 6)
	end
	function tab:Disk(framestate, pass)
		local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
        glu.Disk(quad, 0, self.radius, 12, 1)
	end
	function tab:PatchMesh(framestate, pass)
		gl.Begin('TRIANGLES')         -- Drawing Using Triangles
		gl.Vertex( 0, 0.3, 0)         -- Move Up One Unit From Center (Top Point)
		gl.Vertex(-0.3,-0.3, 0)         -- Left And Down One Unit (Bottom Left)
		gl.Vertex( 0.3,-0.3, 0)         -- Right And Down One Unit (Bottom Right)
		gl.End()                      -- Done Drawing A Triangle
	end
	function tab:Polygon(framestate, pass)
        if GLRenderer.mode == 'LINES' then
            gl.Begin('LINE_LOOP')
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
	function tab:Projection(framestate, pass)
		if pass ~= 2 then return end
		gl.MatrixMode('PROJECTION')
		glu.Perspective(40, 320/240, 1, 5000)
		gl.Scale(1, 1, -1)
	end
	function tab:CoordinateSystem(framestate, pass)
		CoordinateFrame.set(self.name, r.matrixStack[#r.matrixStack])
	end
	function tab:CoordSysTransform(framestate, pass)
		local mat = CoordinateFrame.get(self.name)
		if mat then
			gl.LoadMatrix(mat)
		end
	end
	function tab:LightSource(framestate, pass)
		local quad = glu.NewQuadric()
        if GLRenderer.mode == 'LINES' then
            glu.QuadricDrawStyle(quad, 'LINE')
        else
            glu.QuadricDrawStyle(quad, 'SOLID')
        end
		gl.PushAttrib('CURRENT_BIT')
        gl.PushMatrix()
		gl.Color(1,1,0)
		gl.Translate(0, 0, -1)
		glu.Cylinder(quad, 0.2, 0.0, 1, 12, 1)
		gl.PopMatrix()
		gl.PopAttrib('CURRENT_BIT')
	end
	function tab:CameraTransform(framestate, pass)
		-- Only record the camera transform during pass 1, it
		-- will be 'used' during subsequent passes.
		if pass > 1 then return end
		local mat = r.matrixStack[#r.matrixStack]
		mat = matrix.invert(mat)
		framestate["cameraTransforms"][self.camera.name] = mat
	end
	function tab:RecordTransform(framestate, pass)
		framestate["transforms"][self.name] = r.matrixStack[#r.matrixStack]
	end
	function tab:RestoreTransform(framestate, pass)
		local mat = framestate["transforms"][self.name]
		Renderer.setTransform(r, mat)
		if pass < 2 then return end
		gl.LoadMatrix(mat or matrix(4, "I"))
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


