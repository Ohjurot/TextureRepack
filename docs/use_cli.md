# Command Line User Guide (texrp)

#### General Usage

```bash
texrp <SYSTEM_PARAMETERS> [SCRIPT_NAME] <SCRIPT_PARAMETERS>
```

- texrp
  - Name of Command (Only available when installed via the installer)
- SYSTEM_PARAMETERS  (Optional)
  - System Parameters (Starting with a dash)
- SCRIPT_NAME (Required)
  - Name of the script to be executed
- SCRIPT_PARAMETERS (Required)
  - Parameters passed to the script

A list of by default shipped scripts and their documentation can be found [here](./script_list.md)



#### System Parameters

| Parameter  | Description                            |
| ---------- | -------------------------------------- |
| `-nocolor` | Disables colored output in the console |
| `-silent`  | Disables script console output         |



#### Script Name

The name of the script that will be executed. Script will be searched by default like that:

```
${NAME}
${APP_DIR}\scripts\${NAME}
${APP_DIR}\scripts\${NAME}.lua
${USR_DIR}\scripts\${NAME}
${USR_DIR}\scripts\${NAME}.lua
${RUN_DIR}\${NAME}
${RUN_DIR}\${NAME}.lua
```

Where

- `NAME` is the name of the script (command line input)
- `APP_DIR` is the installation directory of the application
- `USR_DIR` is the user directory (Windows document directory)
- `RUN_DIR` is the current execution directory (Directory where the command line was opened)

You can adjust thees path by editing the file `paths.txt` in the applications installation directory.



#### Script Parameters

Parameters passed to the lua script. 

The synax can be like this

```
INPUT_VALUE
```

or like this

```
INPUT_VALUE_NAME=INPUT_VALUE
```

