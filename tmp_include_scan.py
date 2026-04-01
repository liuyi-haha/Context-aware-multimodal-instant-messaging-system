from pathlib import Path
import re
root = Path(r'F:/GraduationProject/instant-messaging-system/src')
files = [p for p in root.rglob('*') if p.suffix in {'.h', '.hpp', '.cpp'}]
index = {}
for p in files:
    rel = p.relative_to(root).as_posix()
    index.setdefault(p.name, []).append(rel)
pat = re.compile(r'^\s*#include\s+"([^"]+)"')
external_keep = {'dependencyinjector.h', 'dependencypool.h', 'dependencypointer.h', 'dependencycreator.h', 'dep_global.h'}
changes = 0
file_changes = {}
for p in files:
    for line in p.read_text(encoding='utf-8', errors='ignore').splitlines():
        m = pat.match(line)
        if not m:
            continue
        inc = m.group(1)
        if p.suffix == '.cpp' and inc.startswith('../include/'):
            continue
        new = None
        if inc in external_keep:
            continue
        if inc.startswith('exception/'):
            new = 'sys/common/' + inc
        elif inc == 'Avatar.hpp':
            new = 'sys/common/Avatar.hpp'
        elif inc.startswith(('sys/', 'ui/', 'contract/', 'resource/')):
            continue
        elif '/' not in inc and inc in index and len(index[inc]) == 1:
            new = index[inc][0]
        if new and new != inc:
            changes += 1
            file_changes[p.as_posix()] = file_changes.get(p.as_posix(), 0) + 1
print('planned replacements', changes)
print('files affected', len(file_changes))
for f, c in sorted(file_changes.items())[:120]:
    print(c, f)
