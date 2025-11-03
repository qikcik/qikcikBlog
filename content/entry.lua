require('common')

function HandleRequest(request)
    local request_exploder = string.gmatch(request, "%S+")
    local method = request_exploder()
    local location = request_exploder()

    --Routes

    if string.starts(location,"/public") then
        ServeStaticFile(location);
    else
        --main
        if location == "/articles" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = GetFileContent("template/articles.html")})
        elseif location == "/notes" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = GetFileContent("template/notes.html")})
            --Notes
        elseif location == "/chip8-emulator-assembler-game-vhdl" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("posts/chip8EmulatorAssemblerGameVhdl.md"))})
        elseif location == "/writing-http-server-and-blog-in-c-and-lua" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("posts/writingHttpServerAndBlogInCAndLua.md"))})
        elseif location == "/my-attempt-to-implement-minimalistic-flexbox-layout" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("posts/myAttemptToImplementMinimalisticFlexboxLayout.md"))})
        elseif location == "/i-am-terrible-at-math" then
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("posts/iAmTerribleAtMath.md"))})
        --Homepage
        else
            PrintDefaultHtmlOkHeader()
            Print(GetFileContent("template/main.html") % { content = GetFileContent("template/articles.html")})
        end
    end
end