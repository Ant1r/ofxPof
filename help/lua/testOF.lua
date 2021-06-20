--M.sphere = of.SpherePrimitive(10, 50);
of.setWindowTitle("POF window")
M.im = of.Image()
M.im:load(M.getfile("../img/bikers.jpg"))
--print(M.getfile("../img/bikers.jpg"))

M.doc = pugixml.xml_document()
M.doc:load_file(M.getfile("../data/food.xml"))
local nodes = M.doc:select_nodes("/breakfast_menu/food")
print("size: ", nodes:size())
for n=1,nodes:size() do 
	local node = nodes:get(n-1):node()
	local child = node:first_child()
	print(n,":", nodes:get(n-1):node():attribute("id"):value(), " name: ", node:child_value("name"))
	end

function M:draw()
--	if(M.sphere) then
--	print("hello ",swig_type(M.sphere)) ; M.sphere:draw();
--	end
	M.im:draw(0,0)
end

