local socket = require "socket"

local data = io.open(arg[1],"rb")

local archive = data:read("*all")

local size = data:seek("end")

data:close()

local server = socket.tcp()
server:bind("10.64.32.127", 24659)
server:listen()

local data = "HTTP/1.1 200 OK\r\n" .. "Content-Disposition: attachment; filename=" .. arg[2] .. ";\r\n" ..
"Content-Type: " .. arg[3] .. "\r\n" ..
"Content-Length: " .. size .. "\r\n\r\n" .. archive .. "\r\n\r\n\r\n"

local conn = server:accept()

server:close()

function sendrec ( )

	local i
	local sz = math.floor( data:len() / 1024 ) - 1
	for i=0,sz do
	
		local r, pos = conn:send ( data, (i*1024)+1, (i+1)*1024 )
		if not r then
			return sendrec ( string.sub(data, pos, data:len() ) )
		end
		socket.sleep(0.05)
	end

	local r, pos = conn:send ( data, ((sz+1)*1024)+1, data:len() )
	if not r then
		return sendrec ( string.sub(data, pos, data:len() ) )
	end
	socket.sleep(1)
	
	return true
	
end

sendrec()
conn:close()

os.remove(arg[1])