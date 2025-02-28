function interp(s, tab)
    return (s:gsub('($%b{})', function(w) return tab[w:sub(3, -2)] or w end))
end

function string.starts(String,Start)
    return string.sub(String,1,string.len(Start))==Start
end


getmetatable("").__mod = interp