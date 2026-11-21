/*
 * 3DLabsPermedia2.chip - Picasso96 chip driver for 3DLabs Permedia2 / TI TVP4020
 *
 * Reconstructed in C from the 7.16 (gcc) and 7.17 (vbcc) binaries.
 * Behaviour follows 7.17 unless a comment says otherwise.
 */
#ifndef P2CHIP_H
#define P2CHIP_H

#pragma pack(2)
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <exec/semaphores.h>
#include <exec/memory.h>
#pragma pack()

#pragma pack(2)
#include "boardinfo.h"
#pragma pack()

/* the chip functions call exec through bi->ExecBase, not a global SysBase */
#define __NOLIBBASE__
#define EXEC_BASE_NAME (bi->ExecBase)
#pragma pack(2)
#include <proto/exec.h>
#pragma pack()

/*--------------------------------------------------------------------------*/
/* Permedia2 registers. bi->RegisterBase points 0x8000 bytes into the       */
/* control register window (so the graphics core registers start at 0).     */
/* Offsets below are relative to the start of the window.                   */
/*--------------------------------------------------------------------------*/

#define REG(off)        (*(volatile ULONG *)(RegBase + ((off) - 0x8000)))

/* control / status */
#define ResetStatus             0x0000
#define IntEnable               0x0008
#define IntFlags                0x0010
#define InFIFOSpace             0x0018
#define ApertureOne             0x0050
#define ApertureTwo             0x0058
#define FIFODiscon              0x0068  /* bit 31 = graphics processor busy */
#define ChipConfig              0x0070
#define Reboot                  0x1000
#define MemControl              0x1040
#define BootAddress             0x1080
#define MemConfig               0x10c0
#define BypassWriteMask         0x1100
#define FramebufferWriteMask    0x1140

/* video timing */
#define ScreenBase              0x3000
#define ScreenStride            0x3008
#define HTotal                  0x3010
#define HgEnd                   0x3018
#define HbEnd                   0x3020
#define HsStart                 0x3028
#define HsEnd                   0x3030
#define VTotal                  0x3038
#define VbEnd                   0x3040
#define VsStart                 0x3048
#define VsEnd                   0x3050
#define VideoControl            0x3058
#define LineCount               0x3070

/* RAMDAC (TVP4020 compatible) */
#define RDPaletteWriteAddress   0x4000
#define RDPaletteData           0x4008
#define RDPixelMask             0x4010
#define RDCursorColorAddress    0x4020
#define RDCursorColorData       0x4028
#define RDIndexedData           0x4050
#define RDCursorData            0x4058
#define RDCursorXLow            0x4060
#define RDCursorXHigh           0x4068
#define RDCursorYLow            0x4070
#define RDCursorYHigh           0x4078

/* RAMDAC indexed registers */
#define RDI_CursorControl       0x06
#define RDI_ColorMode           0x18
#define RDI_ModeControl         0x19
#define RDI_MiscControl         0x1e
#define RDI_PixelClockA1        0x20
#define RDI_PixelClockA2        0x21
#define RDI_PixelClockA3        0x22
#define RDI_PixelClockStatus    0x29
#define RDI_MemoryClock1        0x30
#define RDI_MemoryClock2        0x31
#define RDI_MemoryClock3        0x32
#define RDI_MemoryClockStatus   0x33
#define RDI_ColorKeyControl     0x40

