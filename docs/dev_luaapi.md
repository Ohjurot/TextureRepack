# Scripting Documentation | Lua API

### Handles

Every object created by TexRPLib is stores in a handle for managing lifetime for LUA. There are several function associated with handles. In LUA a handle is presented as an 64-Bit integer. Do never modify the value of a handle or the application may crash or has undefined behavior. 

It is best pratice to close a handle after use (to free all associated resources). However they will all be closed automatically after script termination.

##### Checking the status of a Handle

```lua
booleanReturnValue = CheckHandle(handle) -- Handle is integer / handle
```



##### Getting the type of a Handle

```lua
stringReturnValue = GetHandleType(handle) -- Handle is integer / handle
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

------

### TexRPLib Lua Bindings



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



