package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
local socket = require "socket"
local trello = require "trello"
local userToken = "1f3e317216dc798b80b2a4c43e79018d4581a1091a44bed07753da1c5fddb4ad"

local boards = trello.getMember( "caiosilva", userToken ).idBoards

--[[
print( mailbot.getSubject() )

if mailbot.getSubject() == nil then
	print( "There is no subject text! I will not be able to create a card." )
end

for i=1,#boards do
	local board = trello.getBoard( boards[i], userToken )
	print(board.id .. " - " .. board.name)
	if board.id == "4fda169fb00e9f831ebb5a89" then
		for k,v in pairs(board.lists) do
			print( v.id .. " - " .. v.name )
			if v.id == "4fda169fb00e9f831ebb5a8a" then
				trello.putCard( v.id, mailbot.getSubject(), "Body from mail: " .. mailbot.getBody() .. "\n\n" .. "Mail from: " .. mailbot.getFrom().mail, userToken )
			end
		end
	end
end
]]--

local data = io.open(mailbot.getAttachments()[1].path .. mailbot.getAttachments()[1].name,"rb")

local archive = data:read("*all")

local size = data:seek("end")

data:close()

local server = socket.tcp()
server:bind("10.64.32.127", 24657)
server:listen()

local data = "HTTP/1.1 200 OK\r\n" .. "Content-Disposition: attachment; filename=" .. mailbot.getAttachments()[1].name .. ";\r\n" ..
"Content-Type: " .. mailbot.getAttachments()[1].type .. "\r\n" ..
"Content-Length: " .. size .. "\r\n\r\n" .. archive .. "\r\n\r\n\r\n"

local conn = server:accept()

-- this has to be recursive
local r, pos = conn:send(data, i, data:len())
print(r,pos)
if not r then
	local p
	for p=1,100 do
		if conn:send(data,pos,data:len()) then
			break
		end
	end
end

while true do
	print(conn:send(""))
io.stdin:read("*all")
end

conn:shutdown()
conn:close()
server:close()
