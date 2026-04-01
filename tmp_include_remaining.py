from pathlib import Path
import re
root=Path(r'F:/GraduationProject/instant-messaging-system/src')
files=[p for p in root.rglob('*') if p.suffix in {'.h','.hpp','.cpp'}]
pat=re.compile(r'^\s*#include\s+"([^"]+)"')
external_keep={'dependencyinjector.h','dependencypool.h','dependencypointer.h','dependencycreator.h','dep_global.h'}
remaining=[]
for p in files:
    for i,line in enumerate(p.read_text(encoding='utf-8',errors='ignore').splitlines(),1):
        m=pat.match(line)
        if not m: continue
        inc=m.group(1)
        if p.suffix=='.cpp' and inc.startswith('../include/'): continue
        if inc in external_keep: continue
        if inc.startswith(('sys/','ui/','contract/','resource/')): continue
        if '/' in inc and not inc.startswith('../'):
            continue
        remaining.append((p.as_posix(),i,inc))
print('remaining',len(remaining))
for r in remaining[:200]:
    print(f'{r[0]}:{r[1]} -> {r[2]}')
