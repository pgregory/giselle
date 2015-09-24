Camera = Model:clone()
Camera.nodeType = "camera"
Cameras = {}

function Camera:generate(time)
	Renderable:pushState()
	if(self.body) then
		self:body(time or self.time)
	end
	CameraTransform(self)
	return Renderable:popState()
end


Cameras_meta = {}

function Cameras_meta:__index(key)
    camera = rawget(self, key)
    if camera and type(camera) == "table" then
        return camera
    else
        error("Camera \""..tostring(key).."\" does not exist")
    end
end

setmetatable(Cameras, Cameras_meta)
