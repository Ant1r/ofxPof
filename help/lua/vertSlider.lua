M.border = 20
local knobW = 20
local rad = 10

pof_instances = pof_instances or {}
pof_instances[M.pdself] = pof_instances[M.pdself] or {}

local function clip(x, min, max)
	if(x < min) then return min
	elseif(x >max) then return max
	else return x
	end
end

local function adjColor(r, g, b, a)
	local style = of.getStyle().color
	of.setColor(style.r*r, style.g*g, style.b*b, style.a*a)
end

function M:dotouch(event, x, y, id)
	if(event == "down") then
		self.y0 = y
		self.v0 = self.v
	elseif(event == "move") then
		self.v = clip(self.v0 - (y - self.y0) / (self.h - M.border - knobW), 0, 1)
		self.out(self.v)
	end
end

function M:dodraw()
	adjColor(1, 1, 1, 1)
	of.drawRectRounded(-self.w/2, -self.h/2, self.w, self.h, rad)
	adjColor(0, 0, 0, 1)
	of.drawRectRounded(
		(-self.w + M.border)/2,
		(-self.h + M.border)/2 + (self.h-knobW - M.border)*(1-self.v),
		self.w - M.border, knobW, rad
	)
end

function M:reload(what)
	-- print("reloading ",self)
	self.draw = what.dodraw
	self.touch = what.dotouch
	self.reload = what.reload
	self.v=0
	self.w=100
	self.config(self.w, self.h)
end

function M:format()
	self.w=50
	self.h=200
	self.config(self.w, self.h)
	self.v=0
	self.draw = M.dodraw
	self.touch = M.dotouch
	self.reload = M.reload
	pof_instances[M.pdself][self.pdself] = self
end

for k,v in pairs(pof_instances[M.pdself]) do
	if(type(v.reload) == "function") then 
		v:reload(M)
	end
end

