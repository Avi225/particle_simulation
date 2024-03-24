<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Avi225/particle_simulation">
    <img src="https://github.com/Avi225/particle_simulation/assets/61160595/9f35a634-993e-45a4-aabc-5e76d5e28e22" alt="Logo" width="120" height="120">
  </a>

<h3 align="center">Particle Simulation</h3>

  <p align="center">
    Simple particle simulation made in C++ with SDL2
    <br>
    <a href="https://github.com/Avi225/particle_simulation/issues">Report Bug</a>
    ·
    <a href="https://github.com/Avi225/particle_simulation/issues">Request Feature</a>
  </p>
</div>

https://github.com/Avi225/particle_simulation/assets/61160595/b71bdaf8-afc3-4e10-9dd8-658903743d84

![](https://github.com/Avi225/particle_simulation/assets/61160595/7cb5c6e1-0f7c-4e8b-9080-6c7dc623160e)

## Instructions
- Use `W`, `S`, `A` and `D` to move around
- Use `Q` and `E` to zoom in and out
- Hold `LSHIFT` to speed up the movement and zooming
- Drag `LMB` to spawn a particle
- Hold `F` to spam particles at cursor
- Press `SPACEBAR` to pause/resume simulation
- Press `F11` to toggle Fullscreen
- While the simulation is paused, press `Z` on a hovered quadrant to show debug properties of particles within



## Building on windows


2. Get SDL2:
    - Download development versions for mingw of [SDL2](https://github.com/libsdl-org/SDL), [SDL2_image](https://github.com/libsdl-org/SDL_image) and [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)
    - Extract and combine them in a memorable place, for example `C:/SDL2-w64`
    
3. Get g++ and mingw:
    - Install [MSYS2](https://www.msys2.org/) following the setup wizard
    - Run `ucrt64.exe` located in the MSYS2 install folder, for example `C:\msys64`
    - Enter `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain` into the opened terminal
    - When prompted to select packages, press `ENTER` to download all
    - Add `C:\msys64\ucrt64\bin` and `C:\msys64\usr\bin` as entries to your `Path` system environmental variable, if not there already (adjust path if needed)
4. Build
    - Clone this repository
    - CD into `particle_simulation`
    - Enter `g++ -c src/*.cpp -std=c++20 -O3 -Wall -I include -I C:/SDL2-w64/include` to build the .o files (adjust SDL2 path if needed)
    - Enter `g++ -static-libgcc -static-libstdc++ *.o -o bin/x64/build/particles -mwindows -s -L C:/SDL2-w64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image` to link (adjust SDL2 path if needed too)
5. Run
    - The executable should be located in `bin/x64/build`, the source code includes this directory and required DLL files to run on any machine

## Support
If you got stuck at any point building the software, encountered a bug or have any other questions feel free to open a Github issue or send me a message.


## Goals for v0.2.0

- ✅️ Better particle-particle collisions
- ⬜️ UI system

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/Avi225/particle_simulation/forks
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/Avi225/particle_simulation/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/Avi225/particle_simulation/issues
[license-shield]: https://img.shields.io/github/license/github_username/repo_name.svg?style=for-the-badge
[license-url]: https://github.com/Avi225/particle_simulation/blob/main/LICENSE
