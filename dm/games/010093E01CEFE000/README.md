# GUNDAM BREAKER 4 (`010093E01CEFE000`)

**Draft.** Brought up on the Android port (nx2android) only: the Mac bring-up and the profile's shader tables are still to do, so this is not ready to merge (see Status).

Update 1.12.0 (v720896) over the base game, SDK 12.3.2 (its own `sdk` build, the same as OCTOPATH TRAVELER II's). Bandai Namco, Unreal Engine 4.27: one version 11 pak
(`GB4/Content/Paks/pakchunk0-Switch.pak`, 6.6 GB) whose shaders are inline in the cooked packages, NVN, CRIWARE sound. Modules `main`, `subsdk0` (the movie module) and `sdk`. 60 fps
(30 on the Switch) by the host's writes of Unreal's frame limits.

Status: brought up on the **Android port (nx2android)**: the boot, the language selection and the opening at 60 fps (frame time p50 16.66-16.67 ms, GPU about 2 ms, footprint about
1.2 GB, CRIWARE sound, no audio underrun after the first sound) on an AYN Thor (Adreno 740, Android 13). The Mac bring-up and the shader tables (`shader_map.tsv`, `shader_facts.tsv`, `shader_pairs.tsv`,
generated with `dmsh_build.py` on a Mac) are still to do: this entry carries the profile and its reasoning only.

## What it needs

| Setting | Where | Why |
|---|---|---|
| `fps_target 60` | [`game.tsv`](game.tsv) | the game's config caps Unreal at 30 (`t.MaxFPS`, `rhi.SyncInterval=2`) and its engine runs with `bUseFixedFrameRate` on: `ue_fixed_rate.py` finds `GEngine` and the property/flag, and the host writes 60 into it, `t.MaxFPS` and 1 into `rhi.SyncInterval` (`ue_cvar.py`'s fixups); see [Frame rate](#frame-rate) |
| FMallocBinned3 constructor call cut, `main+0x1a52c28` `bl 0x1ab1f48` → `nop`, with gate `OBS_UE_MALLOC` on it | the rewriter's engine-wide rule (`dm/tools/ue_malloc.py`), no build ID | its pools are cut out of the engine region (37 GB); the host runs Unreal's heap on a malloc zone (`hle_uemalloc.c` layout UE4_27) |
| Engine region given what the alias region has room for: gate `OBS_UE_REGION` on the store of its size, `main+0x1a525d0` | the rewriter's engine-wide rule (`dm/tools/ue_region.py`), no build ID | Unreal's `nninitStartup` reserves 62 GB by default |
| Node area cut 1 GB → 62 MB and regions of 4 MB and more aligned to 2 MB | `dm/tools/sdk_patches.py`, the SDK-wide rules on `sdk` build `5bcf525f...` | the AddressRegionManager constructor; the build is OCTOPATH TRAVELER II's, so its rules already cover this game |
| 3.75 GB guest block: `mem_reserve 0xF0000000`, `mem_alias 0xC8000000`, `mem_stack 0x10000000`, `app_pool 0xCD500000` | [`game.tsv`](game.tsv) | as OCTOPATH TRAVELER II: alias region = node area + the engine region + 256 MB for regions made later |

## Frame rate

The Switch build sets `t.MaxFPS=30` and `rhi.syncinterval=2` in its config and turns Unreal's fixed frame rate on, so both the engine's tick and its presents stay at 30. The rewriter
finds the two console variables by their registered names and `UEngine::FixedFrameRate` through its reflection entry (`dm/tools/ue_cvar.py`, `ue_fixed_rate.py`), and the host writes the
profile's `fps_target` into them (`hle_fpslock.c`). On the Android port this gave 60 fps: frame time p50 16.66-16.67 ms, p95 17.2 ms, 0 dropped frames in the 600-900 frame window.

## Shader split

No shader code archive: each material's shaders are inline in its `.uexp` and the engine's global shaders are in `Engine/GlobalShaderCache-SF_GLSL_SWITCH.bin`, every one an LZ4 block
(`shader_split.py` `ue_inline_shaders`). The pak's blocks are Oodle-compressed, so the host decodes them with the engine's own decoder (`uepak.Pak.read`'s decode, `DM_OODLE_JOB`) and
the split reads them file by file (`ue_legacy_jobs`). The Android port decoded 120,620 pak blocks (14.86 GB) on the device and found 15,950 shaders; all of them translated to SPIR-V.
