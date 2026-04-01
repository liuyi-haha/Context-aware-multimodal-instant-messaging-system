from pathlib import Path
import re
root = Path(r'F:/GraduationProject/instant-messaging-system/src')
files = [p for p in root.rglob('*') if p.suffix in {'.h', '.hpp', '.cpp'}]
index = {}
for p in files:
    rel = p.relative_to(root).as_posix()
    index.setdefault(p.name, []).append(rel)
pat = re.compile(r'^(\s*#include\s+")([^"]+)(".*)$')
external_keep = {'dependencyinjector.h', 'dependencypool.h', 'dependencypointer.h', 'dependencycreator.h', 'dep_global.h'}
updated = 0
updated_files = 0
for p in files:
    lines = p.read_text(encoding='utf-8', errors='ignore').splitlines()
    changed = False
    for i, line in enumerate(lines):
        m = pat.match(line)
        if not m:
            continue
        prefix, inc, suffix = m.groups()
        # allow cpp to keep its direct self-header include via ../include
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
            lines[i] = f'{prefix}{new}{suffix}'
            changed = True
            updated += 1
    if changed:
        p.write_text('\n'.join(lines) + '\n', encoding='utf-8')
        updated_files += 1
print('updated includes:', updated)
print('updated files:', updated_files)
