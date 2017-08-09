### Galaxy Riders

#### Win64 setup

From a WSL bash shell:
```
sudo apt-get -y install wget unzip g++
git clone git@github.com:jaburns/galaxyriders
cd galaxyriders
./setup_win64
```

Now open ``GalaxyRiders.sln`` in Visual Studio and the project should build and run normally.

#### OSX setup

```
brew install sdl2
git clone git@github.com:jaburns/galaxyriders
cd galaxyriders
git submodule update --init --recursive
pushd tools
./build
./compute_sdfs
./generate_wire_meshes
popd
./configure_osx
make run
```
