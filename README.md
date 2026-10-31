# Carmageddon 2, revisited

[![Discord Carmageddon server](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/f5StsuP)

This project attempts to document 1998's Carmageddon 2 by writing equivalent C code.

When enough functions have been replaced, it should become possible to not need the retail binary anymore and let the game run on modern non-Windows systems.

## Status

Status of this fork's `workv3` branch (2026-10-31, `reccmp`, `CARMA2_HW.EXE`):

| Metric | Value |
|---|---|
| Functions implemented | 3815 / 5162 (73.91%) |
| Functions at exactly 100% similar | 2857 |
| Accuracy | 90.33% |
| Progress | 66.76% |

Functions below 100% are not counted as exact. Most best attempts are kept out of the tree until they match; a few runtime-faithful partials (InitPowerups, LoadPowerups, UpdateCollisionObject, LoadPanGameDroneInfo, InitDepthEffects, ReadPowerupSmash, ReadShrapnelSpec, ReadSpillData, InitPolyPedSystem, InitBoner, ReadPedGiblets, InitNapalmNolts, NetPlayerStatusChanged, PlaySmackerFile, FRONTEND_Setup, FRONTEND_GenericMenuHandler, CheckSystemKeys, FRONTEND_DrawMenu, DoMouseCursor, Generic_MenuHandler, Controls_SlotActivated, WrecksUpdateFunc, OrthogonalVector, DRMatrix34RotateCos, PipeSingleSmashRemoveFaces, RepairSmash, AddPolyhedronCollisionShapePlanes, NetSendMessageToAllPlayers and the front-end menu procedures) are kept in for runtime milestones.

## Requirements

- Carmageddon 2: this project needs the original game data function
- MSVC 5 SP3

## Build requirements

- 32-bit x86 compiler:
  - MSVC 11.0 (Visual Studio 97 SP3) compiler for matching build, or
  - a modern MinGW or MSVC toolchain
- DirectX SDK: we need `dinput` and `dxguid`.
  - MinGW provides these libraries out-of-the box
  - MSVC does not ship with a *DirectX SDK*: this [time-period correct version on archive.org](https://archive.org/details/MicrosoftDirectX7SDK) works perfect.
- [libtiff](http://www.libtiff.org/): this library is used to convert tiff images to BRender pixelmaps

## Goals

- Exact same behavior as the original
- Matching binary
- Every single commit should compile and work: this enables `git-bisect` to search for regressions

## Credits

- The [DethRace](https://github.com/dethrace-labs/dethrace/) project: Carmageddon 2 is a continuation of Carmageddon 1.
