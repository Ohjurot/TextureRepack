

# Scripting Documentation | Lua API

### Handles

Every object created by TexRPLib is stores in a handle for managing lifetime for LUA. There are several function associated with handles. In LUA a handle is presented as an 64-Bit integer. Do never modify the value of a handle or the application may crash or has undefined behavior. 

It is best pratice to close a handle after use (to free all associated resources). However they will all be closed automatically after script termination.

##### Checking the status of a Handle

```lua
booleanReturnType = CheckHandle(handle) -- Handle is integer / handle
```



##### Getting the type of a Handle

```lua
stringReturnType = GetHandleType(handle) -- Handle is integer / handle
```

Possible values are:

- `"Null Handle"`
- `"GPUInterface"`
- `"GPUContext"`
- `"GPUTextureStack"`
- `"GPUGeometryModell"`
- `"GPUMask"`

##### Closing a handle

```lua
CloseHandle(handle) -- Handle is integer / handle
```



**!!!! EVERY FUNCTION THAT RETURNS AN HANDLE MAY RETURN `NIL` ON FAILURE !!!**



------

### TexRPLib Lua Bindings

##### Enumerating GPUs

```lua
handleReturnType = EnumGpuInterfaces(0) -- Index of gpu is system returns handle
```

Returns a handle of type `GPUInterface` when GPU index is valid. Returns Nil when no GPU with that index is available. Handle needs to be closed after usage.

#### IGPUInterface (Functions for `GPUInterface` handles)

##### Retrieving the name of the GPU

```lua
stringReturnType = IGPUInterface_GetGPUName(gpuInterfaceHandle) -- First parameter must be a valid handle of type GPUInterface
```

##### Retrieving the GPU unique ID

```lua
integerReturnType = IGPUInterface_GetGPUUniqueId(gpuInterfaceHandle) -- First parameter must be a valid handle of type GPUInterface
```

##### Retrieving the currently free VRAM

```lua
integerReturnType = IGPUInterface_GetGPUFreeVRAM(gpuInterfaceHandle) -- First parameter must be a valid handle of type GPUInterface
```

##### Creating a GPU context

```lua
handleReturnType = IGPUInterface_CreateContex(gpuInterfaceHandle) -- First parameter must be a valid handle of type GPUInterface
```

Returns a handle of type `GPUContext`. A context is required to load geometry and textures to the GPU and for merging textures. Handle needs to be closed after usage.



#### IGPUContext (Functions for `GPUContext` handles)

##### Creating a Texture Stack

```lua
handleReturnType = IGPUContext_CreateTextureStack(gpuContextHandle) -- First parameter must be a valid handle of type GPUContext
```

Returns a handle of type `GPUTextureStack`. A texture stack is holding all textures (Source and destination textures for merge). Handle needs to be closed after usage.

##### Loading a 3D-Model

```lua
handleReturnType = IGPUContext_OpenModell(gpuContextHandle, ".\\mypath\\modell.fbx") -- First parameter must be a valid handle of type GPUContext
```

Returns a handle of type `GPUGeometryModell`. In case the model is invalid Nil is returned. Represents the 3D Model on the GPU. Handle needs to be closed after usage.

##### Merging textures

```lua
-- gpuContextHandle: must be a valid handle of type GPUContext
-- textureStackHandle: must be a valid handle of type GPUTextureStack
-- outputIndex: must be a integer representing a texture in the given texture stack (textureStackHandle)
-- inputCount: must specify the numbers of input textures (and masks) merged together
-- masks... : one or many handles of type IGPUMask
-- inputIndex... : one or many indicies representing a texture in the given texture stack (textureStackHandle)
booleanReturnType = IGPUContext_MergTextures(gpuContextHandle, textureStackHandle, outputIndex, inputCount, masks..., inputIndex...) 
```



#### IGPUGeometryModell (Functions for `GPUGeometryModell` handles)

##### Retrieving the submodel count (Meshes)

```lua
integerReturnType = IGPUGeometryModell_GetSubmodellCount(gpuGeometryModellHandle) -- First parameter must be a valid handle of type GPUGeometryModell
```

##### Retrieving the name of a submodel

```lua
stringReturnType = IGPUGeometryModell_GetSubmodellName(gpuGeometryModellHandle, submodellIndex) -- First parameter must be a valid handle of type GPUGeometryModell
```

##### Retrieving the material name of a submodel

```lua
stringReturnType = IGPUGeometryModell_GetSubmodellMaterialName(gpuGeometryModellHandle, submodellIndex) -- First parameter must be a valid handle of type GPUGeometryModell
```

##### Retrieving the UV set index of a submodel

```lua
integerReturnType = IGPUGeometryModell_GetUVSetIndex(gpuGeometryModellHandle, submodellIndex) -- First parameter must be a valid handle of type GPUGeometryModell
```

##### Retrieving the vertex count of a submodel

