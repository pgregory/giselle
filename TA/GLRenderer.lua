GLRenderer = Object:clone()
GLRenderer.mode = 'LINES'
GLRenderer.matrixStack = {}

function GLRenderer:create(name)
	local r, tab = Renderer:create(name)
	r.matrixStack = {}
	table.insert(r.matrixStack, matrix(4, "I"))

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

	function tab:WorldBegin()
		gl.MatrixMode('MODELVIEW')
		gl.LoadIdentity()
		table.insert(r.matrixStack, matrix(4, "I"))
	end
	function tab:WorldEnd()
	end
	function tab:TransformBegin()
		gl.PushMatrix()
		currMat = r.matrixStack[#r.matrixStack]
		table.insert(r.matrixStack, matrix(currMat))
	end
	function tab:TransformEnd()
		gl.PopMatrix()
		table.remove(r.matrixStack)
	end
	function tab:Translate()
		gl.Translate(self.x, self.y, self.z)
		local transMat = matrix(4, "I")
		transMat[4][1] = self.x
		transMat[4][2] = self.y
		transMat[4][3] = self.z
		r.matrixStack[#r.matrixStack] = matrix.mul(transMat, r.matrixStack[#r.matrixStack])
	end
    function tab:Scale()
        gl.Scale(self.x, self.y, self.z)
		local scaleMat = matrix(4, "I")
		scaleMat[1][1] = self.x
		scaleMat[2][2] = self.y
		scaleMat[3][3] = self.z
		r.matrixStack[#r.matrixStack] = matrix.mul(scaleMat, r.matrixStack[#r.matrixStack])
    end
    function tab:Rotate()
		gl.Rotate(self.angle, self.x, self.y, self.z)
		local rotMat = matrix(4, "I")
		local c = math.cos(math.rad(self.angle))
		local s = math.sin(math.rad(self.angle))
		local l = math.sqrt(self.x*self.x+self.y*self.y+self.z*self.z)
		local ux = self.x / l
		local uy = self.y / l
		local uz = self.z / l
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
		r.matrixStack[#r.matrixStack] = matrix.mul(rotMat, r.matrixStack[#r.matrixStack])
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
	function tab:Projection()
		gl.MatrixMode('PROJECTION')
		glu.Perspective(40, 320/240, 1, 5000)
		gl.Scale(1, 1, -1)
	end
	function tab:CoordinateSystem()
		CoordinateFrame.set(self.name, r.matrixStack[#r.matrixStack])
	end
	function tab:CoordSysTransform()
		local mat = CoordinateFrame.get(self.name)
		if mat then
			gl.LoadMatrix(mat)
		end
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


