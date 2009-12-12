-- Module private.
local RenderCache = {}
RenderCache.cacheStack = {}
function RenderCache:push(newCache)
	table.insert(self.cacheStack, newCache or {})
	return self.cacheStack[#self.cacheStack]
end
function RenderCache:pop()
	return table.remove(self.cacheStack)
end
function RenderCache:currentCache()
	return self.cacheStack[#self.cacheStack]
end
RenderCache:push()



Renderable = Object:clone()
function Renderable:create()
	local o = self:clone()
	table.insert(RenderCache:currentCache(), o)
	return o
end
function Renderable:isEquivalent(other)
	return self.renderop == other.renderop
end
function Renderable:isEqual(other)
	return self:isEquivalent(other)
end
function Renderable:isParamListEquivalent(a, b)
	-- TODO need to do this check properly
	return true
end
function Renderable:isParamListEqual(a, b)
	-- TODO need to do this check properly
	return true
end



WorldBegin = Renderable:clone()
WorldBegin.renderop = "WorldBegin"
function WorldBegin:__call()
	return self:create()
end

WorldEnd = Renderable:clone()
WorldEnd.renderop = "WorldEnd"
function WorldEnd:__call()
	return self:create()
end

TransformBegin = Renderable:clone()
TransformBegin.renderop = "TransformBegin"
function TransformBegin:__call()
	return self:create()
end

TransformEnd = Renderable:clone()
TransformEnd.renderop = "TransformEnd"
function TransformEnd:__call()
	return self:create()
end

Translate = Renderable:clone()
Translate.x = 0
Translate.y = 0
Translate.z = 0
Translate.renderop = "Translate"
function Translate:__call(x, y, z)
	local o = self:create()
	o.x = x
	o.y = y
	o.z = z
	return o
end
function Translate:isEqual(other)
	if not Renderable.isEqual(self,other) then
		return false
	elseif self.x == other.x and
		   self.y == other.y and
		   self.z == other.z then
		return true
	end
	return false
end

Scale = Renderable:clone()
Scale.x = 0
Scale.y = 0
Scale.z = 0
Scale.renderop = "Scale"
function Scale:__call(x, y, z)
    local o = self:create()
    o.x = x
    o.y = y
    o.z = z
    return o
end
function Scale:isEqual(other)
    if not Renderable.isEqual(self,other) then
        return false
    elseif self.x == other.x and
           self.y == other.y and
           self.z == other.z then
        return true
    end
    return false
end


Rotate = Renderable:clone()
Rotate.renderop = "Rotate"
Rotate.angle = 0
Rotate.x = 0
Rotate.y = 0
Rotate.z = 0
function Rotate:__call(angle, x, y, z)
	local o = self:create()
	o.angle = angle
	o.x = x
	o.y = y
	o.z = z
	return o
end
function Rotate:isEqual(other)
	if not Renderable.isEqual(self, other) then
		return false
	elseif self.angle == other.angle and
		   self.x == other.x and
		   self.y == other.y and
		   self.z == other.z then
		return true
	end
	return false
end

Cylinder = Renderable:clone()
Cylinder.renderop = "Cylinder"
Cylinder.radius = 1.0
Cylinder.zmin = -1.0
Cylinder.zmax = 1.0
Cylinder.thetamax = 360.0
function Cylinder:__call(radius, zmin, zmax, thetamax)
	local o = self:create()
	o.radius = radius
	o.zmin = zmin
	o.zmax = zmax
	o.thetamax = thetamax
	return o
end
function Cylinder:isEqual(other)
	if not Renderable.isEqual(self,other) then
		return false
	elseif self.radius == other.radius and
		   self.zmin == other.zmin and
		   self.zmax == other.zmax and
		   self.thetamax == other.thetamax then
		return true
	end
	return false
end

Sphere = Renderable:clone()
Sphere.renderop = "Sphere"
Sphere.radius = 1.0
Sphere.zmin = -1.0
Sphere.zmax = 1.0
Sphere.thetamax = 360.0
function Sphere:__call(radius, zmin, zmax, thetamax)
	local o = self:create()
	o.radius = radius
	o.zmin = zmin
	o.zmax = zmax
	o.thetamax = thetamax
	return o
end
function Sphere:isEqual(other)
	if not Renderable.isEqual(self,other) then
		return false
	elseif self.radius == other.radius and
		   self.zmin == other.zmin and
		   self.zmax == other.zmax and
		   self.thetamax == other.thetamax then
		return true
	end
	return false
end


Disk = Renderable:clone()
Disk.renderop = "Disk"
Disk.height = 0.0
Disk.radius = 1.0
Disk.thetamax = 360.0
function Disk:__call(height, radius, thetamax)
	local o = self:create()
	o.height = height
	o.radius = radius
	o.thetamax = thetamax
	return o
end
function Disk:isEqual(other)
	if not Renderable.isEqual(self, other) then
		return false
	elseif self.radius == other.radius and
		   self.height == other.height and
		   self.thetamax == other.thetamax then
		return true
	end
	return false
end


PatchMesh = Renderable:clone()
PatchMesh.renderop = "PatchMesh"
PatchMesh.type = "bilinear"
PatchMesh.nu = 0
PatchMesh.uwrap = "nonperiodic"
PatchMesh.nv = 0
PatchMesh.vwrap = "nonperiodic"
PatchMesh.paramList = {}
function PatchMesh:__call(type, nu, uwrap, nv, vwrap, paramList)
	local o = self:create()
	o.type = type
	o.nu = nu
	o.uwrap = uwrap
	o.nv = nv
	o.vwrap = vwrap
	o.paramList = paramList or {}
	return o
end
function PatchMesh:isEqual(other)
	if not Renderable.isEqual(self, other) then
		return false
	elseif self.type == other.type and
		   self.nu == other.nu and
		   self.uwrap == other.uwrap and
		   self.nv == other.nv and
		   self.vwrap == other.vwrap and
		   Renderable:isParamListEqual(self.paramList, other.paramList) then
		return true
	end
	return false
end
function PatchMesh:isEquivalent(other)
	if not Renderable.isEquivalent(self, other) then
		return false
	elseif self.type == other.type and
		   self.nu == other.nu and
		   self.uwrap == other.uwrap and
		   self.nv == other.nv and
		   self.vwrap == other.vwrap and
		   Renderable:isParamListEquivalent(self.paramList, other.paramList) then
		return true
	end
	return false
end


Polygon = Renderable:clone()
Polygon.renderop = "Polygon"
Polygon.paramList = {}
function Polygon:__call(paramList)
	local o = self:create()
	o.paramList = paramList or {}
	return o
end
function Polygon:isEqual(other)
	if not Renderable.isEqual(self, other) then
		return false
	elseif Renderable:isParamListEqual(self.paramList, other.paramList) then
		return true
	end
	return false
end
function Polygon:isEquivalent(other)
	if not Renderable.isEquivalent(self, other) then
		return false
	elseif Renderable:isParamListEquivalent(self.paramList, other.paramList) then
		return true
	end
	return false
end


Projection = Renderable:clone()
Projection.renderop = "Projection"
Projection.type = "perspective"
Projection.paramList = {}
function Projection:__call(type, paramList)
	local o = self:create()
	o.type = type
	o.paramList = paramList or {} 
	return o
end
function Projection:isEqual(other)
	-- Projection is currently always true, no motion blur on that.
	return true
end


CoordinateSystem = Renderable:clone()
CoordinateSystem.renderop = "CoordinateSystem"
CoordinateSystem.name = ""
function CoordinateSystem:__call(name)
	local o = self:create()
	o.name = name
	return o
end
function CoordinateSystem:isEqual(other)
	return true
end


CoordSysTransform = Renderable:clone()
CoordSysTransform.renderop = "CoordSysTransform"
CoordSysTransform.name = ""
function CoordSysTransform:__call(name)
	local o = self:create()
	o.name = name
	return o
end
function CoordSysTransform:isEqual(other)
	return true
end

RecordTransform = Renderable:clone()
RecordTransform.renderop = "RecordTransform"
RecordTransform.name = ""
function RecordTransform:__call(name)
	local o = self:create()
	o.name = name
	return o
end
function RecordTransform:isEqual(other)
	return true
end


RestoreTransform = Renderable:clone()
RestoreTransform.renderop = "RestoreTransform"
RestoreTransform.name = ""
function RestoreTransform:__call(name)
	local o = self:create()
	o.name = name
	return o
end
function RestoreTransform:isEqual(other)
	return true
end


--[[ Render a set of frames.
--   parameter: renderables   a list of lists of Renderables, one list for
--   						  each frame.
--   parameter: times		  a list of times, one time for each frame.
--]]
function Renderable:renderRenderables(renderables, times)
	for i,ro in ipairs(renderables[1]) do
		if ro.group then
			local subRenderables = {}
			local f0 = ro:generate(times[1])
			table.insert(subRenderables, f0)
			for frame = 2, #renderables do
				local fn = renderables[frame][i]:generate(times[frame])
				table.insert(subRenderables, fn)
			end
			self:renderRenderables(subRenderables, times)
		else
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
				ro:render()
			else
				ri.MotionBegin(times)
				ro:render()
				for frame = 2, #renderables do
					renderables[frame][i]:render()
				end
				ri.MotionEnd()
			end
		end
	end
end

function Renderable:pushState()
	return RenderCache:push()
end

function Renderable:popState()
	return RenderCache:pop()
end

Renderable.transformNodes = {
	TransformBegin = 1,
	TransformEnd = 1,
	Translate = 1,
	Rotate = 1,
	Scale = 1,
	RecordTransform = 1,
	RestoreTransform = 1,
	WorldBegin = 1,
	WorldEnd = 1,
}

return Renderable
