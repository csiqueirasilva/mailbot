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

local i = 0

for k,v in pairs(mailbot) do
	i = i + 1
end

-- #table or table.getn does not work since i dont have numerical indexes in this table.

print( "Hello. Im first-plugin! I have an access table named mailbot that has " .. i .. " functions at my disposal." )

print( "Listing them now:\n" )

for k,v in pairs ( mailbot ) do

	print ( 'Function ' .. k .. ' called. Results: ' .. type(v()) or "no result" )
	
end
