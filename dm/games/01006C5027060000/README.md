# Magical Girl Witch Trials (`01006C5027060000`)

Update 2.0.1b (v65536) over the base game, SDK 21.4.7. Unity 6000.0.72f1 IL2CPP with FMOD on NVN. Modules `main`, `subsdk0` (the SDK's movie library,
NintendoSDK_movie) and `sdk`. Nearly everything is in 644 Addressables bundles under `Data/StreamingAssets/aa/Switch` (2.62 GB of the 2.65 GB RomFS), which carry
their type trees: `unityfs.py` reads Unity 6's `Shader` objects from them, and `shader_split.py` splits 2,421 shaders.

## What it needs

| Setting | Where | Why |
|---|---|---|
| 5 GB guest layout: `mem_reserve 0x140000000`, `mem_alias 0x118000000`, `mem_stack 0x10000000`, `app_pool 0xCD500000`, `sys_resource 0x1000000` | [`game.tsv`](game.tsv) | the NPDM's 16 MB of system resource (virtual address memory) and no heap: Unity 6's `nninitStartup` sets no StandardAllocator and takes its memory in address regions of its own; see [Memory](#memory) |
| `shader_groups /Data/=data` | [`game.tsv`](game.tsv) | everything is under `Data/`: the managed metadata, the resources and the `StreamingAssets` bundles |
| `pad_layout full` | [`game.tsv`](game.tsv) | a visual novel played with a controller |

## Memory

- The NPDM declares 16 MB of system resource (virtual address memory) and the game never calls `SetMemoryHeapSize`, so it has no heap. Unity 6's `nninitStartup` sets no StandardAllocator: Unity reserves address regions with
  `nn::os::AllocateAddressRegion` and commits pages with `AllocateMemoryPages` in the alias region.
- The profile gives the game the block of the other Unity games with virtual address memory (256 MB in front + 4.375 GB alias region + 256 MB stack region + 128 MB heap region),
  with the hardware pool as on the Switch: Unity only reads it through `QueryMemoryInfo`, and nothing maps against it.

## Shaders

- Nearly everything is in 644 Addressables bundles (`StreamingAssets/aa/Switch`). The bundles carry their type trees (they report the engine version), so `unityfs.py` reads
  Unity 6's `Shader` objects with no layout written out; `shader_split.py` splits 2,421 shaders from them and writes the profile's tables (`shader_map.tsv`,
  `shader_facts.tsv`, `shader_pairs.tsv`).
