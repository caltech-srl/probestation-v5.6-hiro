

echo gcc mterm.c -O -o mterm.exe
gcc mterm.c -O -o mterm.exe

echo gcc msave.c -O -o msave.exe
gcc msave.c -O -o msave.exe

echo gcc scope.c -O -o scope.exe
gcc scope.c -O -o scope.exe

echo gcc mupld.c -O -o mupld.exe
gcc mupld.c -O -o mupld.exe

echo gcc mupld_c.c -O -o mupld_c.exe
gcc mupld_c.c -O -o mupld_c.exe

echo gcc mcmdr.c -lreadline -O -o mcmdr.exe
gcc mcmdr.c -lreadline -O -o mcmdr.exe

cp mterm.exe ..
cp msave.exe ..
cp scope.exe ..
cp mupld.exe ..
cp mupld_c.exe ..
cp mcmdr.exe ..


