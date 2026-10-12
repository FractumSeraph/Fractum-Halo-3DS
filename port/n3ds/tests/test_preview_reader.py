"""Build the portable readers and test synthetic Xbox/PC geometry and bad ranges.
Pass host compiler flags after --. No Halo assets are needed.
"""
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
ROOT=Path(__file__).resolve().parents[3]
def fixture(version):
    data=bytearray(0x4000)
    def u(offset,value):struct.pack_into("<I",data,offset,value)
    def h(offset,value):struct.pack_into("<H",data,offset,value)
    u(0,0x68656164);u(4,version);u(8,len(data));u(16,0x2000);u(20,0x1000);u(2044,0x666f6f74)
    base=0x803a6000 if version==5 else 0x40440000
    tag=0x2000;u(tag,base+64);u(tag+12,3);u(tag+(32 if version==5 else 36),0x74616773)
    for i,(group,meta,name) in enumerate([(0x73636e72,0x100,b"scenario"),(0x73656e76,0x800,b"shader"),(0x6269746d,0x900,b"bitmap")]):
        entry=tag+64+i*32;u(entry,group);u(entry+12,0xe0000000+i);u(entry+16,base+0xc00+i*32);u(entry+20,base+meta)
        data[tag+0xc00+i*32:tag+0xc00+i*32+len(name)]=name
    u(tag+0x100+0x5a4,1);u(tag+0x100+0x5a8,base+0x700)
    address=0x81800000;u(tag+0x700,0x800);u(tag+0x704,0x900);u(tag+0x708,address)
    bsp=0x800;u(bsp,address+0x18);u(bsp+0x14,0x73627370)
    u(bsp+0x18+0xf8,1);u(bsp+0x18+0xfc,address+0x300)
    u(bsp+0x18+0x104,1);u(bsp+0x18+0x108,address+0x320)
    h(bsp+0x300,0);h(bsp+0x302,1);h(bsp+0x304,2)
    u(bsp+0x320+0x14,1);u(bsp+0x320+0x18,address+0x340)
    mat=bsp+0x340;u(mat,0x73656e76);u(mat+12,0xe0000001);u(mat+0x18,1);u(mat+0xb4,3);u(mat+0xc8,3)
    stride,uv=(32,24) if version==5 else (56,48)
    u(mat+(0xec if version==5 else 0xd8),3*(stride+(8 if version==5 else 20)))
    u(mat+(0xf8 if version==5 else 0xe4),address+0x440)
    for i,pos in enumerate([(0.,0.,0.),(1.,0.,0.),(0.,1.,0.)]):
        struct.pack_into("<3f",data,bsp+0x440+i*stride,*pos)
        struct.pack_into("<2f",data,bsp+0x440+i*stride+uv,pos[0],pos[1])
    u(tag+0x800+0x94,0xe0000002)
    u(tag+0x900+96,1);u(tag+0x900+100,base+0xa00)
    bm=tag+0xa00;u(bm,0x6269746d);h(bm+4,4);h(bm+6,4);h(bm+8,1);h(bm+12,14);u(bm+24,0x3800);u(bm+28,8)
    return data
command=sys.argv[1:]
if command and command[0]=="--":command.pop(0)
if not command:command=["clang"]
with tempfile.TemporaryDirectory(prefix="halo-preview-",ignore_cleanup_errors=True) as temp:
    p=Path(temp);exe=p/("reader.exe" if sys.platform=="win32" else "reader")
    subprocess.run(command+["-std=c11","-I"+str(ROOT/"port/n3ds/preview"),str(ROOT/"port/n3ds/tests/preview_reader_test.c"),str(ROOT/"port/n3ds/preview/halo_cache_reader.c"),str(ROOT/"port/n3ds/preview/halo_pc_bsp.c"),"-o",str(exe)],check=True)
    for version in (5,7):
        path=p/f"fixture-v{version}.map";data=fixture(version);path.write_bytes(data)
        subprocess.run([str(exe),str(path)],check=True)
        # Invalid vertex address/index and truncation must fail before decoding.
        for corrupt in ("address","index","truncate"):
            bad=bytearray(data)
            if corrupt=="address":struct.pack_into("<I",bad,0x800+0x340+(0xf8 if version==5 else 0xe4),0xffffffff)
            elif corrupt=="index":struct.pack_into("<H",bad,0x800+0x300,99)
            else:bad=bad[:-1]
            path.write_bytes(bad)
            result=subprocess.run([str(exe),str(path)],capture_output=True,text=True)
            assert result.returncode!=0,result.stdout
    print("PASS: Xbox compressed and PC uncompressed BSPs; malformed/truncated caches rejected")
