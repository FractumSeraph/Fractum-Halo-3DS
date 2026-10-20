"""port/n3ds/tools/fork_update.py <fork repo> <base> <theirs> <ours tree>:
following the FractumSeraph/OpenCE fork (docs/MULTIPLAYER-PLAN.md): the last
fork commit taken is 8ab6b1324. ... the fork's changes
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
    return r.stdout if r.returncode == 0 else None


changes = git('diff', '--name-status', '--no-renames', base, theirs, '--', *scopes).decode().splitlines()
tmp = tempfile.mkdtemp()
report = {'merged': [], 'conflict': [], 'added': [], 'deleted': [], 'kept': [], 'missing': []}
for line in changes:
    status, path = line.split('\t', 1)
    target = os.path.join(ours, path)
    if status == 'A':
        data = show(theirs, path)
        if os.path.exists(target):
            mine = open(target, 'rb').read()
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
        if open(target, 'rb').read() == show(base, path):
            os.remove(target)
            report['deleted'].append(path)
        else:
            report['kept'].append(path)
    else:
        if not os.path.exists(target):
            report['missing'].append(path)
            continue
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
open(os.path.join(ours, 'build', 'fork-update-conflicts.txt'), 'w').write('\n'.join(report['conflict']) + '\n')