/* graphics core (offset 0x8000 + tag * 8) */
#define StartXDom               0x8000
#define dXDomReg                 0x8008
#define StartXSub               0x8010
#define dXSubReg                 0x8018
#define StartY                  0x8020
#define dYReg                  0x8028
#define GLINTCount              0x8030
#define Render                  0x8038
#define ContinueNewLine         0x8040
#define BitMaskPattern          0x8068
#define RasterizerMode          0x80a0
#define ScreenSize              0x8198
#define RectangleOrigin         0x80d0
#define RectangleSize           0x80d8
#define PackedDataLimits        0x8150
#define ScissorMode             0x8180
#define ScissorMinXY            0x8188
#define ScissorMaxXY            0x8190
#define AreaStippleMode         0x81a0
#define WindowOrigin            0x81c8
#define AreaStipplePattern(n)   (0x8200 + (n) * 8)
#define TextureAddressMode      0x8380
#define TextureMapFormat        0x8588
#define Texel0                  0x8600
#define TextureColorMode        0x8680
#define TextureReadMode         0x8670
#define TexelLUTMode            0x8678  /* 7.17 rebuild called this TextureDataFormat */
#define FogMode                 0x8690  /* 7.17 rebuild called this TexelLUTMode */
#define TextureDataFormat       0x8590
#define ColorDDAMode            0x87e0
#define ConstantColor           0x87e8
#define AlphaBlendMode          0x8810
#define DitherMode              0x8818
#define FBSoftwareWriteMask     0x8820
#define LogicalOpMode           0x8828
#define LBReadMode              0x8880
#define LBReadFormat            0x8888
#define LBSourceOffset          0x8890
#define LBWindowBase            0x88b8
#define LBWriteMode             0x88c0
#define LBWriteFormat           0x88c8
#define Window                  0x8980
#define StencilMode             0x8988
#define DepthMode               0x89a0
#define FBReadMode              0x8a80
#define FBSourceOffset          0x8a88
#define FBPixelOffset           0x8a90
#define FBSourceData            0x8aa8
#define FBWindowBase            0x8ab0
#define FBWriteMode             0x8ab8
#define FBHardwareWriteMask     0x8ac0
#define FBBlockColor            0x8ac8
#define FBReadPixel             0x8ad0
#define FilterMode              0x8c00
#define StatisticMode           0x8c08
#define FBSourceDelta           0x8d88
#define YUVMode                 0x8f00
#define DeltaMode               0x9300

/*--------------------------------------------------------------------------*/

/* "nop" makes the 68040/060 finish all pending bus writes */
#define Serialize()             __asm__ __volatile__("nop")

#define WAITFIFO(n)             while (REG(InFIFOSpace) < (ULONG)(n)) Serialize()
#define WAITIDLE()              while (REG(FIFODiscon) & 0x80000000) Serialize()

#define CurrentVideoControl     ChipData[0]
#define SpriteActive            ChipData[1]
#define EngineLock              ChipData[2]	/* struct SignalSemaphore * (7.18) */
#define EngineDirty             ChipData[3]	/* ULONG * "3D used the chip" flag, or 0 (7.18) */

/* public semaphore shared with Warp3D (7.18) */
#define P2_ENGINE_LOCK          "3DLabsPermedia2.chip engine"

static inline ULONG MakeLong(LONG hi, LONG lo)
{
	return ((ULONG)hi << 16) | (UWORD)lo;
}

static inline ULONG Replicate8(UBYTE b)
{
	return ((ULONG)b << 24) | ((ULONG)b << 16) | ((ULONG)b << 8) | b;
}

/* tables (chip.c / blitter.c) */
extern const UWORD PixelWidth[22];
extern const UBYTE DACValues[24];
extern const UBYTE DstFormat[24];
extern const UBYTE TextureFormat[24];
extern const UBYTE DepthMask[12];
extern const UBYTE ReadPixelValues[24];
extern const UBYTE MinTerms[16];
extern const UWORD EnginePartProd[65];

/* internal functions */
UWORD EngineWidth(UWORD width);
ULONG SwapBytes(ULONG x);
void setupBlitter(struct BoardInfo *bi);
void setupClocks(struct BoardInfo *bi);
void setupSprite(struct BoardInfo *bi);
void setPixelClock(struct BoardInfo *bi, UBYTE num, UBYTE den);
void setMemoryClock(struct BoardInfo *bi, ULONG clock);

#endif
