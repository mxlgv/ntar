# nano tar
It's a simple, small, cross-platform alternative to GNU tar. Using a modified version of the [microtar](https://github.com/rxi/microtar) library.

![image](https://user-images.githubusercontent.com/51446645/180773452-6c01b892-3648-4022-bc6a-742933fc93b5.png)

## Build

You will need [TCC](https://bellard.org/tcc/) and optional [GNU make](https://www.gnu.org/software/make/).

#### With GNU make:
`make`

#### Without GNU make:
`tcc ntar.c microtar.c os.c -o ntar`




