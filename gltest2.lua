iup.key_open()

rtri = 0
rquad = 0

cnv = iup.glcanvas{buffer="DOUBLE", rastersize = "640x480"}

timer = iup.timer{time=10}

function timer:action_cb()
  rtri = rtri + 0.2             -- Increase The Rotation Variable For The Triangle ( NEW )
  rquad = rquad - 0.15          -- Decrease The Rotation Variable For The Quad     ( NEW )
  iup.Update(cnv)
end

function cnv:resize_cb(width, height)
  iup.GLMakeCurrent(self)
  gl.Viewport(0, 0, width, height)

  gl.MatrixMode('PROJECTION')   -- Select The Projection Matrix
  gl.LoadIdentity()             -- Reset The Projection Matrix
  
  if height == 0 then           -- Calculate The Aspect Ratio Of The Window
    height = 1
  end

  glu.Perspective(40, width / height, 1, 5000)
  gl.Scale(1, 1, -1)
  gl.Translate(0, 0, 1)

  gl.MatrixMode('MODELVIEW')    -- Select The Model View Matrix
  gl.LoadIdentity()             -- Reset The Model View Matrix
end

function cnv:action(x, y)
  iup.GLMakeCurrent(self)
  gl.Clear('COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT') -- Clear Screen And Depth Buffer
  
  gl.LoadIdentity()             -- Reset The Current Modelview Matrix
  gl.Translate(0, 0, 2)     -- Move Left 1.5 Units And Into The Screen 6.0
  
  gl.Begin('TRIANGLES')         -- Drawing Using Triangles
	gl.Vertex( 0, 1, 0)         -- Move Up One Unit From Center (Top Point)
    gl.Vertex(-1,-1, 0)         -- Left And Down One Unit (Bottom Left)
    gl.Vertex( 1,-1, 0)         -- Right And Down One Unit (Bottom Right)
  gl.End()                      -- Done Drawing A Triangle

  iup.GLSwapBuffers(self)
end

function cnv:k_any(c)
  if c == iup.K_q or c == iup.K_ESC then
    return iup.CLOSE
  elseif c == iup.K_F1 then
    if fullscreen then
      fullscreen = false
      dlg.fullscreen = "No"
    else
      fullscreen = true
      dlg.fullscreen = "Yes"
    end
  end
end

function cnv:map_cb()
  iup.GLMakeCurrent(self)
  gl.ShadeModel('SMOOTH')            -- Enable Smooth Shading
  gl.ClearColor(0, 0, 0, 0.5)        -- Black Background
  gl.ClearDepth(1.0)                 -- Depth Buffer Setup
  gl.Enable('DEPTH_TEST')            -- Enables Depth Testing
  gl.DepthFunc('LEQUAL')             -- The Type Of Depth Testing To Do
  gl.Enable('COLOR_MATERIAL')
  gl.Hint('PERSPECTIVE_CORRECTION_HINT','NICEST')
end

dlg = iup.dialog{cnv; title="LuaGL Test Application 04"}

dlg:show()
cnv.rastersize = nil -- reset minimum limitation
timer.run = "YES"

if (not iup.MainLoopLevel or iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
