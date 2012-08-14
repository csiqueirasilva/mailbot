local socket = require "socket"
local string = require "string"
local ltn12 = require "ltn12"
local json = require "json"
local https = require "ssl.https"
local table = table

module "trello"

local appKey = "366aaf80a65cc6cce3b8576b85ae0303"

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

function getMember ( member , token )
	local body, code, headers, status = https.request("https://api.trello.com/1/members/" .. member .. "?key=" .. appKey .. "&token=" .. token )
	return json.arrayToTable ( body ) or status
end

function getBoard ( id , token )
	local body, code, headers, status = https.request("https://api.trello.com/1/boards/" .. id .. "?key=" .. appKey .. "&token=" .. token .. "&lists=all" )
	return json.arrayToTable ( body ) or status
end

function putCard ( id , name , desc , token )
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards" , "key=" .. appKey .. "&token=" .. token .. "&idList=" .. id .. "&name=" .. name .. "&desc=" .. desc )
	return json.arrayToTable ( body ) or status
end

function putAttachmentCard ( id , url , name , token )
	local body, code, headers, status = postHttp ("https://api.trello.com/1/cards/" .. id .. "/attachments" , "key=" .. appKey .. "&token=" .. token .. "&name=" .. name .. "&url=" .. url )
	return json.arrayToTable ( body ) or status
end