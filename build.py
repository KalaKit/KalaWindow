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
    "windows-copy",
    "windows-post-build-copy",
    "windows-gnu-copy",
    "windows-gnu-post-build-copy",
    "linux-copy",
    "linux-post-build-copy",
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
class CopyEntry:
    id: str
    origin: str
    target: str

@dataclass
class CopyTable:
    entries: List[CopyEntry]

@dataclass
class ProjectInfo:
    project: ProjectTable
    windows_copy: CopyTable
    windows_post_build_copy: CopyTable
    windows_gnu_copy: CopyTable
    windows_gnu_post_build_copy: CopyTable
    linux_copy: CopyTable
    linux_post_build_copy: CopyTable

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

    # Targets section must exist
    if "targets" not in data or not isinstance(data["targets"], dict):
        logging.error(f"Section 'targets' is missing or not a table!")
        sys.exit(1)

    targets_sec = data["targets"]
    for k, v in targets_sec.items():
        if isinstance(v, (list, dict)) or v is None or (isinstance(v, str) and not v.strip()):
            logging.error(f"Field '{k}' in section 'targets' must be a single non-empty value!")
            sys.exit(1)

    # Reference lookup
    lookup = {k: str(v).strip() for k, v in targets_sec.items()}
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

    def parse_table(name: str) -> CopyTable:
        if name not in data:
            logging.error(f"Section '{name}' is missing!")
            sys.exit(1)
        sec = data[name]
        if not isinstance(sec, dict):
            logging.error(f"Section '{name}' must be a table!")
            sys.exit(1)
        
        entries: List[CopyEntry] = []
        for entry_id, val in sec.items():
            if not isinstance(val, list) or len(val) != 2:
                logging.error(f"Field '{entry_id}' in '{name}' must be [origin, target]!")
                sys.exit(1)
            o, t = val
            if not isinstance(o, str) or not o.strip():
                logging.error(f"Origin in '{name}.{entry_id}' was empty!")
                sys.exit(1)
            if not isinstance(t, str) or not t.strip():
                logging.error(f"Target in '{name}.{entry_id}' was empty!")
                sys.exit(1)

            o = resolve(o.strip(), name, entry_id)
            t = resolve(t.strip(), name, entry_id)
            entries.append(CopyEntry(id=entry_id, origin=o, target=t))

        if not entries:
            logging.error(f"Section '{name}' must have atleast one entry!")
            sys.exit(1)

        return CopyTable(entries=entries)

    logging.info(f"Project '{data['project']['name']}' toml file '{toml_path.name}' verification succeeded!")

    return ProjectInfo(
        project=ProjectTable(
            name=proj["name"].strip(),
            version=proj["version"].strip(),
            kmake=proj["kmake"].strip()),
        windows_copy=parse_table("windows-copy"),
        windows_post_build_copy=parse_table("windows-post-build-copy"),
        windows_gnu_copy=parse_table("windows-gnu-copy"),
        windows_gnu_post_build_copy=parse_table("windows-gnu-post-build-copy"),
        linux_copy=parse_table("linux-copy"),
        linux_post_build_copy=parse_table("linux-post-build-copy"))

def action_copy_target(table: CopyTable):
    for e in table.entries:
        origin = Path(e.origin) if Path(e.origin).is_absolute() else SCRIPT_DIR / e.origin
        target = Path(e.target) if Path(e.target).is_absolute() else SCRIPT_DIR / e.target

        origin = origin.resolve()
        target = (SCRIPT_DIR / target).resolve() if not target.is_absolute() else target.resolve()

        if not origin.exists():
            logging.error(f"Origin '{e.origin}' (id '{e.id}') does not exist!")
            sys.exit(1)

        if origin.is_dir():
            # If target exists as a file, remove it - we need a dir
            if target.is_file():
                target.unlink()

            # Create target dir and all parents
            target.mkdir(parents=True, exist_ok=True)

            # Copy contents of origin inside target, override existing
            for item in origin.iterdir():
                dst = target / item.name
                if item.is_dir():
                    shutil.copytree(item, dst, dirs_exist_ok=True)
                else:
                    # File - ensure parent exists (target already does) and override
                    if dst.exists():
                        if dst.is_dir():
                            shutil.rmtree(dst)
                        else:
                            dst.unlink()
                    shutil.copy2(item, dst)
        else:
            # If target is an existing directory, copy file into that directory
            if target.exists() and target.is_dir():
                dst = target / origin.name
            else:
                # Target is a file path - ensure its parent dir exists
                dst = target
                dst.parent.mkdir(parents=True, exist_ok=True)

                # Override if dst exists
                if dst.exists():
                    if dst.is_dir():
                        shutil.rmtree(dst)
                    else:
                        dst.unlink()

            shutil.copy2(origin, dst)

def action_build_target(info: ProjectInfo, target: str):
    subprocess.run(["kalamake", "--compile", f"{info.project.kmake}", f"release-{target}" ], check=True)
    subprocess.run(["kalamake", "--compile", f"{info.project.kmake}", f"debug-{target}" ], check=True)

    if target == "windows":
        action_copy_target(info.windows_post_build_copy)
    elif target == "windows-gnu":
        action_copy_target(info.windows_gnu_post_build_copy)
    else:
        action_copy_target(info.linux_post_build_copy)

def action_copy(info: ProjectInfo):
    def create_ext():
        target = Path(SCRIPT_DIR / "external")

        if target.exists():
            shutil.rmtree(target)

        target.mkdir(parents=True, exist_ok=True)

    if PLATFORM == "windows":
        create_ext()

        print("----------------------------------------")
        print("[ COPYING WINDOWS FILES ]")

        action_copy_target(info.windows_copy)
    else:
        create_ext()

        print("----------------------------------------")
        print("[ COPYING WINDOWS-GNU FILES ]")

        action_copy_target(info.windows_gnu_copy)

        print("----------------------------------------")
        print("[ COPYING LINUX FILES ]")

        action_copy_target(info.linux_copy)

    logging.info(f"Project '{info.project.name}' copy succeeded!")

def action_build(info: ProjectInfo, target="all"):
    print("----------------------------------------")
    print(f"[ BUILDING TARGET(S) '{target}' ]")

    if target == "all":
        if PLATFORM == "windows":
            action_build_target(info, "windows")
        else:
            action_build_target(info, "linux")
            action_build_target(info, "windows-gnu")
    elif target == "windows":
        if PLATFORM == "windows":
            action_build_target(info, "windows")
        else:
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
    elif target == "windows-gnu":
        if PLATFORM == "windows":
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
        else:
            action_build_target(info, "windows-gnu")
    else:
        if PLATFORM == "windows":
            logging.error(f"Failed to build because build target '{target}' cannot be used for platform '{PLATFORM}'!")
            sys.exit(1)
        else:
            action_build_target(info, "linux")

    logging.info(f"Project '{info.project.name}' target '{target}' build succeeded!")

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
        choices=["copy", "build", "all"])
    p.add_argument(
        "target",
        nargs="?",
        choices=["windows", "windows-gnu", "linux"])

    args = p.parse_args()

    info = action_verify()

    if args.action == "copy":
        if args.target:
            p.error("Action 'copy' does not allow to use target!")
        action_copy(info)
    else: 
        target = args.target or "all"
    
        if args.action == "build":
            action_build(info, target)
        else: 
            action_copy(info)
            action_build(info, target)

if __name__ == "__main__":
    main()
