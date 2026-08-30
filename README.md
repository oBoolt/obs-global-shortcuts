# Global shortcuts for OBS Studio using xdg-desktop-portal
This plugin aims to register OBS hotkeys in the xdg-desktop-portal for global usage mainly in linux using wayland

### Requirements
- OBS Studio 32.2 or later
- An xdg-desktop-portal backend supporting GlobalShortcuts

### Usage
Open OBS Studio, which will automatically register the shortcuts, then use the native way of your desktop environment to display and set the keybinds. 

### Build Requirements
- glib2
- CMake

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

### Guides
<details>
<summary>Hyprland</summary>

To display all the register keybinds type `hyprctl globalshortcuts` in a terminal.

Hyprland provides a dispatcher for DBus Global Shortcuts, in your config file put a line like this:
```lua
hl.bind("<your bind>", hl.dsp.global("com.obsproject.Studio:<registered bind>"))
```

for more information go to [here](https://wiki.hypr.land/0.56.0/Configuring/Basics/Binds/#dbus-global-shortcuts)
</details>

### Roadmap
- [ ] Finish implementing all [static keybinds](#difference-between-static-keybinds-and-dynamic-keybinds)
- [ ] Add [dynamic keybinds](#difference-between-static-keybinds-and-dynamic-keybinds) support
- [ ] Translate to pt-BR

### Other
#### Difference between _Static Keybinds_ and _Dynamic Keybinds_
In this project I am calling "static keybinds" all hotkeys that are default in OBS Studio, meanwhile "dynamic keybinds" are the ones related to
Sources, Outputs and Scenes, which are dynamically created as new sources, outputs and scenes are created.
