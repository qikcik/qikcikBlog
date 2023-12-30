function insertVariable(s, tab)
  return (s:gsub('($%b{})', function(w) return tostring(tab[w:sub(3, -2)]) or w end))
end

function OnNewRequest(request)
    local request_exploder = string.gmatch(request, "%S+")
    local method = request_exploder()
    local location = request_exploder()

    local view = {location = location}
    setmetatable (view,
      {
        __tostring = function (t)
            return "<h1>" .. t.location .. "</h1>"
        end
      }
    )

    local response = insertVariable ([[
        ${header}
    ]], {header = view})

	return response
end
