#if 0
//
// Generated by 2.0.20353.0
//
//   fxc /Fh 2xBR-v3.5a.ps.h /Tps_3_0 filters\2xBR-v3.5a.cg /Emain_fragment
//    /VnPS2xBRa
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
//   sampler2D $decal;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   $IN          c0       2
//   $decal       s0       1
//

// Shader type: pixel 

xps_3_0
defconst $IN.video_size, float, vector, [1, 2], c0
defconst $IN.texture_size, float, vector, [1, 2], c1
defconst $IN.output_size, float, vector, [1, 2], cX
defconst $decal, sampler2d, object, [1, 1], s0
config AutoSerialize=false
config AutoResource=false
config PsMaxReg=14
// PsExportColorCount=1
// PsSampleControl=centers

dcl_texcoord r0.xy
dcl_texcoord1 r1

def c252, 0, 0, 0, 0
def c253, 0, 0.25, 2, 0
def c254, 0.5, 0.833333015, 4, 1
def c255, 5.47200012, 14.3520002, 28.1760006, 0.75


    exec
    mad r7, r1, c253.z, r0.xyxy
    dp3 r0.z, r1.zx, c254.w
    dp3 r0.w, r1.wy, c254.w
    add r4, r0.xyxy, -r1.zwxy
    add r2, r0.xyxy, r1
    add r5.xy, r4.zw, r1.zw
  + movs r0._, r2.x
    exec
    add r6.zw, r2.xxxy, r1.zzzw
  + adds_prev r3.x, -r1.z
    add r0.zw, r0.xxxy, r0.zzzw
  + movs r0._, r2.y
    add r6.xy, r6.zw, r1.zw
  + adds_prev r3.y, -r1.w
    tfetch2D r11.xyz_, r2.zw, tf0
    tfetch2D r3.xyz_, r3.xy, tf0
    tfetch2D r5._xyz, r5.xy, tf0
    exec
    tfetch2D r2.xyz_, r2.xy, tf0
    tfetch2D r8.xyz_, r7.xy, tf0
    tfetch2D r9.xyz_, r7.zw, tf0
    tfetch2D r10.xyz_, r6.zw, tf0
    tfetch2D r13.xyz_, r6.xy, tf0
    tfetch2D r14.xyz_, r0.zw, tf0
    exec
    tfetch2D r1.xyz_, r4.xy, tf0
    tfetch2D r6._xyz, r4.zw, tf0
    tfetch2D r7.xyz_, r0.xy, tf0
    alloc colors
    exec
    mul r4.xy, r0.yx, c1.yx
    dp3 r12.x, r7.zxy, c255.xyz
    dp3 r12.y, r6.wyz, c255.xyz
    dp3 r12.z, r1.zxy, c255.xyz
    dp3 r4.z, r14.zxy, c255.xyz
    dp3 r4.w, r13.zxy, c255.xyz
    exec
    dp3 r12.w, r10.zxy, c255.xyz
    dp3 r0.x, r9.zxy, c255.xyz
    dp3 r0.y, r8.zxy, c255.xyz
    dp3 r5.x, r2.zxy, c255.xyz
    dp3 r5.y, r5.wyz, c255.xyz
    dp3 r5.z, r3.zxy, c255.xyz
    exec
    dp3 r5.w, r11.zxy, c255.xyz
    add r9.xyz, r5_abs.wxw, -r5_abs.xyz
    add r8.xw, r12_abs.w, -r0_abs.yx
    add r8.yz, -r12_abs.zzy, r5_abs.xxw
  + movs r0._, r5_abs.x
    add r0.xyz, r12_abs.x, -r12_abs.wyz
  + adds_prev r1.w, -r4_abs.z
    add r3, r12_abs.x, -r5_abs.zywx
  + movs r0._, r5_abs.w
    exec
    sne r10.xyz, r12_abs.x, r5_abs.xyz
  + adds_prev r2.w, -r4_abs.w
    mul r4.w, r0_abs.x, c254.z
  + adds r0.w, r3_abs.xy
    sne r5.xyz, r12_abs.xyz, r5_abs.wyz
  + movs r0._, r9_abs.x
    sge r0.x, r0_abs.y, r3_abs.z
  + muls_prev r4.z, c254.z
    sge r0.y, r0_abs.z, r0_abs.y
  + adds r6.x, r9_abs.yy
    sge r0.z, r3_abs.w, r0_abs.z
  + movs r0._, r8_abs.z
    exec
    sge r3.x, r3_abs.z, r3_abs.w
  + adds_prev r2.w, r2_abs.w
    cndgt r3.xyz, r3.x, r2.xyz, r11.xyz
    cndgt r2.xyz, r0.z, r1.xyz, r2.xyz
    cndgt r1.xyz, r0.y, r6.yzw, r1.xyz
    cndgt r0.xyz, r0.x, r11.xyz, r6.yzw
    add r6.w, r2.w, r1_abs.w
  + adds r6.y, r9_abs.zz
    exec
    mul r5.xyz, r10.xyz, r5.xyz
  + movs r0._, r0.w
    sge r6.yz, r9_abs.zzy, r6.xxy
  + adds_prev r6.x, r8_abs.w
    add r6.xw, r6.xw, r8_abs.xy
  + snes r5.x, r5.x
    add r4.zw, r6.xxxw, r4.zzzw
  + snes r5.y, r5.y
    sgt r6.x, r4.w, r4.z
  + snes r5.z, r5.z
    mul r8.xyz, r6.xyz, r5.xyz
    exec    // PredicateClean=false
    sne r6.zw, r8.yyyz, c253.x
  + frcs r6.y, r4.x
    setp_ne_push r0.w, c253.x, r8.x
  + frcs r6.x, r4.y
    (p0) mul r1.w, r6.z, r6.w
    (p0) add r3.xyz, r3.zyx, -r7.zyx
    setp_ne_push r0.w, r0.w, r1.w
    (p0) mad r4.xyz, r3.zyx, c254.y, r7.xyz
    exec    // PredicateClean=false
    (p0) mad r9.xyz, r3.zyx, c253.y, r7.xyz
    (p0) mad r5.xyz, r3.zyx, c253.y, r7.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.y
    (p0) mov r5.xyz, r7.xyz
    (p0) mad r4.xyz, r3.zyx, c255.w, r7.xyz
    exec    // PredicateClean=false
    (p0) mad r9.xyz, r3.zyx, c253.y, r7.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.z
    (p0) mad r4.xyz, r3.zyx, c255.w, r7.xyz
    (p0) mad r5.xyz, r3.zyx, c253.y, r7.xyz
    setp_inv r0.w, r0.w
    exec    // PredicateClean=false
    (p0) mov r5.xyz, r7.xyz
    (p0) mad r4.xyz, r3.zyx, c254.x, r7.xyz
    setp_pop r0.w, r0.w
    (p0) mov r9.xyz, r7.xyz
    setp_pop r0.w, r0.w
    setp_pop r0.w, r0.w
    exec    // PredicateClean=false
    setp_inv r0.w, r0.w
    (p0) mov r5.xyz, r7.xyz
    (p0) mov r9.xyz, r7.xyz
    (p0) mov r4.xyz, r7.xyz
    setp_ne_push r0.w, c253.x, r8.x
    (p0) mul r1.w, r6.z, r6.w
    exec    // PredicateClean=false
    (p0) add r3.xyz, r2.zyx, -r5.zyx
    setp_ne_push r0.w, r0.w, r1.w
    (p0) add r10.xyz, r2.xyz, -r7.xyz
    (p0) mad r5.xyz, r3.zyx, c254.y, r5.xyz
    (p0) add r2.xyz, r2.xyz, -r4.xyz
    (p0) mad r4.xyz, r2.xyz, c253.y, r4.xyz
    exec    // PredicateClean=false
    (p0) mad r7.xyz, r10.xyz, c253.y, r7.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.y
    (p0) add r2.xyz, r2.xyz, -r4.xyz
    (p0) mad r5.xyz, r3.zyx, c255.w, r5.xyz
    (p0) mad r4.xyz, r2.xyz, c253.y, r4.xyz
    exec    // PredicateClean=false
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.z
    (p0) add r2.xyz, r2.xyz, -r7.xyz
    (p0) mad r5.xyz, r3.zyx, c255.w, r5.xyz
    (p0) mad r7.xyz, r2.xyz, c253.y, r7.xyz
    setp_inv r0.w, r0.w
    exec    // PredicateClean=false
    (p0) mad r5.xyz, r3.zyx, c254.x, r5.xyz
    setp_ne_push r0.w, c253.x, r8.x
    (p0) mul r1.w, r6.z, r6.w
    (p0) add r2.xyz, r1.zyx, -r7.zyx
    setp_ne_push r0.w, r0.w, r1.w
    (p0) add r3.xyz, r1.xyz, -r9.xyz
    exec    // PredicateClean=false
    (p0) mad r7.xyz, r2.zyx, c254.y, r7.xyz
    (p0) add r1.xyz, r1.xyz, -r5.xyz
    (p0) mad r5.xyz, r1.xyz, c253.y, r5.xyz
    (p0) mad r9.xyz, r3.xyz, c253.y, r9.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.y
    exec    // PredicateClean=false
    (p0) add r1.xyz, r1.xyz, -r5.xyz
    (p0) mad r7.xyz, r2.zyx, c255.w, r7.xyz
    (p0) mad r5.xyz, r1.xyz, c253.y, r5.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.z
    (p0) add r1.xyz, r1.xyz, -r9.xyz
    exec    // PredicateClean=false
    (p0) mad r7.xyz, r2.zyx, c255.w, r7.xyz
    (p0) mad r9.xyz, r1.xyz, c253.y, r9.xyz
    setp_inv r0.w, r0.w
    (p0) mad r7.xyz, r2.zyx, c254.x, r7.xyz
    setp_ne_push r0.w, c253.x, r8.x
    (p0) mul r1.w, r6.z, r6.w
    exec    // PredicateClean=false
    (p0) add r1.xyz, r0.zyx, -r9.zyx
    setp_ne_push r0.w, r0.w, r1.w
    (p0) add r2.xyz, r0.xyz, -r4.xyz
    (p0) mad r9.xyz, r1.zyx, c254.y, r9.xyz
    (p0) add r0.xyz, r0.xyz, -r7.xyz
    (p0) mad r7.xyz, r0.xyz, c253.y, r7.xyz
    exec    // PredicateClean=false
    (p0) mad r4.xyz, r2.xyz, c253.y, r4.xyz
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.y
    (p0) add r0.xyz, r0.xyz, -r7.xyz
    (p0) mad r9.xyz, r1.zyx, c255.w, r9.xyz
    (p0) mad r7.xyz, r0.xyz, c253.y, r7.xyz
    exec    // PredicateClean=false
    setp_inv r0.w, r0.w
    setp_ne_push r0.w, r0.w, r8.z
    (p0) add r0.xyz, r0.xyz, -r4.xyz
    (p0) mad r9.xyz, r1.zyx, c255.w, r9.xyz
    (p0) mad r4.xyz, r0.xyz, c253.y, r4.xyz
    setp_inv r0.w, r0.w
    exece
    (p0) mad r9.xyz, r1.zyx, c254.x, r9.xyz
    sgt r0.xw, c254.x, r6.yx
    cndge r1.xyz, -r0_abs.x, r9.xyz, r7.xyz
    cndge r0.xyz, -r0_abs.x, r4.xyz, r5.xyz
    cndgt oC0.xyz1, r0.w, r1.xyz, r0.xyz

