--[[print("this is the second plugin running!")
print(type(mailbot.getBody))
print(mailbot.getBody(-1,"a"))]]--

print(type(mailbot.getBcc()[2].name))
if mailbot.getBcc()[2].name == "" then
	print("Empty String")
end
