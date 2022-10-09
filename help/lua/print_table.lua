function M:print_table(nt, ...)
	local arg = {...}
	local t = nt
	t = _G[nt]

	for _,v in ipairs(arg) do t=t[v] nt = nt.."."..v end
	if not t then print("no such table") return end
	for k, v in pairs(t) do
	print(nt .."."..tostring(k))
	end
end

