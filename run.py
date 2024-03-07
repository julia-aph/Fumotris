import os, sys

source_files = []

subdirs = []

for dirpath, dirnames, filenames in os.walk("source\\"):
    source_files += [os.path.join(dirpath, f) for f in filenames if f.endswith(".c")]
    subdirs.append(dirpath)

os.system(f"gcc {' '.join(source_files)} -I {' -I '.join(subdirs)} -o {sys.argv[1]} -pthread -Wall")