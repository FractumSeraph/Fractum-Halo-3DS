"""port/n3ds/tools/fork_update.py <fork repo> <base> <theirs> <ours tree>:
following the FractumSeraph/OpenCE fork (docs/MULTIPLAYER-PLAN.md): the last
fork commit taken is recorded in docs/MULTIPLAYER-PLAN.md. ... the fork's changes
from base to theirs (source/, port/linux/{include,game,src}, port/third_party/
{zlib,tomlc17}) merged three-way into ours, file by file (git merge-file):
modified files merged (conflicts marked and listed), added ones copied (or
merged against an empty base if ours has one), deleted ones removed when ours
is the base's (else listed). Reports to stdout; conflicts in conflicts.txt."""
import os, subprocess, sys, tempfile

repo, base, theirs, ours = sys.argv[1:5]
scopes = ['source', 'port/linux/include', 'port/linux/game', 'port/linux/src',
          'port/third_party/zlib', 'port/third_party/tomlc17']


def git(*a):
    return subprocess.run(['git', '-C', repo] + list(a), capture_output=True).stdout


def show(rev, path):
    r = subprocess.run(['git', '-C', repo, 'show', f'{rev}:{path}'], capture_output=True)
    return lf(r.stdout) if r.returncode == 0 else None


# (the three sides with LF endings: the fork keeps CRLF in some files, this
# checkout LF, and a difference in every line conflicted whole files)
def lf(data):
    return data.replace(bytes([13, 10]), bytes([10])) if data is not None else None


def read_lf(path):
    data = lf(open(path, 'rb').read())
    open(path, 'wb').write(data)
    return data


changes = git('diff', '--name-status', '--no-renames', base, theirs, '--', *scopes).decode().splitlines()
tmp = tempfile.mkdtemp()
report = {'merged': [], 'conflict': [], 'added': [], 'deleted': [], 'kept': [], 'missing': []}
for line in changes:
    status, path = line.split('\t', 1)
    target = os.path.join(ours, path)
    if status == 'A':
        data = show(theirs, path)
        if os.path.exists(target):
            mine = read_lf(target)
            if mine == data:
                continue
            open(os.path.join(tmp, 'b'), 'wb').write(b'')
            open(os.path.join(tmp, 't'), 'wb').write(data)
            r = subprocess.run(['git', 'merge-file', '-L', 'ours', '-L', 'base', '-L', 'fork', target,
                                os.path.join(tmp, 'b'), os.path.join(tmp, 't')])
            report['conflict' if r.returncode else 'merged'].append(path)
        else:
            os.makedirs(os.path.dirname(target), exist_ok=True)
            open(target, 'wb').write(data)
            report['added'].append(path)
    elif status == 'D':
        if not os.path.exists(target):
            continue
        if read_lf(target) == show(base, path):
            os.remove(target)
            report['deleted'].append(path)
        else:
            report['kept'].append(path)
    else:
        if not os.path.exists(target):
            report['missing'].append(path)
            continue
        read_lf(target)
        open(os.path.join(tmp, 'b'), 'wb').write(show(base, path))
        open(os.path.join(tmp, 't'), 'wb').write(show(theirs, path))
        r = subprocess.run(['git', 'merge-file', '-L', 'ours', '-L', 'base', '-L', 'fork', target,
                            os.path.join(tmp, 'b'), os.path.join(tmp, 't')])
        report['conflict' if r.returncode else 'merged'].append(path)
for k, v in report.items():
    print(k, len(v))
for k in ('conflict', 'kept', 'missing'):
    for p in report[k]:
        print(' ', k, p)
open(os.path.join(ours, 'build', 'fork-update-conflicts.txt'), 'w', newline=chr(10)).write(chr(10).join(report['conflict']) + chr(10))
