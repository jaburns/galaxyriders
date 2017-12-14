### Galaxy Riders

#### Win64 setup

- From a shell with git available, in the project directory, run: ``git submodule update --init --recursive``
- From PowerShell, run: ``setup_win64.ps1``
- Open ``GalaxyRiders.sln`` in Visual Studio.
- Right click the *GalaxyRiders_tools* project in Solution Explorer, and select *Debug -> Start new instance*.
- Once the tools finish, press F5 to build and run the game.

#### OSX and Linux setup

```
brew install sdl2 #or sudo apt install libsdl2-dev libglew-dev #or w/e
git clone git@github.com:jaburns/galaxyriders
cd galaxyriders
git submodule update --init --recursive
tools/build run
./configure
make run
```
