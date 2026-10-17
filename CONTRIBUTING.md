# Contributing to Ocarina of Time PSP

Thanks for wanting to help. You don't need to be a programmer: playing the game on your PSP
and reporting what goes wrong helps just as much.

## Reporting a problem

[Open an issue](https://github.com/JustKidding23/Ocarina-of-Time-PSP/issues/new/choose) and pick
"Something's wrong". It asks for your PSP model and firmware, how you got the game, and what
happened. A photo of the screen helps a lot. Please never attach a ROM, a built game or
anything taken from the game.

For an idea or an improvement, pick "Idea or improvement" instead.

## What would help

- **Testing** on other PSP models (1000, 2000, Go, Street) and other custom firmware.
- **Other ROM versions.** The release only includes the NTSC 1.1 (US) game. The build can
  make the others, but they haven't been tested on a PSP.
- **Graphics or sound that differ from the N64 version**, with a screenshot and where in the
  game it happens.

## Changing the code

1. Fork the repository and make a branch from `psp3001`.
2. Make your change, and test it (see below).
3. Open a pull request into `psp3001` that says what it changes, why, and how you tested
   it.

The owner of this repository reviews each pull request and tries it on a PSP when possible,
so it can take a little while.

### Building

You need Linux (or WSL) and the [pspdev](https://github.com/pspdev/pspdev) toolchain.
`./psp.sh PSP_PORT_VERSIONS=ntsc-1.1` builds the NTSC 1.1 game, and the README says what to
copy to the memory stick. [docs/psp-first-boot-assets.md](docs/psp-first-boot-assets.md) has
upstream's build instructions.

### Testing

- **On a PSP.** Please try your change on a real PSP if you can, and say in the pull request
  what you tested. PPSSPP is a good first check, but it doesn't behave exactly like a PSP,
  and it can't run the Media Engine audio. [PSPLink](https://github.com/pspdev/psplinkusb)
  can start a build over USB and show crash details.

## Ground rules

- **No game files.** Don't commit or attach ROMs, built games (`EBOOT.PBP`), saves, or
  graphics, sound or text taken from the game.
- **Say how you tested it.** "Played to the first gym on a PSP 3001" is perfect. "Only built
  it" is fine too, as long as it says so.
- **Keep changes focused.** One fix or feature per pull request is much easier to review.

### Using AI

This project was made with Claude, Anthropic's AI assistant, and it says so openly. Changes
made with AI tools are welcome on the same terms: say in the pull request that you used one
(and which), and make sure you've read and tested what it wrote. You're responsible for your
pull request either way.

## Upstream

Fixes to the decompilation itself belong in [zeldaret/oot](https://github.com/zeldaret/oot),
and fixes to the PSP port that aren't specific to this fork can also go to
[z2442/oot-PSP](https://github.com/z2442/oot-PSP), so that everyone gets them.
