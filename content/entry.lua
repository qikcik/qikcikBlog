require('common')

function HandleRequest(request)
    local request_exploder = string.gmatch(request, "%S+")
    local method = request_exploder()
    local location = request_exploder()

    --Routes
    if location == "/quick-notes" then
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = GetFileContent("template/quickNotes.html")})
    --Notes
    elseif location == "/quick-note/writing-http-server-and-blog-in-c-and-lua" then
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("quickNotes/writingHttpServerAndBlogInCAndLua.md"))})
    --Static
    elseif location == "/public/style.css" then
        ServeStaticFile("public/style.css");
    --Homepage
    else
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = GetFileContent("template/quickNotes.html")})
    end
end