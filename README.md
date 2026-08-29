# Global shortcuts for OBS Studio using xdg-desktop-portal

### Requirements
- OBS Studio 32.2 or later
- glib2
- xdg-desktop-portal
- CMake (building)

### Installation
#### Building (Global Install)
```sh
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
$ sudo cmake --install build
```

#### Building (Local Install)
```sh
$ cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="~/.config/obs-studio/plugins"
$ cmake --build build
$ cmake --install build
```

