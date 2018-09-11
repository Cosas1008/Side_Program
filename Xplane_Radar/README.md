include library: freeglut

runing environment: MSVS 2015

This is the repo for OpenGL rendering Texture


* Functionality

Load .bmp image and import as `GL_TEXTURE_2D`

* File explanation

1. main.cpp  
    working code for vs2015 with few non-list setups

2. display_bmp.cpp  
    working version simply read 24-bit RGB bmp with OpenGL

3. radar.cpp  
    working version for display the basic radar, need to add on tcp/ip function later

* Usage:

on Linux:  
`g++ prac1.cpp -lGL -lGLU -lglut  -o prac1.o -std=gnu++0x && ./prac1.o`
