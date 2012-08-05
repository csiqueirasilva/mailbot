
--[[
function print_r ( tab )

	if type(tab) == "table" then
		for k,v in pairs ( tab ) do
			io.write(k .. ": " )
			print_r(v)
		end
	else
		print(tab)
	end

end

for k,v in pairs ( mailbot ) do
	print ( '\nFunction ' .. k .. ' called. Results:' )

	if v() ~= nil then
		if type(v()) == "table" then
			print("Table has this elements:")			
			for i,j in pairs ( v() ) do
				io.write( i .. ': ')
				print_r(j)
			end
		else
			print("Simple value:")
			print(v())
		end
	else
		print("There was no results for this")
	end
end

]]--
