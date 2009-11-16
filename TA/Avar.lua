KeyFrame= Object:clone()
function KeyFrame:create(time, value)
	local o = self:clone()
	o.time = time
	o.value = value
	return o
end

Avar = Object:clone()
function Avar:__call(time)
	if self.interpolator == nil then
		return Avar.lininterp(self,time)
	end
end

function Avar:lininterp(time)
	if time <= self.keyframes[1].time then
		return self.keyframes[1].value
	elseif time >= self.keyframes[table.maxn(self.keyframes)].time then
		return self.keyframes[table.maxn(self.keyframes)].value
	else
		-- Calculate the correct point in the sequence of keyframes
		local span = 1
		while self.keyframes[span].time <= time and span <= table.maxn(self.keyframes) do span = span+1 end
		-- Now calculate the point within the span
		if time == self.keyframes[span-1].time then return self.keyframes[span-1].value end
		if time == self.keyframes[span].time then return self.keyframes[span].value end
		local r = (time - self.keyframes[span-1].time)/(self.keyframes[span].time - self.keyframes[span-1].time)
		local v = self.keyframes[span-1].value + (self.keyframes[span].value - self.keyframes[span-1].value) * r
		return v
	end
end

function Avar:create(name, keyframes)
	local o = Avar:clone()
	o.keyframes = {}
        if #keyframes > 0 then
            table.sort(keyframes, function(a,b) return a[1] < b[1] end)
            for i,v in ipairs(keyframes) do
                    o.keyframes[i] = KeyFrame:create(v[1], v[2])
            end
	end
	o.name = name
	setmetatable(o, self)
	return o
end


return Avar
