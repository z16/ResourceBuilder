# ResourceBuilder

Parses Windower resource files (as output by [ResourceExtractor](https://github.com/Windower/ResourceExtractor/)) and creates FFXI DAT files based on those. Will only write DAT content which is provided in the parsed resources, will not remove existing data otherwise. Supports adjusting existing data as well as adding new data.

### Usage

Required parameters:
1. Directory of the modified resource files to use as a template
2. Directory of the FFXI installation to read DAT files from

Optional flags:
* `--validate` Enables basic validation of some of the assumptions used for its algorithms, otherwise it will proceed with no assumptions, which is riskier but faster.
* '--decode' Will generate decoded DAT files in addition to the proper files for the game (in its own folder `DEC/`)

The resulting DAT files will be written to the resources directory, so they will not automatically take effect in-game. Back up existing FFXI DATs first, then copy the new ones to the game directory. The decoded files will be here as well.

### Not yet implemented

Currently only supports item resources, all other resources cannot be modified.

Does not yet support Japanese DAT files.

