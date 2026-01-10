# Blightspire Engine

This is a fork of the engine source for [Blightspire](https://store.steampowered.com/app/3365920/Blightspire/).

The game was originally made as student project for Y3 at BUAS. 

The assets for the game are not provided as they fall under intellectual property of the university (probably, I'm not really sure xD).

## The plan

There is no plan for this fork as of now. It is simply a playing ground to improve and review the work that was made. 

The contribution pipeline is to just make pull requests and please don't push to main directly.

**To the original members of the team**: we can always arrange a better project structure and pull request flow if people are really interested, just shoot me a message on Discord. Otherwise, it's just me (Santi) on this.

## Build instructions

THINGS THAT YOU SHOULD PROBABLY DO:
- Update LLVM and CMake: Clang introduces a lot of new C++ warnings that are flagged as unknown when compiling with an older version and for now the project makes use of WHOLE_ARCHIVE flags for proper unit test registration, which only configure correctly on newer versions of CMake
- Make sure your vulkan SDK version is updated (atleast 1.4.335.0) and include vk_mem_alloc header when installing the SDK (avoids having to pull another dependency ourselves)

Refer back to the wiki in the original Blightspire project for all the previous setup and configuration needed.
If anything doesn't work, I probably changed it. Make an issue for it if that happens :)