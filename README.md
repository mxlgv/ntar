# nano tar
It's a simple, small, cross-platform alternative to GNU tar. Using a modified version of the [microtar](https://github.com/rxi/microtar) library.

![image](https://user-images.githubusercontent.com/51446645/180776826-7da98f1e-fd79-41e8-b720-24c0df4e22f1.png)

## Build

You will need [TCC](https://bellard.org/tcc/) and optional [GNU make](https://www.gnu.org/software/make/).

#### With GNU make:
`make`

#### Without GNU make:
`tcc ntar.c microtar.c os.c -o ntar`

## TODO:

- [x] Porting to Windows OS
- [ ] Porting to Kolibri OS
