# ofi
http server made in C in one night using **winsock2** and using more **legacy** libs for Linux

## usage for Windows:
```$ gcc -o ofi server.c -lws2_32``` <br>
```$ ./ofi.exe```

## usage for Linux:
```$ gcc -o linuxserver Linuxserver.c``` <br>
```$ ./linuxserver```

navigate to `http://localhost:8080/`

### additional note:
if you can find a better way to read the html, pull requests are open :) 