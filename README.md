# nano tar
It's a simple, small, cross-platform alternative to GNU tar. Using a modified version of the [microtar](https://github.com/rxi/microtar) library.

![image](https://user-images.githubusercontent.com/51446645/183033042-2d56fbda-140a-42d4-8e62-2776e10d0f43.png)

## Build
To build under Windows, Unix or KolibriOS you will need [TCC](https://bellard.org/tcc/).
To build under DOS, I used [OpenWatcom](https://github.com/open-watcom/open-watcom-v2)

#### For Unix, Windows, KolibriOS
`tcc ntar.c microtar.c os.c -o ntar`

#### For DOS
`owcc -s -Os -std=c99 -bdos -mcmodel=s ntar.c os.c microtar.c -o ntar.exe`

## TODO:
- [x] Listing Files
- [x] File Extraction;
- [x] Adding files;
- [ ] Recursively adding folders and files;
- [ ] Add saving of all file attributes of supported.
- [x] Porting to Windows OS;
- [x] Porting to Kolibri OS;
- [x] Porting to FreeDOS/MSDOS.

## Warning
The program has not yet been properly tested, so there may be bugs that can damage files. I highly recommend not using it for archiving valuable data yet
