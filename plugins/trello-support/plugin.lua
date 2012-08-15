package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
local trello = require "trello"

if not mailbot.getSubject() then -- We also need an error for listId, appKey and userToken
	print ( "There's is no subject! I will not be able to create a card." ) -- This is gonna be logged. 
	return
end

local cardid = trello.putCard( listId, mailbot.getSubject(), "Body from mail: " .. ( mailbot.getBody() or "" ) .. "\n\n" .. "Mail from: " .. ( mailbot.getFrom().mail or "" ), userToken, appKey ).id

for k,v in pairs ( mailbot.getAttachments() or {} ) do
	trello.putAttachmentCard( cardid , v.path .. v.name, v.type, v.name, userToken, appKey )
end