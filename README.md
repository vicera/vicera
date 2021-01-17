![Vicera Logo](https://github.com/h34ting4ppliance/vicera/raw/master/vicera.png)

**VICERA** is a fantasy console written in C heavily inspired by the Gameboy. 
It features it's very own CPU architecture, Tiles and Sprites, the same 
controller as the Gameboy and a Monochrome 160x160 display.

The SPU hasn't even been planned yet so stay tuned. I'll tell more here if ever
I start implementing the SPU.

## Blog articles

Here are some blog articles I have wrote about the VICERA!

 - [End of the VICERA :pensive:](https://cutebunni.es/b/end_of_vicera.html)
 - [New Project: The VICERA Fantasy Console!](https://cutebunni.es/b/vicera_part1.html)

## Checklist of the current development

Here is the checklist :

 - [X] Implement CPU
 - [X] Implement GPU
 - [X] Implement Controller emulation
 - [X] Write an SDL front-end
 - [X] Command-line arguments
 - [X] Configuration file
 - [ ] SPU
 - [X] Write an assembler (Not completely)
 
The documentation will start being written when the checklist will be completed.  
If you want a more detailed TODO list, refer to [this file](todoo.txt)

## Building

The VICERA is written in C with only standard libraries and the SDL library.
So the only dependency you need is SDL2. Either build it yourself or install
it using your distribution's package manager.

This is how you install SDL2 using `pacman` on Arch Linux: 

    $ sudo pacman -S sdl2

To build it, this is extremely simple.

    $ make
    $ ./vicera <ROM File>
    
A more detailed building guide will come later when the project will come at a
stable state.

## Contributing

All pull requests and issues in the project are welcome! So if you want to
contribute, just make an issue or fork the code then PR it.

## Licensing

This project is licensed under the [MIT License](blob/master/LICENSE).
