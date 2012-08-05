local socket = require "socket"
local string = require "string"
local ltn12 = require "ltn12"
local json = require "json"
local https = require "ssl.https"

module "trello"

local appKey = "366aaf80a65cc6cce3b8576b85ae0303"

local function postHttp ( i_url, i_data )
	return https.request{
	url = i_url , 
	method = "POST" ,
	headers = {
		["Content-Length"] = i_data:len(),
		["Content-Type"] = "application/x-www-form-urlencoded"
	} ,
	source = ltn12.source.string(i_data),
	}
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
	local body, code, headers, status = postHttp ( "https://api.trello.com/1/cards" , "key=" .. appKey .. "&token=" .. token .. "&idList=" .. id .. "&name=" .. name .. "&desc=" .. desc )
	return json.arrayToTable ( body ) or status
end
