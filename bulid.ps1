Set-PSDebug -Trace 2
gcc .\main.c -o main.exe -g -Wall -Wextra -pedantic
Set-PSDebug -Off