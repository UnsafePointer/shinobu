# shinobu (しのぶ)

> "Well, I'm sorry. You haven't even realized that people don't like you, then?" - Kochō Shinobu
>
> [Demon Slayer: Kimetsu no Yaiba, by Koyoharu Gotōge](https://en.wikipedia.org/wiki/Demon_Slayer:_Kimetsu_no_Yaiba)

shinobu (しのぶ) is a cycle accurate [Game Boy](https://en.wikipedia.org/wiki/Game_Boy) (1989) and [Game Boy Color](https://en.wikipedia.org/wiki/Game_Boy_Color) (1998) debugger, disassembler and emulator written in modern C++.

|Pokémon Gold Version - Game Boy Color (1999) | Metroid II: Return of Samus - Game Boy (1992)|
|---------------------------------------------|----------------------------------------------|
|![gold.gif](/images/gold.gif)                |![metroid.gif](/images/metroid.gif)           |

## Building

This project can be built in Linux or Windows (with MSYS2) using GCC 9.3.0 (C++17) and SDL2.

```Shell
$ mkdir build
$ cd build
$ cmake .. # On Windows with MSYS2: cmake -G 'MSYS Makefiles' ..
$ make -j8
```

## Usage

```Shell
$ shinobu -h
Usage: shinobu [-s] [-d] [-h] filepath

  -s   skip BOOT ROM, only supported by DMG emulation
  -d   disassemble, a `filepath.s` file will be created
  -h   print this message
```

A bootstrap ROM can be optionally (**required** for CGB emulation) placed in the current path:

* `DMG_ROM.BIN` (SHA1: 4ed31ec6b0b175bb109c0eb5fd3d193da823339f)
* `CGB_ROM.BIN` (SHA1: 1293d68bf9643bc4f36954c1e80e38f39864528d)

A `shinobu.yaml` file will be generated to further configure the emulator options:

```Yaml
audio:
  mute: false
emulation:
  CGBBootstrapROM: CGB_ROM.BIN # Relative path to CGB bootstrap ROM file, required
  DMGBootstrapROM: DMG_ROM.BIN # Relative path to DMG bootstrap ROM file, optional
  overrideCGB: false # Use DMG emulation whenever possible
  windowLineCounter: true # See know issues section for a detailed explanation of this
frontend:
  kind: SDL # Default: SDL, Available: Perf (shows performance metrics), PPU (show VRAM state)
log: # Logging configuration (log levels: NOLOG, MSG, WAR)
  CPU: NOLOG
  DMA: NOLOG
  PPU: NOLOG
  ROM: NOLOG
  disassembler: NOLOG
  interrupt: NOLOG
  joypad: NOLOG
  memory: NOLOG
  openGL: NOLOG
  serial: NOLOG
  sound: NOLOG
  timer: NOLOG
video:
  fullscreen: false # Desktop fullscreen startup
  overlayScale: 1 # Increase this if the frontend overlay is too small
  palette: 0 # Select a custom palette for DMG emulation (10 available)
```

### Keybindings

![keyboard.png](/images/keyboard.png)

Gamepad support is implemented by [`SDL_GameController` API](https://wiki.libsdl.org/CategoryGameController) (Xbox-type controllers). Only the Game Boy buttons and DMG palette selection are mapped.

### PPU visualizer

![ppu.gif](/images/ppu.gif)

## Known issues

* Scanline rendering: the PPU emulation is driven by a scanline renderer, LCD timing effects are not working and games like Prehistorik Man won't run correctly.
* Cartridge supported: MBC1 (RAM+BATTERY), MBC3 (RAM+BATTERY+TIMER), MBC5 (RAM+BATTERY), everything else won't run at all.
* Game Boy Color emulation **requires** a bootstrap ROM.
* The OpenGL renderer can be slow in some integrated graphic cards.

## Acknowledgments

This emulator, like many others, has it's implementation based on the [Pan Docs Game Boy technical reference](https://gbdev.io/pandocs/.), a compilation of the work of [many people](https://gbdev.io/pandocs/#references). I also got **a lot** of questions answered by the emulator development community at [emudev.org](https://emudev.org/).
