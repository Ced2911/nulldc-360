#if 0
//
// Generated by 2.0.20353.0
//
//   fxc /Fh 5xBR-v3.7a.vs.h /Tvs_3_0 filters\5xBR-v3.7a.cg /Emain_vertex
//    /VnVS5xBRa
//
//
// Parameters:
//
//   struct
//   {
//       float2 video_size;
//       float2 texture_size;
//       float2 output_size;
//
//   } $IN;
//   
//   float4x4 $modelViewProj;
//
//
// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   $modelViewProj c0       4
//   $IN            c4       2
//

// Shader type: vertex 

xvs_3_0
defconst $IN.video_size, float, vector, [1, 2], c4
defconst $IN.texture_size, float, vector, [1, 2], c5
defconst $IN.output_size, float, vector, [1, 2], cX
defconst $modelViewProj, float, matrix_columns, [4, 4], c0-3
config AutoSerialize=false
config AutoResource=false
config VsMaxReg=2
config VsResource=1
// VsExportCount=1

dcl_index r0.x
dcl_texcoord o0.xy
dcl_texcoord1 o1


    exec
    vfetch r2, r0.x, position
    vfetch r0.xy__, r0.x, texcoord
    alloc position
    exec
    mul r1, r2.w, c3.xwzy
    mad r1, r2.z, c2.xwzy, r1
    mad r1, r2.y, c1.xzyw, r1.xzwy
    mad oPos, r2.x, c0, r1.xzyw
    alloc interpolators
    exece
    rcp o1.x000, c5.x
    rcp o1.w, c5.y
    mov o0.xy, r0.xy

// PDB hint 00000000-00000000-00000000

#endif

// This microcode is in native DWORD byte order.

const DWORD VS5xBRa[] =
{
    0x102a1101, 0x00000148, 0x0000009c, 0x00000000, 0x00000024, 0x00000000, 
    0x00000104, 0x00000000, 0x00000000, 0x000000dc, 0x0000001c, 0x000000cf, 
    0xfffe0300, 0x00000002, 0x0000001c, 0x00000000, 0x000000c8, 0x00000044, 
    0x00020004, 0x00020000, 0x00000098, 0x00000000, 0x000000a8, 0x00020000, 
    0x00040000, 0x000000b8, 0x00000000, 0x24494e00, 0x76696465, 0x6f5f7369, 
    0x7a6500ab, 0x00010003, 0x00010002, 0x00010000, 0x00000000, 0x74657874, 
    0x7572655f, 0x73697a65, 0x006f7574, 0x7075745f, 0x73697a65, 0x00ababab, 
    0x00000048, 0x00000054, 0x00000064, 0x00000054, 0x00000071, 0x00000054, 
    0x00050000, 0x00010006, 0x00010003, 0x00000080, 0x246d6f64, 0x656c5669, 
    0x65775072, 0x6f6a00ab, 0x00030003, 0x00040004, 0x00010000, 0x00000000, 
    0x76735f33, 0x5f300032, 0x2e302e32, 0x30333533, 0x2e3000ab, 0x00000000, 
    0x0000009c, 0x00110002, 0x00000000, 0x00000000, 0x00001842, 0x00000001, 
    0x00000002, 0x00000003, 0x00000290, 0x00100003, 0x00305004, 0x00003050, 
    0x0001f151, 0x0000100b, 0x00000009, 0x0000100a, 0x30052003, 0x00001200, 
    0xc2000000, 0x00004005, 0x00001200, 0xc4000000, 0x00003009, 0x00002200, 
    0x00000000, 0x05f82000, 0x00000688, 0x00000000, 0x05f80000, 0x00000fc8, 
    0x00000000, 0xc80f0001, 0x001b8800, 0xa1020300, 0xc80f0001, 0x00c68800, 
    0xab020201, 0xc80f0001, 0x00b13494, 0xab020101, 0xc80f803e, 0x006c0034, 
    0xab020001, 0x4c10c001, 0x0000006c, 0xc2000005, 0x4c808001, 0x000000b1, 
    0xc2000005, 0xc8038000, 0x00b0b000, 0xe2000000, 0x00000000, 0x00000000, 
    0x00000000
};
