g++ -c C:\A\projects\Kusogemu\source\client.cpp -o C:\A\projects\Kusogemu\source\client.o -I C:\A\SFML\include -D SFML_STATIC
g++ C:\A\projects\Kusogemu\source\client.o -o C:\A\projects\Kusogemu\client.exe -L C:\A\SFML\lib -l sfml-graphics-s -l sfml-window-s -l sfml-network-s -l sfml-system-s -l ws2_32 -l opengl32 -l winmm -l gdi32 -static-libgcc -static-libstdc++
pause