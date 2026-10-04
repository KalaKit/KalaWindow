import sys
import platform
import subprocess
import shutil
import tomllib
import argparse
import logging
import re
from pathlib import Path
from dataclasses import dataclass
from typing import List
from typing import Optional

SCRIPT_DIR = Path(__file__).parent.resolve()
TOML_PATH = SCRIPT_DIR / "build.toml"

PLATFORM: str = ""

REQUIRED_SECTIONS = [
    "windows-sync",
    "windows-post-build-sync",
    "windows-gnu-sync",
    "windows-gnu-post-build-sync",
    "linux-sync",
    "linux-post-build-sync",
]

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s: %(message)s",
    datefmt="%H:%M:%S")

@dataclass
class ProjectTable:
    name: str
    version: str
    kmake: str

@dataclass
class TargetEntry:
    id: str
    origin: str
    target: str
    action: str

@dataclass
class TargetTable:
    entries: List[TargetEntry]

@dataclass
class ProjectInfo:
    project_table: ProjectTable
    windows_table: TargetTable
    windows_post_build_table: TargetTable
    windows_gnu_table: TargetTable
    windows_gnu_post_build_table: TargetTable
    linux_table: TargetTable
    linux_post_build_table: TargetTable

def action_verify() -> ProjectInfo:
    print("----------------------------------------")
    print(f"[ VERIFYING TOML FILE '{TOML_PATH.name}' ]")

    toml_path = Path(TOML_PATH)

    if not toml_path.is_file():
        logging.error(f"Did not find toml file!")
        sys.exit(1)

    try:
        with open(toml_path, "rb") as f:
            data = tomllib.load(f)
    except Exception as e:
        logging.error(f"Toml file is malformed! Reason: {e}")
        sys.exit(1)

    # Project section must exist
    if "project" not in data:
        logging.error(f"Section 'project' is missing!")
        sys.exit(1)

    proj = data["project"]
    if not isinstance(proj, dict):
        logging.error(f"Section 'project' must be a table!")
        sys.exit(1)

    for field in ("name", "version", "kmake"):
        if field not in proj:
            logging.error(f"Field '{field}' is missing in section 'project'!")
            sys.exit(1)
        if not isinstance(proj[field], str) or not proj[field].strip():
            logging.error(f"Field '{field}' in section 'project' must be a single non-empty value!")
            sys.exit(1)

    # References section must exist
    if "references" not in data or not isinstance(data["references"], dict):
        logging.error(f"Section 'references' is missing or not a table!")
        sys.exit(1)

    ref_sec = data["references"]
    for k, v in ref_sec.items():
        if isinstance(v, (list, dict)) or v is None or (isinstance(v, str) and not v.strip()):
            logging.error(f"Field '{k}' in section 'references' must be a single non-empty value!")
            sys.exit(1)

    # Reference lookup
    lookup = {k: str(v).strip() for k, v in ref_sec.items()}
    lookup["version"] = proj["version"].strip()
    lookup["name"] = proj["name"].strip()

    pattern = re.compile(r"\$\{([^}]+)\}")

    def resolve(s: str, section: str, field: str) -> str:
        def repl(m):
            key = m.group(1).strip()
            if key not in lookup:
                logging.error(f"Unknown placeholder '${{{key}}}' in '{section}.{field}'!")
                sys.exit(1)
            return lookup[key]
        return pattern.sub(repl, s)

    def parse_table(name: str) -> TargetTable:
        if name not in data:
            logging.error(f"Section '{name}' is missing!")
            sys.exit(1)
        sec = data[name]
        if not isinstance(sec, dict):
            logging.error(f"Section '{name}' must be a table!")
            sys.exit(1)
        
        entries: List[TargetEntry] = []
        for entry_id, val in sec.items():
            if not isinstance(val, list) or len(val) != 3:
                logging.error(f"Field '{entry_id}' in '{name}' must be [origin, target, action]!")
                sys.exit(1)

            o, t, a = val

            if not isinstance(o, str) or not o.strip():
                logging.error(f"Origin in '{name}.{entry_id}' was empty!")
                sys.exit(1)
            if not isinstance(t, str):
                logging.error(f"Target in '{name}.{entry_id}' must be a string!")
                sys.exit(1)
            if not isinstance(a, str) or not a.strip():
                logging.error(f"Action in '{name}.{entry_id}' was empty!")
                sys.exit(1)
                
            a = a.strip().lower()
            if a not in ("copy", "move", "delete"):
                logging.error(f"Action '{a}' in '{name}.{entry_id}' must be 'copy', 'move' or 'delete'!")
                sys.exit(1)

            o_resolved = resolve(o.strip(), name, entry_id)

            # Delete target must always be empty
            if a == "delete":
                if t.strip() != "":
                    logging.error(f"Target in '{name}.{entry_id}' must be empty!")
                    sys.exit(1)
                entries.append(TargetEntry(id=entry_id, origin=o_resolved, target="", action=a))
            else:
                if not t.strip():
                    logging.error(f"Target in '{name}.{entry_id}' was empty for action '{a}'!")
                    sys.exit(1)
                t_resolved = resolve(t.strip(), name, entry_id)
                entries.append(TargetEntry(id=entry_id, origin=o_resolved, target=t_resolved, action=a))

        if not entries:
            logging.error(f"Section '{name}' must have atleast one entry!")
            sys.exit(1)

        return TargetTable(entries=entries)

    logging.info(f"Project '{data['project']['name']}' toml file '{toml_path.name}' verification succeeded!")

    return ProjectInfo(
        project_table=ProjectTable(
            name=proj["name"].strip(),
            version=proj["version"].strip(),
            kmake=proj["kmake"].strip()),
        windows_table=parse_table("windows-sync"),
        windows_post_build_table=parse_table("windows-post-build-sync"),
        windows_gnu_table=parse_table("windows-gnu-sync"),
        windows_gnu_post_build_table=parse_table("windows-gnu-post-build-sync"),
        linux_table=parse_table("linux-sync"),
        linux_post_build_table=parse_table("linux-post-build-sync"))

