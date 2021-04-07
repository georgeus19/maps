from os import listdir
from os.path import isfile, join
import glob
sources = glob.glob('./src/routing/**/*.cpp', recursive=True)
sources += glob.glob('./src/database/**/*.cpp', recursive=True)
sources += glob.glob('./src/utility/**/*.cpp', recursive=True)
print(sources)