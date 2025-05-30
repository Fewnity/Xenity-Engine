# Docker configuration

The Docker configuration window shows you the current state of Docker and steps to finish the installation of Docker if Docker is not fully installed.

![image](images/docker_config.jpg)

The Check Docker state button re-check if there is any change about the Docker installation.
Here is the list of different states:
- Docker not installed: Docker is not detected on your system. See [Game console support](../../installation/game_console_installation.md) to install Docker.
- Docker not running: Docker is found on your system but the software is not running. When building a game Docker is automatically started. You can launch yourself Docker too.
- Missing Docker Image: Docker is running but Xenity Engine can't find the Docker Image (The image includes all the needed files and tools to compile games).
- Running: Docker is ready to use, the configuration is done.