CoordinateFrame = Object:clone()
CoordinateFrame.frames = {}

function CoordinateFrame.set(name, matrix)
	CoordinateFrame.frames[name] = matrix
end

function CoordinateFrame.get(name)
	return CoordinateFrame.frames[name] or {}
end

function CoordinateFrame.clearAll()
	CoordinateFrame.frames = {}
end
