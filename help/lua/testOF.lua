--M.sphere = of.SpherePrimitive(10, 50);
of.setWindowTitle("POF window")
M.im = of.Image()
M.im:load(M.getfile("../img/bikers.jpg"))
--print(M.getfile("../img/bikers.jpg"))

function M:draw()
--	if(M.sphere) then
--	print("hello ",swig_type(M.sphere)) ; M.sphere:draw();
--	end
	M.im:draw(0,0)
end

