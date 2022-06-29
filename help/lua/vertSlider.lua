M.border = 20
M.knobW = 20
M.rad = 10

local red = 0
red = 1

local pof_instances = pof_instances or {}
pof_instances[M.pdself] = pof_instances[M.pdself] or {}

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

function M:dotouch(event, x, y, id)
	if(event == "down") then
		self.y0 = y
		self.v0 = self.v
	elseif(event == "move") then
		self.v = clip(self.v0 - (y - self.y0) / (self.h - M.border - M.knobW), 0, 1)
		self.out(self.v)
		self.drawconfig("do")
	end
end

function M:update()
	if((self.w ~= self.oldw) or (self.h ~= self.oldh)) then
		self.oldw = self.w
		self.oldh = self.h
		self.touchconfig("size", self.w, self.h)
	end
end

function M:dodraw()
	local initColor = of.getStyle().color
	self:update()
	of.setRectMode(of.RECTMODE_CENTER)
	adjColor(0, 1, 0, 1, initColor)
	of.drawRectRounded(0, 0, self.w, self.h, M.rad)
	adjColor(1, 1, 1, 1, initColor)
	of.drawRectRounded(0, 0, self.w - M.border, self.h - M.border, M.rad)
	adjColor(red, 0, 0, 1, initColor)
	of.drawRectRounded(
		0, (self.h - M.knobW - M.border) * (0.5 - self.v),
		self.w - M.border, M.knobW, M.rad
	)
	of.setRectMode(of.RECTMODE_CORNER)
end

function M:set(v)
	self.v = v
end

function M:format()
	self.w = self.w or 50
	self.h = self.h or 200
	-- self.config("size", self.w, self.h)
	self.v = self.v or 0
	self.draw = M.dodraw
	self.touch = M.dotouch
	self.update = M.update
	self.set = M.set
	self.drawconfig("continuousForce", false)
	pof_instances[M.pdself][self.pdself] = self
end

for k,v in pairs(pof_instances[M.pdself]) do
	M.format(v)
end

