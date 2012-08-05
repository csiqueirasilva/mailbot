local type = type
local loadstring = loadstring

module "json"

function arrayToTable ( str )

	if type(str) == "string" then
		local buff = str:gsub("%[","{"):gsub("%]","}"):gsub("(\")(%w+)(\":)","%2=")
		return loadstring("return " .. buff)()
	end

	return str
end
