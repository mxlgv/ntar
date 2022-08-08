# nano tar
It's a simple, small, cross-platform alternative to GNU tar. Using a modified version of the [microtar](https://github.com/rxi/microtar) library.

![image](https://user-images.githubusercontent.com/51446645/183033042-2d56fbda-140a-42d4-8e62-2776e10d0f43.png)

## Build

You will need [TCC](https://bellard.org/tcc/) and optional [GNU make](https://www.gnu.org/software/make/).

#### With GNU make:
`make`

#### Without GNU make:
`tcc ntar.c microtar.c os.c -o ntar`

## TODO:
- [ ] Recursively adding folders and files;
- [ ] Port for FreeDOS/MSDOS;
- [ ] Autocall KPACK for Kolibri OS;
- [ ] Add saving of all file attributes of supported.

## Warning
The program has not yet been properly tested, so there may be bugs that can damage files. I highly recommend not using it for archiving valuable data yet
