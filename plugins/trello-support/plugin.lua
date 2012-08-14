package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
local trello = require "trello"
local userToken = "1f3e317216dc798b80b2a4c43e79018d4581a1091a44bed07753da1c5fddb4ad"

local boards = trello.getMember( "caiosilva", userToken ).idBoards

if mailbot.getSubject() == nil then
	print( "There is no subject text! I will not be able to create a card." )
else
	local cardid = trello.putCard( "4fda169fb00e9f831ebb5a8a", mailbot.getSubject(), "Body from mail: " .. mailbot.getBody() .. "\n\n" .. "Mail from: " .. mailbot.getFrom().mail, userToken ).id

	for k,v in pairs ( mailbot.getAttachments() ) do
		trello.putAttachmentCard( cardid , v.path .. v.name, v.type, v.name, userToken )
	end
end
