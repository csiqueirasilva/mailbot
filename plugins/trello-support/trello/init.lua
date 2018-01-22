local socket = require "socket"
local string = require "string"
local ltn12 = require "ltn12"
local JSON = require "JSON"
local https = require "ssl.https"
local table = table
local os = os
	
local cURL = require "cURL"

module "trello"

local function putHttp ( i_url, i_data )
	local body = {}
	
	local r1, r2, r3, r4 = https.request{
	url = i_url , 
	method = "PUT" ,
	headers = {
		["Content-Length"] = i_data:len(),
		["Content-Type"] = "application/x-www-form-urlencoded"
	} ,
	sink = ltn12.sink.table(body),
	source = ltn12.source.string(i_data),
	}
		
	return table.concat(body), r2, r3, r4
end


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
	return JSON:decode ( body ) or status
end

function getBoard ( id , token, key )
	local body, code, headers, status = https.request("https://api.trello.com/1/boards/" .. id .. "?key=" .. key .. "&token=" .. token .. "&lists=all" )
	return JSON:decode ( body ) or status
end

function putCard ( id , name , desc , token, key )
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards" , "key=" .. key .. "&token=" .. token .. "&idList=" .. id .. "&name=" .. name .. "&desc=" .. desc .. "&pos=top" )
	return JSON:decode ( body ) or status
end

function addDueDate ( id, token, key )
	local dueDate = os.date("%Y-%m-%d %H:%M:%S",os.time()+(24*60*60))
	local body, code, headers, status = putHttp ("https://api.trello.com/1/cards/" .. id .. "/due" , "key=" .. key .. "&token=" .. token .. "&value=" .. dueDate )	
	return JSON:decode ( body ) or status
end

function addLabel ( id, label, token, key )
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards/" .. id .. "/idLabels" , "key=" .. key .. "&token=" .. token .. "&value=" .. label )
	return JSON:decode ( body ) or status
end

function putCardLabelDue ( id , name , desc , label, token, key )
	local due = os.date("%Y-%m-%d %H:%M:%S",os.time()+(24*60*60))
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards" , "key=" .. key .. "&token=" .. token .. "&idList=" .. id .. "&name=" .. name .. "&desc=" .. desc .. "&pos=top" .. "&due=" .. due .. "&idLabels=" .. label)
	return JSON:decode ( body ) or status
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

	return JSON:decode( ret )
end
