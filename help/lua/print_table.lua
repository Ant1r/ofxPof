function M:print_table(nt, ...)
	local args = {...}
	local t = _G[nt]
	if nt == "self" then t = self end
	local res = {}

	for _,v in ipairs(args) do t=t[v] nt = nt.."."..v end

	if not t then print("no such table") return end

	for k, v in pairs(t) do
		table.insert(res, k)
	end

	table.sort(res)

	print("\ncontent", "of", nt, ":")
	for _, v in ipairs(res) do
		print(nt .."."..tostring(v))
	end
end

