import re
import subprocess
from distutils.core import setup, Extension

def pkg_config(package, option):
    sub = subprocess.Popen(["pkg-config",option] + package.split(),
                           stdout=subprocess.PIPE)
    spaces = re.compile('\s+',re.DOTALL)
    data = sub.stdout.read().strip().replace("-I", "").replace("-L", "")
    args = spaces.split(data)
    sub.stdout.close()
    sub.wait()
    return args


extra_includes = pkg_config("cairo libpng", "--cflags-only-I")

module1 = Extension("pychq", 
            include_dirs = ['..'] + extra_includes,
            library_dirs = ['..'],
            libraries = ["cairo", "png", "chartesque"],
            sources = ["pychq.c"])

setup (name = "pychq",
       version = "0.1.1",
       description = "Wrapper module around the chartesque library.",
       ext_modules = [module1])

