-- Console title
ConsoleTitle("TexRP - Substance Painter Interactive Script")

-- Get and check GPU interface
interface = EnumGpuInterfaces(0)
if CheckHandle(interface) then
    ConsoleColor(CONSOLE_COLOR_LIGHT_GREEN)
    ConsoleWriteLine("TexRP Running on: " .. IGPUInterface_GetGPUName(interface))
    ConsoleColor(CONSOLE_COLOR_WHITE)

    -- Create and check context
    context = IGPUInterface_CreateContex(interface)
    if CheckHandle(context) then
        -- Create texture stack
        textures = IGPUContext_CreateTextureStack(context)
        if CheckHandle(textures) then
            -- Enter name
            ConsoleColor(CONSOLE_COLOR_LIGHT_BLUE)
            ConsoleWrite("Please enter modell name (without .fbx): ")
            ConsoleColor(CONSOLE_COLOR_WHITE)
            mdlName = ConsoleReadLine()
            -- Enter mdl extension
            ConsoleColor(CONSOLE_COLOR_LIGHT_BLUE)
            ConsoleWrite("Please enter modell extension (only fbx/obj/...): ")
            ConsoleColor(CONSOLE_COLOR_WHITE)
            mdlExt = ConsoleReadLine()
            -- Enter img extension
            ConsoleColor(CONSOLE_COLOR_LIGHT_BLUE)
            ConsoleWrite("Please enter image extension (only png/bmp/...): ")
            ConsoleColor(CONSOLE_COLOR_WHITE)
            imgExt = ConsoleReadLine()
            -- Enter img resolution
            ConsoleColor(CONSOLE_COLOR_LIGHT_BLUE)
            ConsoleWrite("Please enter image resolution (1024/2048/4096/...): ")
            ConsoleColor(CONSOLE_COLOR_WHITE)
            imgResStr = ConsoleReadLine()
            imgRes = tonumber(imgResStr)

            -- Build path and check
            fbxPath = GetWorkDir() .. "\\" .. mdlName .. "." .. mdlExt
            if FileExists(fbxPath) and imgRes then
                -- Open modell
                modell = IGPUContext_OpenModell(context, fbxPath)
                if CheckHandle(modell) then
                    -- Echo modell
                    ConsoleWriteLine("\nDetails of: \"" .. fbxPath .. "\"")
                    subCount = IGPUGeometryModell_GetSubmodellCount(modell)
                    masks = {}
                    for i=0,subCount-1 do
                        table.insert(masks, IGPUGeometryModell_CreateMask(modell, i, imgRes))
                        ConsoleWriteLine("- " .. IGPUGeometryModell_GetSubmodellName(modell, i) .. " UV(" .. IGPUGeometryModell_GetUVSetIndex(modell, i) .. ") Vertex Count: " .. IGPUGeometryModell_GetVerticesCount(modell, i))
                    end

                    -- Input workflow
                    ConsoleWriteLine("\nSelect your workflow of choice:")
                    ConsoleWriteLine("[0] BaseColor, Height, Metallic, Normal, Roughness")
                    ConsoleWriteLine("[x] Abort")
                    ConsoleColor(CONSOLE_COLOR_LIGHT_BLUE)
                    ConsoleWrite("> ")
                    ConsoleColor(CONSOLE_COLOR_WHITE)
                    workflowIn = ConsoleReadLine();
                    worflowArray = nil

                    -- Evaluate input
                    if workflowIn == "0" then
                        worflowArray = {"BaseColor", "Height", "Metallic", "Normal", "Roughness"};
                    end

                    -- Run workflow
                    if worflowArray then
                        -- Compute texture count
                        texureCount = IGPUGeometryModell_GetSubmodellCount(modell) + 1
                        
                        -- Process each texture
                        for k,name in pairs(worflowArray) do
                            -- Reset texture stack
                            if IGPUTextureStack_Reset(textures, imgRes, imgRes, 32, texureCount) then
                                -- Index array
                                indexArr = {}
                                for i=0,subCount-1 do
                                    -- Build texture path
                                    imgPath = GetWorkDir() .. "\\" .. mdlName .. "_" .. IGPUGeometryModell_GetSubmodellMaterialName(modell, i) .. "_" .. name .. "." .. imgExt
                                    -- Check and load
                                    table.insert(indexArr, IGPUTextureStack_LoadFromDisk(textures, imgPath))
                                end
                                
                                if #indexArr == subCount then
                                    outputFile = GetWorkDir() .. "\\" .. mdlName .. "_" .. name .. "." .. imgExt
                                    indexTarget = IGPUTextureStack_CreateEmpty(textures, indexArr[1], outputFile)
                                    if indexTarget then
                                        -- Concat tables
                                        tempTable = {}
                                        for k,v in pairs(masks) do
                                            table.insert(tempTable, v)
                                        end
                                        for k,v in pairs(indexArr) do
                                            table.insert(tempTable, v)
                                        end

                                        -- Merge
                                        if IGPUContext_MergeTextures(context, textures, indexTarget, subCount, table.unpack(tempTable)) then
                                            -- Save to disk
                                            if IGPUTextureStack_SafeToDisk(textures, indexTarget) then
                                                ConsoleWriteLine("Writing \"" .. outputFile .. "\"")
                                            else
                                                ConsoleColor(CONSOLE_COLOR_RED)
                                                ConsoleWriteLine("Failed to write \"" .. outputFile .. "\"")
                                                ConsoleColor(CONSOLE_COLOR_WHITE)
                                            end
                                        else
                                            ConsoleColor(CONSOLE_COLOR_RED)
                                            ConsoleWriteLine("Failed to merge textures for " .. name)
                                            ConsoleColor(CONSOLE_COLOR_WHITE)
                                        end
                                    else
                                        ConsoleColor(CONSOLE_COLOR_RED)
                                        ConsoleWriteLine("Failed to allocate empty texture for " .. name)
                                        ConsoleColor(CONSOLE_COLOR_WHITE)
                                    end
                                else
                                    ConsoleColor(CONSOLE_COLOR_RED)
                                    ConsoleWriteLine("Failed to load textures for " .. name)
                                    ConsoleColor(CONSOLE_COLOR_WHITE)
                                end
                            else
                                ConsoleColor(CONSOLE_COLOR_RED)
                                ConsoleWriteLine("Failed to reset texture stack")
                            end
                        end
                    end

                    -- Close
                    for k,mask in pairs(masks) do
                        CloseHandle(mask)
                    end 
                    CloseHandle(modell)

                    -- Notify
                    ConsoleWriteLine("Merge finished")
                else
                    ConsoleColor(CONSOLE_COLOR_RED)
                    ConsoleWriteLine("Failed to open modell \"" .. fbxPath .. "\"")
                end
            else
                ConsoleColor(CONSOLE_COLOR_RED)
                ConsoleWriteLine("Failed to open modell \"" .. fbxPath .. "\"")
            end

            -- Close Handle
            CloseHandle(textures)
        else
            ConsoleColor(CONSOLE_COLOR_RED)
            ConsoleWriteLine("Failed to create texture stack")
        end

        -- Close hanlde
        CloseHandle(context)
    else
        ConsoleColor(CONSOLE_COLOR_RED)
        ConsoleWriteLine("Failed to create context")
    end

    -- Close hanlde
    CloseHandle(interface)
else
    ConsoleColor(CONSOLE_COLOR_RED)
    ConsoleWriteLine("Failed to enum GPU")
end
