# nano tar
It's a simple, small, cross-platform alternative to GNU tar. Using a modified version of the [microtar](https://github.com/rxi/microtar) library.

## Build

You will need [TCC](https://bellard.org/tcc/) and optional [GNU make](https://www.gnu.org/software/make/).

#### With GNU make:
`make`

#### Without GNU make:
`tcc ntar.c microtar.c os.c -o ntar`
