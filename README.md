# TMX Map Engine
### A tiled map engine for tmx format using SDL2

---

##About
This library provides tools for tile mapping with SDL2 such as :

* Scrolling
* Collisions
* Event management (lethal tiles,water, etc)

##Dependencies

This project depends on **[Tmx](https://github.com/baylej/tmx)** by Baylej, **[SDL2](https://www.libsdl.org/index.php)** and **[SDL Image 2.0](https://www.libsdl.org/projects/SDL_image/)**.

##Compiling

To compile, you just need to write

```
make
```

##Using tmx-map-engine in a project
Make sure that you have correctly installed **[Tmx](https://github.com/baylej/tmx)** by Baylej and it's dependencies. 

You need to add these linker flags : 
```
-Lpath/to/libtmxme.a -Lpath/to/libtmx.a -ltmxme -ltmx -ljansson -lz -lxml2 `sdl2-config --libs` -lSDL2_image
```
