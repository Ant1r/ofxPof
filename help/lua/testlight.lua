-- This example is based on the openFrameworks example "3DPrimitivesExample"

of.setSmoothLighting(true);
of.setSphereResolution(24);
local width, height = of.getWidth() * 0.12, of.getHeight() * 0.12;
local sphere = of.SpherePrimitive();
sphere:setRadius(width);
local icoSphere = of.IcoSpherePrimitive();
icoSphere:setRadius(width);
local plane = of.PlanePrimitive();
plane:set(width * 1.5, height * 1.5);
local cylinder = of.CylinderPrimitive();
cylinder:set(width * 0.7, height * 2.2);
local cone = of.ConePrimitive();
cone:set(width * 0.75, height * 2.2);
local box = of.BoxPrimitive();
box:set(width * 1.25);
local screenWidth, screenHeight = of.getWidth(), of.getHeight();
--plane:setPosition(screenWidth * 0.2, screenHeight * 0.25, 0);
--box:setPosition(screenWidth * 0.5, screenHeight * 0.25, 0);
--box:setPosition(0,0, 0);
--[[sphere:setPosition(screenWidth * 0.8, screenHeight * 0.25, 0);
icoSphere:setPosition(screenWidth * 0.2, screenHeight * 0.75, 0);
cylinder:setPosition(screenWidth * 0.5, screenHeight * 0.75, 0);
cone:setPosition(screenWidth * 0.8, screenHeight * 0.75, 0);]]--
local pointLight = of.Light();
pointLight:setPointLight();
pointLight:setDiffuseColor(of.FloatColor(0.85, 0.85, 0.55));
pointLight:setSpecularColor(of.FloatColor(1, 1, 1));
local pointLight2 = of.Light();
pointLight2:setPointLight();
pointLight2:setDiffuseColor(of.FloatColor(238 / 255, 57 / 255, 135 / 255));
pointLight2:setSpecularColor(of.FloatColor(0.8, 0.8, 0.9));
local pointLight3 = of.Light();
pointLight3:setPointLight();
pointLight3:setDiffuseColor(of.FloatColor(19 / 255, 94 / 255, 77 / 255));
pointLight3:setSpecularColor(of.FloatColor(18 / 255, 150 / 255, 135 / 255));
local material = of.Material();
material:setShininess(120);
material:setSpecularColor(of.FloatColor(1, 1, 1));

function M.draw()
local width, height, time = of.getWidth(), of.getHeight(), of.getElapsedTimef();
pointLight:setPosition((width * 0.5) + math.cos(time * 0.5) * (width * 0.3), height / 2, 500);
pointLight2:setPosition((width * 0.5) + math.cos(time * 0.15) * (width * 0.3), height * 0.5 + math.sin(time * 0.7) * height, -300);
pointLight3:setPosition(math.cos(time * 1.5) * width * 0.5, math.sin(time * 1.5) * width * 0.5, math.cos(time * 0.2) * width);

local spinX = math.sin(of.getElapsedTimef() * 0.35);
local spinY = math.cos(of.getElapsedTimef() * 0.075);
of.enableDepthTest();
of.enableLighting();
pointLight:enable();
pointLight2:enable();
pointLight3:enable();
material:beginMaterial();
--plane:rotateDeg(spinX, 1, 0, 0);
--plane:rotateDeg(spinY, 0, 1, 0);
plane:draw();
--box:rotateDeg(spinX, 1, 0, 0);
--box:rotateDeg(spinY, 0, 1, 0);
box:draw();
--sphere:rotateDeg(spinX, 1, 0, 0);
--sphere:rotateDeg(spinY, 0, 1, 0);
sphere:draw();
--icoSphere:rotateDeg(spinX, 1, 0, 0);
--icoSphere:rotateDeg(spinY, 0, 1, 0);
icoSphere:draw();
--cylinder:rotateDeg(spinX, 1, 0, 0);
--cylinder:rotateDeg(spinY, 0, 1, 0);
cylinder:draw();
--cone:rotateDeg(spinX, 1, 0, 0);
--cone:rotateDeg(spinY, 0, 1, 0);
cone:draw();
material:endMaterial();
ofDisableLighting();
ofDisableDepthTest();
--[[

--]]
end



