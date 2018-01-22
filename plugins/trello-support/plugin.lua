package.path = package.path .. ";/var/dev/mail-bot/plugins/trello-support/?/init.lua"
package.path = package.path .. ";/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/ssl/?.lua"
package.cpath = package.cpath .. ";/usr/local/lib/lua/5.1/?.so;/usr/local/lib/lua/5.1/?/core.so"

local smtp = require("socket.smtp")
local trello = require "trello"
local socket = require("socket")

local validEmails = {"csiqueirasilva@gmail%.com", ".+@ccesp%.puc%-rio%.br", ".+@esp%.puc%-rio%.br", ".+@puc%-rio%.br"}

local invalidEmail = true
local fromMail = mailbot.getFrom().mail

if(fromMail:find("MAILER%-DAEMON@ccesp%.puc%-rio%.br")) then
	return
end

for k,v in ipairs ( validEmails ) do
	if fromMail:find(v) then
		invalidEmail = false
		break
	end
end

if invalidEmail then
	print ( "Received email from non-matching address! I will not be able to create a card." ) -- This is gonna be logged.
	return
end

if not mailbot.getSubject() then -- We also need an error for listId, appKey and userToken
	print ( "There's is no subject! I will not be able to create a card." ) -- This is gonna be logged. 
	return
end

local refNumber = socket.gettime() * 1000;

local cardid = trello.putCardLabelDue( listId, mailbot.getSubject() .. " [#" .. refNumber .. "]", "Mail from: " .. ( mailbot.getFrom().mail or "" ) .. "\nBody from mail:\n\n" .. ( mailbot.getBody() or "" ), labelId, userToken, appKey ).id

for k,v in pairs ( mailbot.getAttachments() or {} ) do
	trello.putAttachmentCard( cardid , v.path .. v.name, v.type, v.name, userToken, appKey )
end

local mesgt = {
  headers = {
    to = "<" .. mailbot.getFrom().mail .. ">",
	cc = "<informatica@ccesp.puc-rio.br>",
	subject = "Chamado #" .. refNumber .. " de suporte criado: " .. mailbot.getSubject()
  },
  body = "Seu chamado foi criado no sistema com sucesso.\nUma resposta a esse chamado virá em até 1 (um) dia útil.\n\nNúmero de referência: " .. refNumber .. "\nLink do chamado de suporte no sistema Trello: https://trello.com/c/" .. cardid .. "\n\nMensagem original recebida:\n\n===================\n\n" .. mailbot.getBody()
}

mesgt.headers['content-type'] = 'text/plain; charset=UTF-8'

smtp.send{
	from = "<informatica@ccesp.puc-rio.br>",
	rcpt = {"<" .. mailbot.getFrom().mail .. ">", "<informatica@ccesp.puc-rio.br>"},
	source = smtp.message(mesgt)
}