package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
local trello = require "trello"
local userToken = "1f3e317216dc798b80b2a4c43e79018d4581a1091a44bed07753da1c5fddb4ad"

local boards = trello.getMember( "caiosilva", userToken ).idBoards

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
				local data = io.open(mailbot.getAttachments()[1].path .. mailbot.getAttachments()[1].name,"rb")
				local tname = os.tmpname()
				local wdata = io.open(tname, "wb")

				wdata:write(data:read("*all"))
				wdata:close()
				data:close()
				os.execute("sudo lua /var/dev/mail-bot/plugins/trello-support/sendfile.lua " .. tname .. " " .. mailbot.getAttachments()[1].name .. " " .. mailbot.getAttachments()[1].type .. " &")
				local data = io.open(mailbot.getAttachments()[1].path .. mailbot.getAttachments()[1].name,"rb")
				trello.putAttachmentCard( trello.putCard( v.id, mailbot.getSubject(), "Body from mail: " .. mailbot.getBody() .. "\n\n" .. "Mail from: " .. mailbot.getFrom().mail, userToken ).id , "http://hawaii-land.dyndns.tv:24659", mailbot.getAttachments()[1].name, userToken )
				data:close()
			end
		end
	end
end