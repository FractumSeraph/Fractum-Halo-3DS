"""Test Xbox compressed/uncompressed and PC gear models without game assets.
Optional real-map checks can run the compiled C reader directly on owner data.
"""
from pathlib import Path
import struct, subprocess, sys, tempfile
ROOT=Path(__file__).resolve().parents[3]
# Kevin Boggs: fixture - minimal model caches, including streamed region references.
def fixture(version,compressed):
    data=bytearray(0x6000);tag=0x1000;base=0x803a6000 if version==5 else 0x40440000
    def u(o,v):struct.pack_into('<I',data,o,v)
    def h(o,v):struct.pack_into('<H',data,o,v)
    u(0,0x68656164);u(4,version);u(8,len(data));u(16,tag);u(20,0x3000);u(2044,0x666f6f74)
    u(tag,base+64);u(tag+12,1);u(tag+(32 if version==5 else 36),0x74616773)
    if version==7:u(tag+20,0x4000);u(tag+28,0x200);u(tag+32,0x1000)
    u(tag+64,0x6d6f6465 if version==5 else 0x6d6f6432);u(tag+80,base+0xc00);u(tag+84,base+0x100)
    data[tag+0xc00:tag+0xc07]=b'fixture'
    u(tag+0x100+208,2);u(tag+0x100+212,base+0x400)
    u(tag+0x100+196,2);u(tag+0x100+200,base+0xa00)
    for i in range(2):
        u(tag+0xa00+i*76+64,1);u(tag+0xa00+i*76+68,base+0xb00+i*88)
        h(tag+0xb00+i*88+64+8,i)
        u(tag+0x400+i*48+36,1);u(tag+0x400+i*48+40,base+0x500)
    part=tag+0x500;u(part+68,1);u(part+72,1);u(part+88,3)
    stride=32 if compressed else 68
    vertex=tag+0x700 if version==5 else 0x4000;strip=tag+0x900 if version==5 else 0x4200
    if version==5:
        u(part+76,base+0x900);u(part+84,5 if compressed else 4);u(part+100,base+0x600);u(tag+0x604,base+0x700)
        u(part+96,0xdeadbeef) # never use the stale Xbox base_address field
    for i in range(3):
        struct.pack_into('<3f',data,vertex+i*stride,float(i),0.,0.)
        if compressed:u(vertex+i*stride+12,1023);struct.pack_into('<hh',data,vertex+i*stride+24,8191,-16384)
        else:struct.pack_into('<3f',data,vertex+i*stride+12,1.,0.,0.);struct.pack_into('<2f',data,vertex+i*stride+48,.25,-.5)
        h(strip+i*2,i)
    return data,vertex,strip
command=sys.argv[1:]
if command and command[0]=='--':command.pop(0)
if not command:command=['clang']
with tempfile.TemporaryDirectory(prefix='halo-model-',ignore_cleanup_errors=True) as temp:
    p=Path(temp);exe=p/('reader.exe' if sys.platform=='win32' else 'reader')
    subprocess.run(command+['-std=c11','-I'+str(ROOT/'port/n3ds/preview')]+[str(ROOT/'port/n3ds'/f) for f in ['tests/preview_model_test.c','preview/halo_cache_reader.c','preview/halo_pc_model.c']]+['-o',str(exe)],check=True)
    for version,compressed in [(5,True),(5,False),(7,False)]:
        data,vertex,strip=fixture(version,compressed);path=p/'model.map';path.write_bytes(data)
        subprocess.run([str(exe),str(path)],check=True)
        for failure in ['index','nan','truncate']+(['descriptor','vertex'] if version==5 else []):
            bad=bytearray(data)
            if failure=='index':struct.pack_into('<H',bad,strip,99)
            elif failure=='nan':struct.pack_into('<f',bad,vertex,float('nan'))
            elif failure=='descriptor':struct.pack_into('<I',bad,0x1500+100,0xffffffff)
            elif failure=='vertex':struct.pack_into('<I',bad,0x1604,0xffffffff)
            else:bad=bad[:-1]
            path.write_bytes(bad)
            result=subprocess.run([str(exe),str(path)],capture_output=True,text=True)
            assert result.returncode!=0,f'{version} {failure}: {result.stdout}'
    print('PASS: bad descriptors, vertex addresses, indices, nonfinite positions and truncation rejected')
