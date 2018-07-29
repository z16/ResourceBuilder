# ResourceBuilder

Parses Windower resource files (as output by [ResourceExtractor](https://github.com/Windower/ResourceExtractor/)) and creates FFXI DAT files based on those. Will only write DAT content which is provided in the parsed resources, will not remove existing data otherwise. Supports adjusting existing data as well as adding new data.

### Usage

Invoke the executable with two parameters, first the directory of the modified resource files, second the directory to the FFXI install.

The resulting DAT files will be written to the resources directory, so they will not automatically take effect in-game. Back up existing FFXI DATs first, then copy the new ones to the game directory.

### Not yet implemented

Currently only supports item resources, all other resources cannot be modified.

Does not yet support Japanese DAT files.

