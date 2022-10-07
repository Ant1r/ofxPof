local Text = {text = {}, fontsize = 12, maxW = 1e6, anchorX = 0, anchorY = 0, maxlines = 1e6,
	centered = false, finalX = 0, finalY = 0}
local T = Text

function T:setfont(fontsym, fontsize)
	self.fontsym = fontsym
	if fontsize then self.fontsize = fontsize end
	self.font = nil
	self.lines = nil
end

function T:update()
	if not self.font then if self.fontsym then self.font = pof.fonts_get(self.fontsym) end end
	if not self.font then return end
	if not self.lines then
		self.lines = self.font:computeMultiLines(self.text, self.fontsize, self.maxW)
		local bbox = self.font:drawMultiLines(self.lines, self.fontsize, 0, 0, self.maxW, true, self.maxlines, self.centered)
		self.finalX = bbox:getWidth() * (-self.anchorX - 1) / 2 - bbox:getX()
		self.finalY = bbox:getHeight() * (-self.anchorY - 1) / 2 - bbox:getY()
	end
end

function T.settext(self, ...)
	local result = ""
	local arg = arg or {...}
	for i,v in ipairs(arg) do
	  result = result .. tostring(v) .. " "
	end
	self.text = result
	self.lines = nil
end

function T:dodraw()
	self:update()
	if self.font == nil then return end
	if self.lines == nil then return end
	self.bbox = self.font:drawMultiLines(self.lines, self.fontsize, self.finalX, self.finalY, self.maxW, false, self.maxlines, self.centered)
end

function T:set(v, k)
	self[v] = k
	self.font = nil
	self.lines = nil
end

function T:new(o, fontsym, fontsize)
	setmetatable(o, self)
	self.__index = self
	if fontsym and fontsize then o:setfont(fontsym, fontsize) end
	return o
end

T:new(M, $"font1", 50)
M:settext("ce texte est franchement long, il va tenir sur plusieurs lignes !")
M.maxW = 400

function M:draw()
	self:dodraw()
	of.setColor(255,255,255,50)
	if (self.bbox) then of.drawRectangle(self.bbox) end -- show bbox
end

function M:setfontn(f)
	if f == 1 then getmetatable(self).setfont(self, $"font1") end
	if f == 2 then getmetatable(self).setfont(self, $"font2") end
	if f == 3 then getmetatable(self).setfont(self, self.font3) end
end