// PDB hint 00000000-00000000-00000000

#endif

// This microcode is in native DWORD byte order.

const DWORD PS2xBRa[] =
{
    0x102a1100, 0x0000014c, 0x00000814, 0x00000000, 0x00000024, 0x000000fc, 
    0x00000124, 0x00000000, 0x00000000, 0x000000d4, 0x0000001c, 0x000000c7, 
    0xffff0300, 0x00000002, 0x0000001c, 0x00000000, 0x000000c0, 0x00000044, 
    0x00020000, 0x00020000, 0x00000098, 0x00000000, 0x000000a8, 0x00030000, 
    0x00010000, 0x000000b0, 0x00000000, 0x24494e00, 0x76696465, 0x6f5f7369, 
    0x7a6500ab, 0x00010003, 0x00010002, 0x00010000, 0x00000000, 0x74657874, 
    0x7572655f, 0x73697a65, 0x006f7574, 0x7075745f, 0x73697a65, 0x00ababab, 
    0x00000048, 0x00000054, 0x00000064, 0x00000054, 0x00000071, 0x00000054, 
    0x00050000, 0x00010006, 0x00010003, 0x00000080, 0x24646563, 0x616c00ab, 
    0x0004000c, 0x00010001, 0x00010000, 0x00000000, 0x70735f33, 0x5f300032, 
    0x2e302e32, 0x30333533, 0x2e3000ab, 0x00000000, 0x00000001, 0x00000000, 
    0x00000000, 0x00000014, 0x01fc0010, 0x00000000, 0x00000000, 0x00000000, 
    0x00000000, 0x00000040, 0x000007d4, 0x10000e00, 0x00000004, 0x00000000, 
    0x00001842, 0x00030003, 0x00000001, 0x00003050, 0x0000f151, 0x00000000, 
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3e800000, 0x40000000, 
    0x00000000, 0x3f000000, 0x3f555550, 0x40800000, 0x3f800000, 0x40af1aa0, 
    0x4165a1cb, 0x41e16873, 0x3f400000, 0x0000600e, 0x60141200, 0x12000540, 
    0x0555601a, 0x30201200, 0x12000015, 0x00000000, 0x6023c400, 0x12000000, 
    0x00006029, 0x602f1200, 0x12000000, 0x00006035, 0x603b1200, 0x12000000, 
    0x00006041, 0x60471200, 0x10000000, 0x0000604d, 0x60531000, 0x10000000, 
    0x00006059, 0x605f1000, 0x10000000, 0x00006065, 0x606b1000, 0x10000000, 
    0x00006071, 0x60771000, 0x10000000, 0x0000607d, 0x60831000, 0x10000000, 
    0x00006089, 0x608f1000, 0x10000000, 0x00006095, 0x609b1000, 0x10000000, 
    0x000050a1, 0x00002200, 0x00000000, 0xc80f0007, 0x0000c6a0, 0xab01fd00, 
    0xc8040000, 0x006e1b00, 0xb001fe00, 0xc8080000, 0x00b31b00, 0xb001fe00, 
    0xc80f0004, 0x02a0aa00, 0xe0000100, 0xc80f0002, 0x00a00000, 0xe0000100, 
    0x14030005, 0x001a1a6c, 0xe0040102, 0x041c0306, 0x01ac06c6, 0xe0020101, 
    0x140c0000, 0x00ac06b1, 0xe0000002, 0x04230306, 0x011a1a1b, 0xe0060101, 
    0xb808b041, 0x1f1ffe88, 0x00004000, 0x10083061, 0x1f1ffe88, 0x00004000, 
    0x100850a1, 0x1f1ff447, 0x00004000, 0x10082041, 0x1f1ffe88, 0x00004000, 
    0x100880e1, 0x1f1ffe88, 0x00004000, 0xb80890e1, 0x1f1ffe88, 0x00004000, 
    0xb808a0c1, 0x1f1ffe88, 0x00004000, 0x1008d0c1, 0x1f1ffe88, 0x00004000, 
    0xb808e001, 0x1f1ffe88, 0x00004000, 0x10081081, 0x1f1ffe88, 0x00004000, 
    0xb8086081, 0x1f1ff447, 0x00004000, 0x10087001, 0x1f1ffe88, 0x00004000, 
    0xc8030004, 0x006d6d00, 0xa1000100, 0xc801000c, 0x00bec000, 0xb007ff00, 
    0xc802000c, 0x00c3c000, 0xb006ff00, 0xc804000c, 0x00bec000, 0xb001ff00, 
    0xc8040004, 0x00bec000, 0xb00eff00, 0xc8080004, 0x00bec000, 0xb00dff00, 
    0xc808000c, 0x00bec000, 0xb00aff00, 0xc8010000, 0x00bec000, 0xb009ff00, 
    0xc8020000, 0x00bec000, 0xb008ff00, 0xc8010005, 0x00bec000, 0xb002ff00, 
    0xc8020005, 0x00c3c000, 0xb005ff00, 0xc8040005, 0x00bec000, 0xb003ff00, 
    0xc8080005, 0x00bec000, 0xb00bff00, 0xc8070009, 0x021fc000, 0xe0858500, 
    0xc8090008, 0x021b6d00, 0xe08c8000, 0x14060008, 0x04b61c6c, 0xe08c8585, 
    0x04870100, 0x036cc3c6, 0xe08c8c84, 0x140f0003, 0x026c521b, 0xe08c8585, 
    0x0487020a, 0x016cc01b, 0xe78c8584, 0x00880004, 0x006cc661, 0xa180fe83, 
    0x14070005, 0x00c0c36c, 0xe78c8589, 0x0c410400, 0x00b1c6c6, 0xc68083fe, 
    0x00120600, 0x00c6b1b1, 0xe6808089, 0x14040000, 0x001bc6c6, 0xe6838088, 
    0x04810203, 0x00c61b1b, 0xe6838382, 0xc8070003, 0x006cc0c0, 0xee03020b, 
    0xc8070002, 0x00c6c0c0, 0xee000102, 0xc8070001, 0x00b115c0, 0xee000601, 
    0xc8070000, 0x006cc015, 0xee000b06, 0x00280606, 0x001b1bc6, 0xe0028189, 
    0x14070005, 0x00c0c01b, 0xe10a0500, 0x04160606, 0x00b6bc1b, 0xe6890688, 
    0x28190506, 0x0018b06c, 0xe0068805, 0x282c0504, 0x002c06b1, 0xe0060405, 
    0x28410506, 0x001bc6c6, 0xe5040405, 0xc8070008, 0x00c0c000, 0xe1060500, 
    0x2c2c0606, 0x00f16c6c, 0xa708fd04, 0x2c180600, 0x006c6cb1, 0x75fd0804, 
    0xc8080001, 0x18c61b00, 0xe1060600, 0xc8070003, 0x1a626200, 0xe0030700, 
    0xc8080000, 0x001b1b00, 0xf5000100, 0xc8070004, 0x1862b1c0, 0xab03fe07, 
    0xc8070009, 0x1862b1c0, 0xab03fd07, 0xc8070005, 0x1862b1c0, 0xab03fd07, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8080000, 0x001bb100, 0xf5000800, 
    0xc8070005, 0x18c0c000, 0xe2070700, 0xc8070004, 0x18621bc0, 0xab03ff07, 
    0xc8070009, 0x1862b1c0, 0xab03fd07, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8080000, 0x001bc600, 0xf5000800, 0xc8070004, 0x18621bc0, 0xab03ff07, 
    0xc8070005, 0x1862b1c0, 0xab03fd07, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8070005, 0x18c0c000, 0xe2070700, 0xc8070004, 0x18626cc0, 0xab03fe07, 
    0x80800000, 0x0000001b, 0xe2000000, 0xc8070009, 0x18c0c000, 0xe2070700, 
    0x80800000, 0x0000001b, 0xe2000000, 0x80800000, 0x0000001b, 0xe2000000, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8070005, 0x18c0c000, 0xe2070700, 
    0xc8070009, 0x18c0c000, 0xe2070700, 0xc8070004, 0x18c0c000, 0xe2070700, 
    0xc8080000, 0x006c6c00, 0x75fd0800, 0xc8080001, 0x18c61b00, 0xe1060600, 
    0xc8070003, 0x1a626200, 0xe0020500, 0xc8080000, 0x001b1b00, 0xf5000100, 
    0xc807000a, 0x1ac0c000, 0xe0020700, 0xc8070005, 0x1862b1c0, 0xab03fe05, 
    0xc8070002, 0x1ac0c000, 0xe0020400, 0xc8070004, 0x18c0b1c0, 0xab02fd04, 
    0xc8070007, 0x18c0b1c0, 0xab0afd07, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8080000, 0x001bb100, 0xf5000800, 0xc8070002, 0x1ac0c000, 0xe0020400, 
    0xc8070005, 0x18621bc0, 0xab03ff05, 0xc8070004, 0x18c0b1c0, 0xab02fd04, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8080000, 0x001bc600, 0xf5000800, 
    0xc8070002, 0x1ac0c000, 0xe0020700, 0xc8070005, 0x18621bc0, 0xab03ff05, 
    0xc8070007, 0x18c0b1c0, 0xab02fd07, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8070005, 0x18626cc0, 0xab03fe05, 0xc8080000, 0x006c6c00, 0x75fd0800, 
    0xc8080001, 0x18c61b00, 0xe1060600, 0xc8070002, 0x1a626200, 0xe0010700, 
    0xc8080000, 0x001b1b00, 0xf5000100, 0xc8070003, 0x1ac0c000, 0xe0010900, 
    0xc8070007, 0x1862b1c0, 0xab02fe07, 0xc8070001, 0x1ac0c000, 0xe0010500, 
    0xc8070005, 0x18c0b1c0, 0xab01fd05, 0xc8070009, 0x18c0b1c0, 0xab03fd09, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8080000, 0x001bb100, 0xf5000800, 
    0xc8070001, 0x1ac0c000, 0xe0010500, 0xc8070007, 0x18621bc0, 0xab02ff07, 
    0xc8070005, 0x18c0b1c0, 0xab01fd05, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8080000, 0x001bc600, 0xf5000800, 0xc8070001, 0x1ac0c000, 0xe0010900, 
    0xc8070007, 0x18621bc0, 0xab02ff07, 0xc8070009, 0x18c0b1c0, 0xab01fd09, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8070007, 0x18626cc0, 0xab02fe07, 
    0xc8080000, 0x006c6c00, 0x75fd0800, 0xc8080001, 0x18c61b00, 0xe1060600, 
    0xc8070001, 0x1a626200, 0xe0000900, 0xc8080000, 0x001b1b00, 0xf5000100, 
    0xc8070002, 0x1ac0c000, 0xe0000400, 0xc8070009, 0x1862b1c0, 0xab01fe09, 
    0xc8070000, 0x1ac0c000, 0xe0000700, 0xc8070007, 0x18c0b1c0, 0xab00fd07, 
    0xc8070004, 0x18c0b1c0, 0xab02fd04, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8080000, 0x001bb100, 0xf5000800, 0xc8070000, 0x1ac0c000, 0xe0000700, 
    0xc8070009, 0x18621bc0, 0xab01ff09, 0xc8070007, 0x18c0b1c0, 0xab00fd07, 
    0x7c800000, 0x0000001b, 0xe2000000, 0xc8080000, 0x001bc600, 0xf5000800, 
    0xc8070000, 0x1ac0c000, 0xe0000400, 0xc8070009, 0x18621bc0, 0xab01ff09, 
    0xc8070004, 0x18c0b1c0, 0xab00fd04, 0x7c800000, 0x0000001b, 0xe2000000, 
    0xc8070009, 0x18626cc0, 0xab01fe09, 0xc8090000, 0x006c6d00, 0x65fe0600, 
    0xc8070001, 0x046cc0c0, 0xed800907, 0xc8070000, 0x046cc0c0, 0xed800405, 
    0xc88fc000, 0x001bc0c0, 0xee000100, 0x00000000, 0x00000000, 0x00000000
};