/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef VC4_DEFINES_H_INCLUDED
#define VC4_DEFINES_H_INCLUDED
#include <cstddef>
#include <stdint.h>
//#include <mach/platform.h>
#define VC4_BASE 0x3fc00000

#define VC4_READ(base, a) (base[(a)/4])
#define VC4_WRITE(base, a, v) (base[(a)/4])=v

#define V3D_IDENT0 0x00000
#define V3D_IDENT1 0x00004
#define V3D_IDENT2 0x00008
#define V3D_SCRATCH 0x00010
#define V3D_L2CACTL 0x00020
#define V3D_SLCACTL 0x00024
#define V3D_INTCTL 0x00030
#define V3D_INTENA 0x00034
#define V3D_INTDIS 0x00038
#define V3D_CT0CS 0x00100
#define V3D_CT1CS 0x00104
#define V3D_CT0EA 0x00108
#define V3D_CT1EA 0x0010c
#define V3D_CT0CA 0x00110
#define V3D_CT1CA 0x00114
#define V3D_CT00RA0 0x00118
#define V3D_CT01RA0 0x0011c
#define V3D_CT0LC 0x00120
#define V3D_CT1LC 0x00124
#define V3D_CT0PC 0x00128
#define V3D_CT1PC 0x0012c
#define V3D_PCS 0x00130
#define V3D_BFC 0x00134
#define V3D_RFC 0x00138
#define V3D_BPCA 0x00300
#define V3D_BPCS 0x00304
#define V3D_BPOA 0x00308
#define V3D_BPOS 0x0030c
#define V3D_BXCF 0x00310
#define V3D_SQRSV0 0x00410
#define V3D_SQRSV1 0x00414
#define V3D_SQCNTL 0x00418
#define V3D_SRQPC 0x00430
#define V3D_SRQUA 0x00434
#define V3D_SRQUL 0x00438
#define V3D_SRQCS 0x0043c
#define V3D_VPACNTL 0x00500
#define V3D_VPMBASE 0x00504
#define V3D_PCTRC 0x00670
#define V3D_PCTRE 0x00674
#define V3D_PCTR0 0x00680
#define V3D_PCTRS0 0x00684
#define V3D_PCTR1 0x00688
#define V3D_PCTRS1 0x0068c
#define V3D_PCTR2 0x00690
#define V3D_PCTRS2 0x00694
#define V3D_PCTR3 0x00698
#define V3D_PCTRS3 0x0069c
#define V3D_PCTR4 0x006a0
#define V3D_PCTRS4 0x006a4
#define V3D_PCTR5 0x006a8
#define V3D_PCTRS5 0x006ac
#define V3D_PCTR6 0x006b0
#define V3D_PCTRS6 0x006b4
#define V3D_PCTR7 0x006b8
#define V3D_PCTRS7 0x006bc
#define V3D_PCTR8 0x006c0
#define V3D_PCTRS8 0x006c4
#define V3D_PCTR9 0x006c8
#define V3D_PCTRS9 0x006cc
#define V3D_PCTR10 0x006d0
#define V3D_PCTRS10 0x006d4
#define V3D_PCTR11 0x006d8
#define V3D_PCTRS11 0x006dc
#define V3D_PCTR12 0x006e0
#define V3D_PCTRS12 0x006e4
#define V3D_PCTR13 0x006e8
#define V3D_PCTRS13 0x006ec
#define V3D_PCTR14 0x006f0
#define V3D_PCTRS14 0x006f4
#define V3D_PCTR15 0x006f8
#define V3D_PCTRS15 0x006fc
#define V3D_DBGE 0x00f00
#define V3D_FDBGO 0x00f04
#define V3D_FDBGB 0x00f08
#define V3D_FDBGR 0x00f0c
#define V3D_FDBGS 0x00f10
#define V3D_ERRSTAT 0x00f20
static struct {
        int offset;
        const char* name;
        const char* desc;
}
registers[] = {
{ 0x00000, "V3D_IDENT0", "V3D Identification 0 (V3D block identity)" },
{ 0x00004,"V3D_IDENT1","V3D Identification 1 (V3D Configuration A)" },
{ 0x00008,"V3D_IDENT2","V3D Identification 1 (V3D Configuration B)" },
{ 0x00010,"V3D_SCRATCH","Scratch Register" },
{ 0x00020,"V3D_L2CACTL","L2 Cache Control" },
{ 0x00024,"V3D_SLCACTL","Slices Cache Control" },
{ 0x00030,"V3D_INTCTL","Interrupt Control" },
{ 0x00034,"V3D_INTENA","Interrupt Enables" },
{ 0x00038,"V3D_INTDIS","Interrupt Disables" },
{ 0x00100,"V3D_CT0CS","Control List Executor Thread 0 Control and Status." },
{ 0x00104,"V3D_CT1CS","Control List Executor Thread 1 Control and Status." },
{ 0x00108,"V3D_CT0EA","Control List Executor Thread 0 End Address." },
{ 0x0010c,"V3D_CT1EA","Control List Executor Thread 1 End Address." },
{ 0x00110,"V3D_CT0CA","Control List Executor Thread 0 Current Address." },
{ 0x00114,"V3D_CT1CA","Control List Executor Thread 1 Current Address." },
{ 0x00118,"V3D_CT00RA0","Control List Executor Thread 0 Return Address." },
{ 0x0011c,"V3D_CT01RA0","Control List Executor Thread 1 Return Address." },
{ 0x00120,"V3D_CT0LC","Control List Executor Thread 0 List Counter" },
{ 0x00124,"V3D_CT1LC","Control List Executor Thread 1 List Counter" },
{ 0x00128,"V3D_CT0PC","Control List Executor Thread 0 Primitive List Counter" },
{ 0x0012c,"V3D_CT1PC","Control List Executor Thread 1 Primitive List Counter" },
{ 0x00130,"V3D_PCS","V3D Pipeline Control and Status" },
{ 0x00134,"V3D_BFC","Binning Mode Flush Count" },
{ 0x00138,"V3D_RFC","Rendering Mode Frame Count" },
{ 0x00300,"V3D_BPCA","Current Address of Binning Memory Pool" },
{ 0x00304,"V3D_BPCS","Remaining Size of Binning Memory Pool" },
{ 0x00308,"V3D_BPOA","Address of Overspill Binning Memory Block" },
{ 0x0030c,"V3D_BPOS","Size of Overspill Binning Memory Block" },
{ 0x00310,"V3D_BXCF","Binner Debug" },
{ 0x00410,"V3D_SQRSV0","Reserve QPUs 0-7" },
{ 0x00414,"V3D_SQRSV1","Reserve QPUs 8-15" },
{ 0x00418,"V3D_SQCNTL","QPU Scheduler Control" },
{ 0x00430,"V3D_SRQPC","QPU User Program Request Program Address" },
{ 0x00434,"V3D_SRQUA","QPU User Program Request Uniforms Address" },
{ 0x00438,"V3D_SRQUL","QPU User Program Request Uniforms Length" },
{ 0x0043c,"V3D_SRQCS","QPU User Program Request Control and Status" },
{ 0x00500,"V3D_VPACNTL","VPM Allocator Control" },
{ 0x00504,"V3D_VPMBASE","VPM base (user) memory reservation" },
{ 0x00670,"V3D_PCTRC","Performance Counter Clear" },
{ 0x00674,"V3D_PCTRE","Performance Counter Enables" },
{ 0x00680,"V3D_PCTR0","Performance Counter Count 0" },
{ 0x00684,"V3D_PCTRS0","Performance Counter Mapping 0" },
{ 0x00688,"V3D_PCTR1","Performance Counter Count 1" },
{ 0x0068c,"V3D_PCTRS1","Performance Counter Mapping 1" },
{ 0x00690,"V3D_PCTR2","Performance Counter Count 2" },
{ 0x00694,"V3D_PCTRS2","Performance Counter Mapping 2" },
{ 0x00698,"V3D_PCTR3","Performance Counter Count 3" },
{ 0x0069c,"V3D_PCTRS3","Performance Counter Mapping 3" },
{ 0x006a0,"V3D_PCTR4","Performance Counter Count 4" },
{ 0x006a4,"V3D_PCTRS4","Performance Counter Mapping 4" },
{ 0x006a8,"V3D_PCTR5","Performance Counter Count 5" },
{ 0x006ac,"V3D_PCTRS5","Performance Counter Mapping 5" },
{ 0x006b0,"V3D_PCTR6","Performance Counter Count 6" },
{ 0x006b4,"V3D_PCTRS6","Performance Counter Mapping 6" },
{ 0x006b8,"V3D_PCTR7","Performance Counter Count 7" },
{ 0x006bc,"V3D_PCTRS7","Performance Counter Mapping 7" },
{ 0x006c0,"V3D_PCTR8","Performance Counter Count 8" },
{ 0x006c4,"V3D_PCTRS8","Performance Counter Mapping 8" },
{ 0x006c8,"V3D_PCTR9","Performance Counter Count 9" },
{ 0x006cc,"V3D_PCTRS9","Performance Counter Mapping 9" },
{ 0x006d0,"V3D_PCTR10","Performance Counter Count 10" },
{ 0x006d4,"V3D_PCTRS10","Performance Counter Mapping 10" },
{ 0x006d8,"V3D_PCTR11","Performance Counter Count 11" },
{ 0x006dc,"V3D_PCTRS11","Performance Counter Mapping 11" },
{ 0x006e0,"V3D_PCTR12","Performance Counter Count 12" },
{ 0x006e4,"V3D_PCTRS12","Performance Counter Mapping 12" },
{ 0x006e8,"V3D_PCTR13","Performance Counter Count 13" },
{ 0x006ec,"V3D_PCTRS13","Performance Counter Mapping 13" },
{ 0x006f0,"V3D_PCTR14","Performance Counter Count 14" },
{ 0x006f4,"V3D_PCTRS14","Performance Counter Mapping 14" },
{ 0x006f8,"V3D_PCTR15","Performance Counter Count 15" },
{ 0x006fc,"V3D_PCTRS15","Performance Counter Mapping 15" },
{ 0x00f00,"V3D_DBGE","PSE Error Signals" },
{ 0x00f04,"V3D_FDBGO","FEP Overrun Error Signals" },
{ 0x00f08,"V3D_FDBGB","FEP Interface Ready and Stall Signals, FEP Busy Signals" },
{ 0x00f0c,"V3D_FDBGR","FEP Internal Ready Signals" },
{ 0x00f10,"V3D_FDBGS","FEP Internal Stall Input Signals" },
{ 0x00f20,"V3D_ERRSTAT","Miscellaneous Error Signals (VPM, VDW, VCD, VCM, L2C)" },
};
const int VC4_BLOCK_SIZE=0x1000;

