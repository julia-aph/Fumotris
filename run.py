import os, sys
import json, hashlib


def walk_source():
    source_files = []
    subdirs = []

    for dirpath, dirnames, filenames in os.walk("source\\"):
        source_files += [os.path.join(dirpath, f) for f in filenames if f.endswith(".c")]
        subdirs.append(dirpath)

    return (source_files, subdirs)


def get_checksums(source_files):
    checksums = {}

    for path in source_files:
        with open(path, "rb") as source_file:
            source = source_file.read()

            checksum = hashlib.md5(source).hexdigest()
            checksums[path] = checksum 

    return checksums


def read_checksum_file():
    checksums = {}

    if not os.path.exists("checksums.txt"):
        return checksums

    with open("checksums.txt", "rb") as checksum_file:
        checksums = json.loads(checksum_file.read())

    return checksums


def write_checksum_file(checksums):
    with open("checksums.txt", "w+") as checksum_file:
        checksum_file.write(json.dumps(checksums))


def build():
    source_files, subdirs = walk_source()

    checksums_before = read_checksum_file()
    checksums_now = get_checksums(source_files)

    compile_list = []

    for path in checksums_now:
        if not path in checksums_before or checksums_before[path] != checksums_now[path]:
            compile_list.append(path)

    write_checksum_file(checksums_now)

    for path in compile_list:
        name = os.path.splitext(os.path.basename(path))[0]

        os.system(f"gcc -c {path} -I {' -I '.join(subdirs)} -o objects/{name}.o -pthread -Wall")

    os.system(f"gcc objects\\*.o -o {sys.argv[1]} -pthread -Wall")


build()