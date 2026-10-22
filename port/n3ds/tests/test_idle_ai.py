# New3DS integration additions by Kevin Boggs.
"""Exercise the actual decision scheduler with mocked engine inputs."""
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[3]

# Kevin Boggs: main
def main():
    compiler = sys.argv[sys.argv.index('--') + 1:]
    source = (ROOT / 'source/ai/actors.c').read_text()
    start = source.index('static unsigned int native_decisions_deferred;')
    end = source.index('\n#endif', start)
    fixture = (ROOT / 'port/n3ds/tests/src/idle_ai_tests.c').read_text()
    fixture = fixture.replace('/* SCHEDULER_IMPLEMENTATION */', source[start:end])
    with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as directory:
        test = Path(directory) / 'idle-ai.c'
        test.write_text(fixture)
        exe = test.with_suffix('.exe')
        subprocess.run(compiler + [str(test), '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)

if __name__ == '__main__':
    main()
