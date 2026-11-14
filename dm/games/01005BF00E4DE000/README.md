# Rabi-Ribi (01005BF00E4DE000)

Update 1.2.0 (v131072) over the base game, SDK 7.4.0, NVN. CreSpirit's own 2D engine; modules main, subsdk0 (GLSLC, NVN's runtime shader compiler), subsdk1 (the movie library,
NintendoSDK_movie) and sdk. RomFS holds the art (PNG and Ogg) under pack/, the area maps and fonts under data/ and the translations under localize/. No game file holds the
shaders: main keeps 5 GLSL sources and calls glslcCompile in subsdk0, so the game compiles them with its own GLSLC at run time, as Three Houses' built-in shaders are.

## What it needs

| Setting | Where | Why |
|---|---|---|
| `shader_groups … /glslc_capture/=runtime` | [`game.tsv`](game.tsv) | the game compiles its shaders at run time (see [Shaders](#shaders)) |
| `nv::GetGraphicsMemoryInfo` | `dm/host/hle_nvmem.c` | SDK 7's main calls the NVN internal helper, which reads the NVN driver's interface object; the host replaces the NVN API, so that object is null and the call faults right after the swap chain is created. The host has no separate video memory, so the 16-byte MemoryInfo comes back zeroed |
| descriptor pool lifetime | `dm/gfx/nvn/hle_nvn.c` | the game makes two or three texture and sampler pools a frame and finalizes them; a table that never forgot a pool fills, and later screens read other pools' IDs. A finalized pool, or one whose NVNmemoryPool goes, hands its slot and base range back for the next pool no larger; the table holds 1024 live pools, and a dead slot is reused after 8 more submissions so that a queued replay cannot name a reused pool |
| masked offset jump table | `dm/tools/patch_nso.py` | `main+0x446f0c` is an offset jump table whose bounds check masks the index and tests the mask (and, mvn, tst) instead of comparing it; `jump_mask_offsets` reads the mask and the table's mask+1 targets |
| `pad_layout full` | [`game.tsv`](game.tsv) | a 2D platformer played with a controller; no touch or pointer input |

## Memory

The NPDM declares no system resource (META +0x14 = 0): the game uses SetHeapSize, and main sets a heap of at most the pool − 64 MB, a 3328 MB heap region.

## Shaders

main holds 5 GLSL sources and subsdk0 is NVIDIA's GLSLC: the game compiles the programs it needs at run time, and no file in the RomFS holds a shader. The NSP script boots
the game in the Mac host with `DM_GLSLC_CAPTURE=<version dir>/glslc_capture` (`hle_glslc.c`) and `shader_split.py` reads the outputs like any other GLSLC shader; the capture
holds 24 of them.
