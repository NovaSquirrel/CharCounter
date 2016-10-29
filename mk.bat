gcc -Wall -Os -DWIN32 -c charcounter.c
dllwrap --def plugin.def --dllname charcounter.dll charcounter.o