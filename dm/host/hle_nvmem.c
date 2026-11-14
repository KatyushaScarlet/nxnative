// The NVN internals a game calls beside the public API. Rabi-Ribi (SDK 7) calls nv::GetGraphicsMemoryInfo, which reads the NVN driver's interface object (sdk+0xcc7ca0 in its
// sdk): the host replaces the NVN API, so that object is never built and the call faults on the null pointer right after the swap chain is created. The host has no separate
// video memory to report, so the 16-byte MemoryInfo comes back zeroed, as a driver with no memory of its own would say.
#include "hle.h"
#include <string.h>
HLE_FORCE("_ZN2nv21GetGraphicsMemoryInfoEPNS_10MemoryInfoE", "graphics memory info (an internal NVN helper that reads the NVN driver's object): the host has no separate video memory", void, h_nv_GfxMem, void *info) {
    if (info) memset(info, 0, 16);
}