def action_sync_target_table(table: TargetTable):
    for e in table.entries:
        origin = Path(e.origin) if Path(e.origin).is_absolute() else SCRIPT_DIR / e.origin
        origin = origin.resolve()

        if e.action == "copy":
            target = Path(e.target) if Path(e.target).is_absolute() else SCRIPT_DIR / e.target
            target = target.resolve()

            if origin.is_dir():
                if target.is_file():
                    target.unlink()

                target.mkdir(parents=True, exist_ok=True)

                for item in origin.iterdir():
                    dst = target / item.name
                    if item.is_dir():
                        shutil.copytree(item, dst, dirs_exist_ok=True)
                    else:
                        if dst.exists():
                            shutil.rmtree(dst) if dst.is_dir() else dst.unlink()

                        shutil.copy2(item, dst)
            else:
                dst = target / origin.name if (target.exists() and target.is_dir()) else target

                if dst != target or not target.is_dir():
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    if dst.exists():
                        shutil.rmtree(dst) if dst.is_dir() else dst.unlink()

                shutil.copy2(origin, dst)

        elif e.action == "move":
            target = Path(e.target) if Path(e.target).is_absolute() else SCRIPT_DIR / e.target
            target = target.resolve()

            if origin.is_dir():
                if target.is_file():
                    target.unlink()

                target.mkdir(parents=True, exist_ok=True)

                for item in origin.iterdir():
                    dst = target / item.name
                    if dst.exists():
                        shutil.rmtree(dst) if dst.is_dir() else dst.unlink()

                    shutil.move(str(item), str(dst))

                try:
                    origin.rmdir()
                except OSError:
                    pass
            else:
                dst = target / origin.name if (target.exists() and target.is_dir()) else target

                if dst != target or not target.is_dir():
                    dst.parent.mkdir(parents=True, exist_ok=True)
                    if dst.exists():
                        shutil.rmtree(dst) if dst.is_dir() else dst.unlink()

                shutil.move(str(origin), str(dst))

        elif e.action == "delete":
            if not origin.exists():
                continue
            if origin.is_dir():
                shutil.rmtree(origin)
            else:
                origin.unlink()

def action_sync_target(info: ProjectInfo):
    if PLATFORM == "windows":
        print("----------------------------------------")
        print("[ SYNCING WINDOWS FILES ]")

        action_sync_target_table(info.windows_table)
    else:
        print("----------------------------------------")
        print("[ SYNCING WINDOWS-GNU FILES ]")

        action_sync_target_table(info.windows_gnu_table)

        print("----------------------------------------")
        print("[ SYNCING LINUX FILES ]")

        action_sync_target_table(info.linux_table)

    logging.info(f"Project '{info.project_table.name}' copy succeeded!")

def action_build(info: ProjectInfo, target: str):
    def action_build_target(info: ProjectInfo, target: str):
        subprocess.run(["kalamake", "--compile", f"{info.project_table.kmake}", f"release-{target}" ], check=True)
        subprocess.run(["kalamake", "--compile", f"{info.project_table.kmake}", f"debug-{target}" ], check=True)

    print("----------------------------------------")
    print(f"[ BUILDING TARGET(S) '{target}' ]")

    if target == "all":
        if PLATFORM == "windows":
            action_build_target(info, "windows")

            action_sync_target_table(info.windows_post_build_table)
        else:
            action_build_target(info, "linux")
            action_build_target(info, "windows-gnu")

            action_sync_target_table(info.windows_gnu_post_build_table)
            action_sync_target_table(info.linux_post_build_table)
    elif target == "windows":
        if PLATFORM == "windows":
            action_build_target(info, "windows")

            action_sync_target_table(info.windows_post_build_table)
        else:
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
    elif target == "windows-gnu":
        if PLATFORM == "windows":
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
        else:
            action_build_target(info, "windows-gnu")

            action_sync_target_table(info.windows_gnu_post_build_table)
    else:
        if PLATFORM == "windows":
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
        else:
            action_build_target(info, "linux")

            action_sync_target_table(info.linux_post_build_table)

    logging.info(f"Project '{info.project_table.name}' target '{target}' build succeeded!")

def main():
    global PLATFORM

    _system = platform.system().lower()
    if _system == "windows":
        PLATFORM = "windows"
    elif _system == "linux":
        PLATFORM = "linux"
    else:
        logging.error(f"Platform '{_system}' is unsupported!")
        sys.exit(1)

    p = argparse.ArgumentParser()

    p.add_argument(
        "action", 
        choices=["sync", "build"])
    p.add_argument(
        "target",
        nargs="?",
        choices=["windows", "windows-gnu", "linux"])

    args = p.parse_args()

    info = action_verify()

    if args.action == "sync":
        if args.target:
            p.error("Action 'sync' does not allow to use target!")
        action_sync_target(info)
    else: 
        target = args.target or "all"
        action_build(info, target)

if __name__ == "__main__":
    main()