```lua
integerReturnType = IGPUGeometryModell_GetVerticesCount(gpuGeometryModellHandle, submodellIndex) -- First parameter must be a valid handle of type GPUGeometryModell
```

##### Creating a mask for a submodel

```lua
-- gpuGeometryModellHandle: must be a valid handle of type GPUGeometryModell
-- submodellIndex: Index of the submodel to gennerate mask for
-- maskResolution: Resolution of the mask (should be eqal to texture resoltion)
handleReturnType = IGPUGeometryModell_CreateMask(gpuGeometryModellHandle, submodellIndex, maskResolution)
```

Returns a handle of type `GPUMask`. Representing the coverage of the submodel in texture space (UV). Used to mask all pixels from the source texture that corresponds to that model. Handle needs to be closed after usage.



#### IGPUTextureStack (Functions for `GPUTextureStack` handles)

##### Reseting the texture stack (required at least once!)

```lua
-- textureStackHandle: must be a valid handle of type GPUTextureStack
-- width: Maximum texture width
-- height: Maximum texture height
-- bitsPerPixel: can be 32 / 64 (8-Bit / 16-Bit)
-- count: maximum number of textures
booleanReturnType = IGPUTextureStack_Reset(textureStackHandle, width, height, bitsPerPixel, count)
```

##### Loading textures from disk

```lua
textureIndexInteger = IGPUTextureStack_LoadFromDisk(textureStackHandle, ".\\pathto\\image.png") -- First parameter must be a valid handle of type GPUTextureStack 
```

Returned index used to modify the texture and reference it while merging

##### Creating an empty texture from a template

```lua
textureIndexInteger = IGPUTextureStack_CreateEmpty(textureStackHandle, templateIndex, ".\\pathto\\dummy_image.png") -- First parameter must be a valid handle of type GPUTextureStack 
```

Path is used to safe the texture. Template index is returned from Load From Disk. Parameters will be copied name is changed. Returned index used to modify the texture and reference it while merging

##### Clear a texture to zero

```lua
booleanReturnType = IGPUTextureStack_ClearTexture(textureStackHandle, textureIndex) -- First parameter must be a valid handle of type GPUTextureStack 
```

##### Safe a texture to disk

```lua
booleanReturnType = IGPUTextureStack_SafeToDisk(textureStackHandle, textureIndex) -- First parameter must be a valid handle of type GPUTextureStack 
```

##### Rename a texture

```lua
booleanReturnType = IGPUTextureStack_Rename(textureStackHandle, textureIndex, ".\\newpathto\\image.png") -- First parameter must be a valid handle of type GPUTextureStack 
```

Will not reload the texture. Will rename the texture for saving --> not overriding

### 

------

### Helper Functions

#### Console In-/Output

TexRPCli provides an easy to use console that can be used for input and output.

##### Setting the consoles color

```lua
ConsoleColor(CONSOLE_COLOR_XXXX) -- Parameter is global constant
```

Where CONSOLE_COLOR_XXXX can be: `CONSOLE_COLOR_BLACK`, `CONSOLE_COLOR_WHITE`, `CONSOLE_COLOR_RED`, `CONSOLE_COLOR_GREEN`, `CONSOLE_COLOR_BLUE`, `CONSOLE_COLOR_LIGHT_RED`, `CONSOLE_COLOR_LIGHT_GREEN`, `CONSOLE_COLOR_LIGHT_BLUE` or `CONSOLE_COLOR_ORANGE`

##### Setting the consoles name / title

```lua
ConsoleTitle("Name of Console")
```

##### Writing to the console

```lua
ConsoleWrite("Text without an appended newline")
ConsoleWriteLine("Text with newline at end")
```

Reading from the console (User text input)

```lua
stringReturnType = ConsoleReadLine()
```



#### Basic file operations

##### Retrieving magic directory paths

```lua
stringReturnType = GetAppDir() -- Retrives the applications install directory
stringReturnType = GetWorkDir() -- Retrives the current working directory
stringReturnType = GetUserDir() -- Retrives the current users document directory
```

##### Basic file operations

```lua
booleanReturnType = FileExists(".\\myFile.txt") -- Checks if a file exists
booleanReturnType = FileCopy(".\\myFile.txt", ".\\myFile_copy.txt") -- Copys a file
booleanReturnType = FileDelete(".\\myFile.txt") -- Deletes a file
booleanReturnType = FileMove(".\\myFile.txt", ".\\output\\myFile.txt") -- Moves / Renames a file
```



#### Environment

##### Application return code

```lua
ReturnCode(0) -- Sets the applications return code
```

##### Command line arguments

```lua
stringReturnType = GetCmdArg(index) -- Used to retrive args that are passed without a name "texrp myscript value" --> "value"
stringReturnType = GetCmdArg("name") -- Used to retrive args that are passed with a name "texrp myscript name=value" --> "value"
```





**Remarks**: There are a few more undocumented function that are not critical for generic usage. Fell free to play around!