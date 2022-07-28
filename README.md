# C-Shell

The purpose of this project is to write a Meterpreter like program where reverse/bind shell executables can be easily made. These shells should have the ability to interface with a victim's computer and perform a variety of other attacks. This project is designed to be used with and to target Windows 10 machines.

# Usage

To begin, compile the main/ directory using a C compiler and then run the resulting executable file. With gcc, it would be

```gcc .\main\*.c -o output.exe```

```.\output.exe```

Using the provided interface, setup your desired shell by setting the type, port, and IP address. Use the "help" command in this interface to learn more. Once you have all the correct/desired settings, use the "create" command to create the shell and client executable.

**NOTE:** The project structure is very important as the program assumes it is unchanged when it is compiling the shell and client from their C files. If the relative file structure is changed there will be errors.

At this point, you will have two files both placed in a newly created output/ directory.
- bshell.exe / rshell.exe
- bclient.exe / rclient.exe

Their names depend on what type of shell was created - either a bind or reverse shell. Either way, the shell executable is meant to be run on the hypothetical victim's machine, while the client is meant for the attacker's machine. Running both files will initiate a connection between the two computers provided the client and shell files match and the attacker and victim are connected to the same local network.

**NOTE:** To ensure the success of the connection, make sure the client and shell files being used in tandem were created together at the same time.

# Capabilities

Once a connection is established, a number of potentially malicious actions on the part of the attacker are supported.

### Remote Shell

The attacker has the ability to execute any and all shell commands on the victim's machine remotely. Anything that works on a Windows 10 command prompt is able to be run by the attacker. When issuing shell commands from the client application, the command must be preceded by "cmd" or "c". For instance, to see the contents of the current working directory, use 

```> c dir```

and to read a file, use

```> c more file```.

##### Working directories

Its also important to note that the attacker can change directory around the victims machine like normal. To do so, use commands like

```> c cd ..``` - move to parent directory

```> c cd directory_name/``` - use relative path

```> c cd C:\Users``` - use absolute path

```> c cd D:``` - change drives (in this case, to the D drive)

The default working directory will be set to the current users home directory.

### Other commands

There are also some external commands that can be run by the client. These commands are notably not preceded by a "c" or "cmd" on the command line.

```> transfer filename``` - transfer the given file from the victim to the attacker's machine.

```> screenshot``` - send a screenshot of the victim's screen.

