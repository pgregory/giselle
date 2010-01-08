function partialSphere(radius, thetamin, thetamax, zmin, zmax)
	local thetan = math.ceil(math.max(3, ((thetamax - thetamin)/360) * 18))
	local phimin = -90
	local phimax = 90
	if zmin > -radius then
		phimin = math.deg(math.asin(zmin/radius))
	end
	if zmax < radius then
		phimax = math.deg(math.asin(zmax/radius))
	end
	local phin = math.ceil(math.max(3, ((phimax - phimin)/180) * 18))
	local phi1 = math.rad(math.max(phimin, -90))
	local phi2 = math.rad(math.min(phimax, 90))
	local theta1 = math.rad(math.max(thetamin, 0)) 
	local theta2 = math.rad(math.min(thetamax, 360))
	for j = 0, phin-1 do
		local t1 = phi1 + j * (phi2 - phi1) / phin
		local t2 = phi1 + (j + 1) * (phi2 - phi1) / phin
		gl.Begin('QUAD_STRIP')
		for i = 0, thetan do
			local t3 = theta1 + i * (theta2 - theta1) / thetan

			local ex = math.cos(t1) * math.cos(t3)
			local ey = math.cos(t1) * math.sin(t3)
			local ez = math.sin(t1)
			local px = radius * ex
			local py = radius * ey
			local pz = radius * ez
			gl.Normal(ex, ey, ez)
			gl.TexCoord(i/thetan, j/phin)
			gl.Vertex(px, py, pz)

			ex = math.cos(t2) * math.cos(t3)
			ey = math.cos(t2) * math.sin(t3)
			ez = math.sin(t2)
			px = radius * ex
			py = radius * ey
			pz = radius * ez
			gl.Normal(ex, ey, ez)
			gl.TexCoord(i/thetan, (j+1)/phin)
			gl.Vertex(px, py, pz)
		end
		gl.End()
	end
end

function partialCylinder(radius, thetamin, thetamax, zmin, zmax)
	local thetan = math.ceil(math.max(3, ((thetamax - thetamin)/360) * 18))
	local theta1 = math.rad(math.max(thetamin, 0)) 
	local theta2 = math.rad(math.min(thetamax, 360))
	gl.Begin('QUAD_STRIP')
	for i = 0, thetan do
		local t1 = theta1 + i * (theta2 - theta1) / thetan

		local ex = math.cos(t1)
		local ey = math.sin(t1)
		local ez = 0
		local px = radius * ex
		local py = radius * ey
		local pz = zmin
		gl.Normal(ex, ey, ez)
		gl.TexCoord(i/thetan, 0)
		gl.Vertex(px, py, pz)

		pz = zmax
		gl.Normal(ex, ey, ez)
		gl.TexCoord(i/thetan, 1)
		gl.Vertex(px, py, pz)
	end
	gl.End()
end


function partialDisk(radius, thetamin, thetamax, height)
	local thetan = math.ceil(math.max(3, ((thetamax - thetamin)/360) * 18))
	local theta1 = math.rad(math.max(thetamin, 0)) 
	local theta2 = math.rad(math.min(thetamax, 360))
	for j = 0, 8 do -- Default to 8 rings (9 rows of vertices) for now.
		local t1 = j / 9
		local t2 = (j + 1) / 9
		gl.Begin('QUAD_STRIP')
		for i = 0, thetan do
			local t3 = theta1 + i * (theta2 - theta1) / thetan

			local px = radius * (1-t1) * math.cos(t3)
			local py = radius * (1-t1) * math.sin(t3)
			local pz = height
			gl.Normal(0, 0, 1)
			gl.TexCoord(i/thetan, t1)
			gl.Vertex(px, py, pz)

			px = radius * (1-t2) * math.cos(t3) 
			py = radius * (1-t2) * math.sin(t3)
			pz = height
			gl.Normal(0, 0, 1)
			gl.TexCoord(i/thetan, t2)
			gl.Vertex(px, py, pz)
		end
		gl.End()
	end
end


bezier = matrix({ {-1, 3,-3, 1 }, 
				  { 3,-6, 3, 0 }, 
				  {-3, 3, 0, 0 }, 
				  { 1, 0, 0, 0 } })
				  
catmull_rom = matrix({ {-0.5, 1.0,-0.5, 0.0 }, 
					   { 1.5,-2.5, 0.0, 1.0 }, 
					   {-1.5, 2.0, 0.5, 0.0 }, 
					   { 0.5,-0.5, 0.0, 0.0 } })                 
					   
