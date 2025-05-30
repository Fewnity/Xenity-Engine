# Game console support

Docker Desktop is needed to compile PSP, PS3 and PsVita games.<br>
The main reason is most SDKs are only working on Linux. Docker is a simple way to install Linux and every SDK and needed tools to compile your game even if your system is running Windows.

Virtualization shoud be enabled in the bios. Click here to see [how to check if virtualization is enabled on my system.](https://stackoverflow.com/questions/49005791/how-to-check-if-intel-virtualization-is-enabled-without-going-to-bios-in-windows)

The SDKs alone take around 8GB, and Docker will also install a Linux image (this won't affect your Windows system). Please make sure you have at least 20-30GB of free disk space available.

If you are already using Docker, Xenity Engine won't affect your existing images/containers.

## Docker Setup

To be able to compile on game consoles you have to:
- Download and install [Docker Desktop](https://www.docker.com/products/docker-desktop/),
- Finish the docker install in Xenity. For this, go to the [Docker Configuration menu](../manual/editor_ui/docker_configuration.md) (`Window -> Docker Configuration`), and follow the instructions.

The process can take several minutes especially if you have a slow computer or a slow internet connection. (The editor will freeze, do not close it).

If anything goes wrong during the installation, check in Docker Desktop if an Image called `xenity_x_x_x` (`x_x_x` is a version).<br>
If there is an image, delete it and retry the installation from the editor.

## Build and run

A build and run option is supported for the PSP to launch the game on an emulator after the build.<br>

You have to:
- Install the [PPSSPP Emulator](https://www.ppsspp.org/).<br>
- Set `PPSSPP location` in the `Engine Settings` menu if your installation location is different.<br>