local type = type
local load = load

module "json"

function arrayToTable ( str )

	if type(str) == "string" then
		local buff = str:gsub("%[","{"):gsub("%]","}"):gsub("(\")(%w+)(\":)","%2="):gsub("[\n\r]","")
		return load("return " .. buff)
	end

	return str
end
