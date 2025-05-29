# Game console support

## Docker Setup
Docker Desktop is needed to compile PSP, PS3 and PsVita games.<br>

You have to:
- Download and install [Docker Desktop](https://www.docker.com/products/docker-desktop/),
- Finish the docker install in Xenity. For this go in the Docker Configuration menu (`Window -> Docker Configuration`), and follow the instructions.

The process can take several minutes especially if you have a slow computer or a slow internet connection. (The editor will freeze, do not close it).

If anything goes wrong during the installation, check in Docker Desktop if an Image called `xenity_x_x_x` (`x_x_x` is the engine version).<br>
If there is any, delete it and retry the installation from the editor.

## Build and run

A build and run option is supported for the PSP to launch the game on an emulator after the build.<br>

You have to:
- Install the [PPSSPP Emulator](https://www.ppsspp.org/).<br>
- Set `PPSSPP location` in the `Engine Settings` menu if your installation location is different.<br>.