# GNU-Linux-Shell
Instructions:

First install the readline development libraries.
For ubuntu and other debian based systems in the terminal do:
sudo apt-get install libreadline-dev 

Make sure all files are in the same folder before compiling.

For compilation include the flag:
-lreadline

for example:
gcc shell.c -lreadline -o bin
./bin

When running the application you can input the help command to get more info.
