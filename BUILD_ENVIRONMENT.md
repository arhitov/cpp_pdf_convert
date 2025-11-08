# Windows (MinGW)

1. Установите CMake в MSYS2
pacman -Syu
pacman -S cmake

2. Также установите необходимые инструменты для сборки:
pacman -S base-devel
pacman -S git
pacman -S make

3. Установите необходимые библиотеки
pacman -S mingw-w64-x86_64-lcms2
pacman -S mingw-w64-x86_64-libjpeg-turbo
pacman -S mingw-w64-x86_64-openjpeg2
pacman -S mingw-w64-x86_64-freetype
pacman -S mingw-w64-x86_64-harfbuzz
pacman -S mingw-w64-x86_64-gumbo-parser

4. Установите jbig2dec в MSYS2
pacman -S mingw-w64-x86_64-jbig2dec

