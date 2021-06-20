-- This example is based on the openFrameworks example "3DPrimitivesExample"

of.setSmoothLighting(true);
of.setSphereResolution(151);
local width, height = of.getWidth() * 0.3, of.getHeight() * 0.3;
M.sphere = of.SpherePrimitive();
M.sphere:setRadius(height);
local screenWidth, screenHeight = of.getWidth(), of.getHeight();
shine = 400
--sphere:setPosition(screenWidth * 0.8, screenHeight * 0.25, 0);
---[[
-- pointLight = nil;
pointLight = pointLight or of.Light();
pointLight:setPointLight();
pointLight:setDiffuseColor(of.FloatColor(0.85, 0.85, 0.55));
pointLight:setSpecularColor(of.FloatColor(0.5, 0.5, 0.5));

pointLight2 = pointLight2 or of.Light();
pointLight2:setPointLight();
pointLight2:setDiffuseColor(of.FloatColor(238 / 255, 57 / 255, 135 / 255));
pointLight2:setSpecularColor(of.FloatColor(0.6, 0.6, 0.7));

pointLight3 = pointLight3 or of.Light();
pointLight3:setPointLight();
pointLight3:setDiffuseColor(of.FloatColor(19 / 255, 94 / 255, 77 / 255));
pointLight3:setSpecularColor(of.FloatColor(18 / 255, 150 / 255, 135 / 255));

material = of.Material();
material:setShininess(shine);
material:setSpecularColor(of.FloatColor(1, 1, 1));
--]]--

function M:draw()
---[[
	local width, height, time = of.getWidth(), of.getHeight(), of.getElapsedTimef();
	pointLight:setPosition((width * 0.5) + math.cos(time * 0.5) * (width * 0.3), height / 2, 500);
	pointLight2:setPosition((width * 0.5) + math.cos(time * 0.15) * (width * 0.3), height * 0.5 + math.sin(time * 0.7) * height, -300);
	pointLight3:setPosition(math.cos(time * 1.5) * width * 0.5, math.sin(time * 1.5) * width * 0.5, math.cos(time * 0.2) * width);

	local spinX = math.sin(time * 0.35);
	local spinY = math.cos(time * 0.075 * 2);
	of.enableDepthTest();
	of.enableLighting();
	pointLight:enable();
	pointLight2:enable();
	pointLight3:enable();
	
	material:setShininess(shine);

	material:beginMaterial();
--]]--
	of.drawSphere(M.sphere:getRadius());
	
	of.rotate(math.sin(time * 0.1) * 90, 1, 0, 0);
	of.drawPlane(0, 0, -400, 1000, 1000);

	--M.sphere:drawFaces();
	--M.sphere:rotateDeg(spinX, 1, 0, 0);
	--M.sphere:rotateDeg(spinY, 0, 1, 0);
---[[
	material:endMaterial();
	of.disableLighting();
	of.disableDepthTest();
--]]--
end



