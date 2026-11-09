"""Fail if an expected production file has no metrics or at most 85% coverage."""
import argparse
import json
from pathlib import Path
import sys
from collections import defaultdict

p = argparse.ArgumentParser()
p.add_argument("language", choices=["cpp", "go"])
p.add_argument("report", type=Path)
args = p.parse_args()
root = Path(__file__).resolve().parent.parent
counts = defaultdict(lambda: [0, 0])
if args.language == "cpp":
    expected = {
        f.relative_to(root).as_posix()
        for folder, ext in [("src", "*.cpp"), ("include", "*.h"), ("benchmarks", "*.cpp")]
        for f in (root / "cpp" / folder).glob(ext)
    }
    for row in json.loads(args.report.read_text())["files"]:
        name = Path(row["filename"])
        if name.is_absolute():
            name = name.relative_to(root)
        if name.parts and name.parts[0] != "cpp":
            name = Path("cpp") / name
        counts[name.as_posix()] = [row["line_covered"], row["line_total"]]
else:
    # container.go only declares interfaces and has no executable statements.
    expected = {
        f.relative_to(root).as_posix()
        for f in (root / "go").rglob("*.go")
        if not f.name.endswith("_test.go") and f.name != "container.go"
    }
    for row in args.report.read_text().splitlines()[1:]:
        where, size, hits = row.split()
        name = "go/" + where.split(":")[0].removeprefix("lab3/")
        counts[name][0] += int(size) if int(hits) > 0 else 0
        counts[name][1] += int(size)

ok = True
for name in sorted(expected):
    hits, total = counts[name]
    percent = 100 * hits / total if total else 0
    passed = total > 0 and hits * 100 > 85 * total
    ok &= passed
    print(f"{name}: {percent:6.2f}% ({hits}/{total})" + ("" if passed else "  FAIL"))
sys.exit(0 if ok else 1)
