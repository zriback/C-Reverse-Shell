# C-Shell

The purpose of this project is to write a Meterpreter like program where reverse/bind shell executables can be easily made. These shells should have the ability to interface with a victim's computer and perform a variety of other attacks. This project is designed to be used with and to target Windows 10 machines.

# Usage

To begin, compile the main/ directory using a C compiler and then run the resulting executable file. With gcc, it would be

```gcc .\main\*.c -o output.exe```
```.\output.exe```

Using the providing interface, setup your desired shell by setting the type, port, and IP address. Use the "help" command in this interface to learn more. Once you have all the correct/desired settings, use the "create" command to create the shell and client executable.

**NOTE:** The project structure is very important as the program assumes it is unchanged when it is compiling the shell and client from their C files. If the relative file structure is changed their will be errors.

At this point, you will have two files both places in a newly created output/ directory.
- bshell.exe / rshell.exe
- bclient.exe / rclient.exe

Their names depend on what type of shell was created - either a bind or reverse shell. Either way, the shell executable is meant to be run on the hypothetical victim's machine, while the client is meant for the attacker's machine. Running both files will initiate a connection between the two computers provided the client and shell files match the computer's are connected to the same local network.

**NOTE:** To ensure the success of the connection, make sure the client and shell files being used in tandem were created together at the same time.

# Capabilities

Once a connection is established, a number of potentially malicious actions on the part of the attacker are supported.

### Remote Shell

The attacker has the ability to execute any and all shell commands on the victim's machine remotely. Anything that works on a Windows 10 command prompt is able to be run by the attacker. When issuing shell commands from the client application, the command must be preceded by "cmd" or "c". For instance, to see the contents of the current working directory, use 
```> c dir```
and to read a file, use
```> c more pathtofile```.

##### Note about working directories
As of 7/16/2022, the current working directory is not maintained between system calls from the client. In other words, issuing the commands
```> c cd C:\```
```> c dir```
will **NOT** list the contents of the C:\ directory. It will simply list the contents of the directory where the shell executable file was run from. The call to cd does nothing. However, to list the contents of the C:\ directory, the command
```> c dir C:\```
can still be used.

### Other commands

There are no external commands unique to this application implemented as of 7/16/2022. However, the goal is to have the following:
```> transfer filename``` - transfer the given file from the victim to the attacker's machine.
```> screenshot``` - send a screenshot of the victim's screen.
```> camera``` - take a send a picture from the victim's webcam

AND MORE!

