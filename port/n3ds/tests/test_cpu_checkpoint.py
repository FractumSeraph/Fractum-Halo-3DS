# New3DS integration additions by Kevin Boggs.
"""Compile exact CPU/save regressions with the supplied host C compiler.

Example (from a Visual Studio developer prompt):
  python port/n3ds/tests/test_cpu_checkpoint.py -- clang -O2
"""
from pathlib import Path
import subprocess
import sys
import tempfile
ROOT=Path(__file__).resolve().parents[3]
# Kevin Boggs: main
def main():
    if '--' not in sys.argv or sys.argv.index('--')==len(sys.argv)-1:
        raise SystemExit('Usage: test_cpu_checkpoint.py -- <host C compiler> [flags]')
    compiler=sys.argv[sys.argv.index('--')+1:]
    source=(ROOT/'port/n3ds/tests/src/cpu_checkpoint_tests.c').read_text()
    models=(ROOT/'source/models/models.c').read_text()
    begin=models.index('struct native_model_name_entry {')
    end=models.index('/* Kevin Boggs: n3ds_model_name_cache_report */',begin)
    source=source.replace('/* MODEL_NAME_IMPLEMENTATION */',models[begin:end])
    storage=(ROOT/'port/n3ds/include/engine_checkpoint_storage.inl').read_text()
    path_begin=storage.index('static struct {long profile;')
    path_end=storage.index('const char *game_state_get_persistent_storage_filename',path_begin)
    source=source.replace('/* CHECKPOINT_PATH_IMPLEMENTATION */',storage[path_begin:path_end])
    begin=storage.index('static int compare_records(')
    end=storage.index('static int decode_pointer(',begin)
    source=source.replace('/* CHECKPOINT_SORT_IMPLEMENTATION */',
        'struct pointer_record {unsigned int offset,target;};\n'+storage[begin:end])
    with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as directory:
        path=Path(directory);test=path/'cpu-checkpoint.c';test.write_text(source);exe=path/'tests.exe'
        subprocess.run(compiler+['-I'+str(ROOT/'port/n3ds/include'),'-I'+str(ROOT/'port/n3ds/src'),str(test),'-o',str(exe)],check=True)
        subprocess.run([str(exe)],check=True)
if __name__=='__main__':main()

