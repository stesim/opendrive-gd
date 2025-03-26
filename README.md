# OpenDRIVE Importer for Godot

(Experimental) Godot plugin for importing OpenDRIVE (.xodr) files as scenes.

> [!NOTE]
> OpenDRIVE files are imported as Godot scenes containing meshes generated from the defined roads.
> The plugin **does not** provide access to semantics like road trajectories, lanes and junctions.

> [!WARNING]
> The plugin is experimental, has a minimal feature set and is available only for Linux systems at this point.


## Build From Source

> [!NOTE]
> This repository uses git submodules, make to sure clone recursively (e.g. `git clone --recursive <url>`).

### Linux

Download or clone the repository to a folder of your choice.
Build and install by running the following commands in that directory.

```sh
cmake --install-prefix <INSTALLATION_DIRECTORY> -B build -DCMAKE_BUILD_TYPE=Release .
cd build
cmake --build . -j8
cmake --install .
```

Here `<INSTALLATION_DIRECTORY>` may be any directory you want to install the plugin to, such as any Godot project folder, for example.
