f32disk
========

A FAT32 disk emulation test task with DOS commands support

## Build & Run

```bash
$ git clone https://github.com/nevmnd/fat32_emulator.git
$ mkdir build
$ make all
$ cd build
$ sudo ./f32disk
```
## Usage

Program get as a parameter path to FAT32 disk. If disk does not exist, new one of size 20 MB will be created.
List of supported commands shown at program start. Root permissions are required for normal work.

Example:
```
$ sudo ./f32disk test.disk
```
