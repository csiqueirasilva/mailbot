package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
local trello = require "trello"
local userToken = "1f3e317216dc798b80b2a4c43e79018d4581a1091a44bed07753da1c5fddb4ad"

local boards = trello.getMember( "caiosilva", userToken ).idBoards

for i=1,#boards do
	local board = trello.getBoard( boards[i], userToken )
	print(board.id .. " - " .. board.name)
	if board.id == "4fda169fb00e9f831ebb5a89" then
		for k,v in pairs(board.lists) do
			print( v.id .. " - " .. v.name )
			if v.id == "4fda169fb00e9f831ebb5a8a" then
				trello.putCard( v.id, "Body from mail: " .. mailbot.getBody(), "Mail from: " .. mailbot.getFrom().mail, userToken )
			end
		end
	end
end