struct {
        int id;
        const char* desc;
}counters[] = {
{0, "FEP Valid primitives that result in no rendered pixels, for all rendered tiles"},
{ 1,"FEP Valid primitives for all rendered tiles. (primitives may be counted in more than one tile)" },
{ 2,"FEP Early-Z/Near/Far clipped quads" },
{ 3,"FEP Valid quads" },
{ 4,"TLB Quads with no pixels passing the stencil test" },
{ 5,"TLB Quads with no pixels passing the Z and stencil tests" },
{ 6,"TLB Quads with any pixels passing the Z and stencil tests" },
{ 7,"TLB Quads with all pixels having zero coverage" },
{ 8,"TLB Quads with any pixels having non-zero coverage" },
{ 9,"TLB Quads with valid pixels written to color buffer" },
{ 10,"PTB Primitives discarded by being outside the viewport" },
{ 11,"PTB Primitives that need clipping" },
{ 12,"PSE Primitives that are discarded because they are reversed" },
{ 13,"QPU Total idle clock cycles for all QPUs" },
{ 14,"QPU Total clock cycles for all QPUs doing vertex/coordinate shading" },
{ 15,"QPU Total clock cycles for all QPUs doing fragment shading" },
{ 16,"QPU Total clock cycles for all QPUs executing valid instructions" },
{ 17,"QPU Total clock cycles for all QPUs stalled waiting for TMUs" },
{ 18,"QPU Total clock cycles for all QPUs stalled waiting for Scoreboard" },
{ 19,"QPU Total clock cycles for all QPUs stalled waiting for Varyings" },
{ 20,"QPU Total instruction cache hits for all slices" },
{ 21,"QPU Total instruction cache misses for all slices" },
{ 22,"QPU Total uniforms cache hits for all slices" },
{ 23,"QPU Total uniforms cache misses for all slices" },
{ 24,"TMU Total texture quads processed" },
{ 25,"TMU Total texture cache misses (number of fetches from memory/L2cache)" },
{ 26,"VPM Total clock cycles VDW is stalled waiting for VPM access" },
{ 27,"VPM Total clock cycles VCD is stalled waiting for VPM access" },
{ 28,"L2C Total Level 2 cache hits" },
{ 29,"L2C Total Level 2 cache misses" },
};
#endif // VC4_DEFINES_H_INCLUDED
