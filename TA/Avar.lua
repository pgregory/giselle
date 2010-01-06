KeyFrame= {}
function KeyFrame:create(frame, value)
        local o = {}
        o.frame = math.floor(frame)
	o.value = value
	return o
end

Avar = {}
function Avar:__call(time)
	if self.interpolator == nil then
		return Avar.lininterp(self,time)
	end
end
Avar.__index = Avar

function Avar:lininterp(time)
	local keyframes = ExposureSheet.avars[self:locator()]
    if keyframes == nil or #keyframes == 0 then
        return 0
    end
    if time <= keyframes[1].frame then
		return keyframes[1].value
    elseif time >= keyframes[table.maxn(keyframes)].frame then
		return keyframes[table.maxn(keyframes)].value
	else
		-- Calculate the correct point in the sequence of keyframes
		local span = 1
        while keyframes[span].frame <= time and span <= table.maxn(keyframes) do span = span+1 end
		-- Now calculate the point within the span
        if time == keyframes[span-1].frame then return keyframes[span-1].value end
        if time == keyframes[span].frame then return keyframes[span].value end
        local r = (time - keyframes[span-1].frame)/(keyframes[span].frame - keyframes[span-1].frame)
		local v = keyframes[span-1].value + (keyframes[span].value - keyframes[span-1].value) * r
		return v
	end
end

function Avar:locator()
	return self.model:locator()..":"..self.name
end

function Avar:addKeyframe(frame, value)
	return ExposureSheet:addKeyframe(self:locator(), frame, value)
end


function Avar:removeKeyframe(kf)
	ExposureSheet:removeKeyframe(self:locator(), kf.frame) 
end

function Avar:create(name, model)
	local o = {}
	o.name = name
	o.model = model
	setmetatable(o, Avar)
	return o
end


return Avar