bspline = matrix({  {-1.0/6, 3.0/6,-3.0/6, 1.0/6 }, 
					{ 3.0/6,-6.0/6,   0, 4.0/6 }, 
					{-3.0/6, 3.0/6, 3.0/6, 1.0/6 }, 
					{ 1.0/6,   0,   0,   0 } })

power = matrix({	{1, 0, 0, 0},
					{0, 1, 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 0} })
					
hermite = matrix({	{2, 1, -2,  1},
					{-3, -2, 3, -1},
					{0, 1, 0, 0},
					{1, 0, 0, 0} })

linear = matrix({	{0, 0, 0, 0},
				    {0, 0, 0, 0},
				    {0, -1, 1, 0},
				    {0, 1, 0, 0} })

function bicubicPatch(mpu, mpv, cpX, cpY, cpZ)
	local count = 10
	local du, dv
	local N
  
	local px = matrix(cpX)
	local py = matrix(cpY)
	local pz = matrix(cpZ)
  
	local mput = matrix.transpose(mpu)
	local mpvt = matrix.transpose(mpv)

	local fpX = {}
	local fpY = {}
	local fpZ = {}
	for i = 0, count do
		fpX[i+1] = {}
		fpY[i+1] = {}
		fpZ[i+1] = {}
	end
  
	for i = 0, count do
		local u = i/count
		local mu = matrix(1,4)
		mu[1][1] = math.pow(u,3)
		mu[1][2] = math.pow(u,2)
		mu[1][3] = u
		mu[1][4] = 1

		mu = mu * mput
		local mux = mu * px 
		local muy = mu * py
		local muz = mu * pz 

		for j = 0, count do
			local v = j/count
			local mv = matrix(4,1)
			mv[1][1] = math.pow(v,3)
			mv[2][1] = math.pow(v,2)
			mv[3][1] = v
			mv[4][1] = 1
			mv = mpv * mv

			local x = mux * mv
			local y = muy * mv
			local z = muz * mv

			fpX[i+1][j+1] = x[1][1]
			fpY[i+1][j+1] = y[1][1]
			fpZ[i+1][j+1] = z[1][1]
		end
	end

	-- Calculate normals here.
    
	gl.Begin('QUADS')
	for i = 1, count do
		for j = 1, count do
			gl.Vertex(fpX[i][j], fpY[i][j], fpZ[i][j])
			gl.Vertex(fpX[i+1][j], fpY[i+1][j], fpZ[i+1][j])
			gl.Vertex(fpX[i+1][j+1], fpY[i+1][j+1], fpZ[i+1][j+1])
			gl.Vertex(fpX[i][j+1], fpY[i][j+1], fpZ[i][j+1])
		end
	end
	gl.End()
end

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
		partialCylinder(self.radius, 0, self.thetamax, self.zmin, self.zmax)
	end
	function tab:Sphere(framestate, pass)
		partialSphere(self.radius, 0, self.thetamax, self.zmin, self.zmax)
	end
	function tab:Disk(framestate, pass)
		partialDisk(self.radius, 0, self.thetamax, self.height)
	end
	function tab:Patch(framestate, pass)
		local pX = {}
		local pY = {}
		local pZ = {}
		local index = 1
		for i = 1, 4 do
			pX [i] = {}
			pY [i] = {}
			pZ [i] = {}
			for j = 1, 4 do
				pX[i][j] = self.paramList["P"][index]
				pY[i][j] = self.paramList["P"][index+1]
				pZ[i][j] = self.paramList["P"][index+2]
				index = index + 3
			end
		end
		bicubicPatch(bezier, bezier, pX, pY, pZ) 
	end
	function tab:PatchMesh(framestate, pass)
		gl.Begin('TRIANGLES')         -- Drawing Using Triangles
		gl.Vertex( 0, 0.3, 0)         -- Move Up One Unit From Center (Top Point)
		gl.Vertex(-0.3,-0.3, 0)         -- Left And Down One Unit (Bottom Left)
		gl.Vertex( 0.3,-0.3, 0)         -- Right And Down One Unit (Bottom Right)
		gl.End()                      -- Done Drawing A Triangle
	end
	function tab:Polygon(framestate, pass)
		gl.Begin('POLYGON')
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
	function tab:Surface(framestate, pass)
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
--		gl.Enable('LIGHTING')
		gl.Light('LIGHT1', 'AMBIENT', {0.1, 0.1, 0.1, 1})
		gl.Light('LIGHT1', 'DIFFUSE', {1, 1, 1, 1})
		gl.Light('LIGHT1', 'POSITION', {1, 1, 0, 0})
		gl.Light('LIGHT1', 'SPOT_DIRECTION', {0, 0, 1})
		gl.Enable('LIGHT1')
		gl.PolygonMode('FRONT_AND_BACK', 'LINE')
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


