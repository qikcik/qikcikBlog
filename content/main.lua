package.path = 'content/?.lua;' .. package.path
require('common')


getmetatable("").__mod = interp

function OnNewRequest(request)
    local request_exploder = string.gmatch(request, "%S+")
    local method = request_exploder()
    local location = request_exploder()

    --return [[
    --  <head></head>
    --  <html>
    --     <h2> ${header} </h2>
    --  </html>
    --]] % { header = location }
    local view = {}
    view.location = location
    view.method = method
    return "test23"
end
--