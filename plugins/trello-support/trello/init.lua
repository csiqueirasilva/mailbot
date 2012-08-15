local socket = require "socket"
local string = require "string"
local ltn12 = require "ltn12"
local json = require "json"
local https = require "ssl.https"
local table = table

--[[	
	The following cURL lib can be found here:

	http://msva.github.com/lua-curl/
	
	Make sure you have autoconf installed and lua package set. 
	I had trouble with my lua package, so I created a reference
	for lua5.1 package. The command is "ln -s lua5.1.pc lua.pc".
	Just run this inside /usr/lib/pkgconfig.
]]--
	
local cURL = require "cURL"

module "trello"

local function postHttp ( i_url, i_data )
	local body = {}
	
	local r1, r2, r3, r4 = https.request{
	url = i_url , 
	method = "POST" ,
	headers = {
		["Content-Length"] = i_data:len(),
		["Content-Type"] = "application/x-www-form-urlencoded"
	} ,
	sink = ltn12.sink.table(body),
	source = ltn12.source.string(i_data),
	}
		
	return table.concat(body), r2, r3, r4
	
end

function getMember ( member , token , key )
	local body, code, headers, status = https.request("https://api.trello.com/1/members/" .. member .. "?key=" .. key .. "&token=" .. token )
	return json.arrayToTable ( body ) or status
end

function getBoard ( id , token, key )
	local body, code, headers, status = https.request("https://api.trello.com/1/boards/" .. id .. "?key=" .. key .. "&token=" .. token .. "&lists=all" )
	return json.arrayToTable ( body ) or status
end

function putCard ( id , name , desc , token, key )
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards" , "key=" .. key .. "&token=" .. token .. "&idList=" .. id .. "&name=" .. name .. "&desc=" .. desc )
	return json.arrayToTable ( body ) or status
end

function putAttachmentCard ( id , file , filetype, filename , token, key )
	local c = cURL.easy_init()

	c:setopt_url("https://api.trello.com/1/cards/" .. id .. "/attachments")
	local postdata =  {
		file = {
			file = file,
			type = filetype
		},
		name = filename,
		token = token,
		key = key
	}
	c:post(postdata)

	local ret

	c:perform({ writefunction = function (str)
		ret = str
	end})

	return json.arrayToTable( ret )
end
