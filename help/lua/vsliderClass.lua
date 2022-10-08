M.proto = {
	border = 20,
	knobW = 20,
	rad = 10,
	w = 50,
	h = 200,
	v = 0,
	red = 0
}

--local red = 1

local function clip(x, min, max)
	if(x < min) then return min
	elseif(x >max) then return max
	else return x
	end
end

local function adjColor(r, g, b, a, initColor)
	local initColor = initColor or of.getStyle().color
	of.setColor(initColor.r*r, initColor.g*g, initColor.b*b, initColor.a*a)
end

function M.proto:touch(event, x, y, id)
	if(event == "down") then
		self.y0 = y
		self.v0 = self.v
	elseif(event == "move") then
		self.v = clip(self.v0 - (y - self.y0) / (self.h - self.border - self.knobW), 0, 1)
		self:out(self.v)
		self:drawconfig("do")
	end
end

function M.proto:update()
	if((self.w ~= self.oldw) or (self.h ~= self.oldh)) then
		self.oldw = self.w
		self.oldh = self.h
		self:touchconfig("size", self.w, self.h)
	end
end

function M.proto:draw()
	local initColor = of.getStyle().color
	self:update()
	of.setRectMode(of.RECTMODE_CENTER)
	adjColor(0, 1, 0, 1, initColor)
	of.drawRectRounded(0, 0, self.w, self.h, self.rad)
	adjColor(1, 1, 1, 1, initColor)
	of.drawRectRounded(0, 0, self.w - self.border, self.h - self.border, self.rad)
	adjColor(self.red, 0, 0, 1, initColor)
	of.drawRectRounded(
		0, (self.h - self.knobW - self.border) * (0.5 - self.v),
		self.w - self.border, self.knobW, self.rad
	)
	of.setRectMode(of.RECTMODE_CORNER)
	of.setColor(initColor)
end

function M.proto:set(v)
	self.v = v
end

function M:new(o)
	o = o or {}   -- create object if user does not provide one
	setmetatable(o, self.proto)
	self.proto.__index = self.proto
	return o
end

