ExposureSheet = {}
ExposureSheet.avars = {}

function ExposureSheet:addKeyframe(locator, frame, value)
    local kf = KeyFrame:create(frame, value)
	local avar = self.avars[locator] or {}
    table.insert(avar, kf)
    table.sort(avar, function(a,b) return a.frame < b.frame end)
	self.avars[locator] = avar
	return kf
end

function ExposureSheet:removeKeyframe(locator, frame)
	if self.avars[locator] == nil then
		return
	end
	local avar = self.avars[locator]

    for i,v in ipairs(avar) do
        if v.frame == frame then
            table.remove(avar, i)
            return
        end
    end
end


function ExposureSheet:printSheet()
	for avar,keyframes in pairs(self.avars) do
		io.stderr:write(avar.." --> ")
		for i,kf in ipairs(keyframes) do
			io.stderr:write(kf.frame.."="..kf.value..", ")
		end
		io.stderr:write("\n")
	end
end

