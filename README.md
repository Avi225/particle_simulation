
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
    Including quad-tree optimalization and realistic energy transfer simulation
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
- Drag `LMB` to place an accelerated particle
- Hold `F` to spawn particles at cursor
- Use `SCROLLWHEEL` to increase/decrease the amount of spawned particles
- Press `SPACEBAR` to pause/resume simulation
- Press `F11` to toggle Fullscreen
- While the simulation is paused, press `Z` to show debug properties of particles within the hovered quadrant



## Building
### On Windows
 1. Prerequisites
	 - [Git](https://git-scm.com/downloads/win)
	 - [CMake](https://cmake.org/download/)
	 - [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) (Desktop development with C++)
	 - [NASM](https://www.nasm.us/pub/nasm/stable/)
	 - [Perl](https://strawberryperl.com/)
	 - Make sure Git, CMake, NASM and Perl are included in your system's `PATH` environmental variable
 3. Clone this repository 
	 - `git clone https://github.com/Avi225/particle_simulation`
 4. Enter the cloned repository
	 - `cd particle_simulation`
 5. Create a build folder
	 - `mkdir build`
 6. Enter the build folder
	-  `cd build`
 7. Run CMake configuration
	 - `cmake -G "Visual Studio 17 2022" ..`
 8. Build the project
	 - `cmake --build . --config Release`
 9. The executable is located in `build/Release`
 10. Copy the `res` directory into the executable directory
 11. You may need to copy the required SDL3 DLLs into the executable directory, these should be located at:
	 - `SDL3.dll` - `build/_deps/sdl3-build/Release`
	 - `SDL3_ttf.dll` - `build/_deps/sdl_ttf-build/Release`
	 - `SDL3_image.dll` - `build/_deps/sdl_image-build/Release`

## Support
If you got stuck at any point building the software, encountered a bug or have any other questions feel free to open a Github issue or send me a message.


## Goals for v0.3.0

- Unified logging ✓
- Better comments
- New control scheme

## Goals for future releases

- Expanded GUI system
- Obstacle implementation
- Optimalization

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
