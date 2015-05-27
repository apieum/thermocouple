#!/usr/bin/env python2
# -*- coding: utf-8; -*-

"""
Ino is a command-line toolkit for working with Arduino hardware.

It is intended to replace Arduino IDE UI for those who prefer to work in
terminal or want to integrate Arduino development in a 3rd party IDE.

Ino can build sketches, libraries, upload firmwares, establish
serial-communication. For this it is split in a bunch of subcommands, like git
or mercurial do. The full list is provided below. You may run any of them with
--help to get further help. E.g.:

    ino build --help
"""

import sys
import os.path, os
import argparse

from ino.commands import Init, Preprocess, Build as OldBuild, Clean, Upload, Serial, ListModels

from ino.utils import SpaceList, list_subdirs
from ino.conf import configure
from ino.exc import Abort
from ino.filters import colorize
from ino.environment import Environment
from ino.argparsing import FlexiFormatter


base_dir = os.path.realpath(os.getcwd())
environment = Environment()
environment.load()
# Override source directory name
# environment.src_dir = 'src'  # default 'src
environment.extra_libs = [os.path.join(base_dir, environment.src_dir)]
environment.build_base = 'build'
environment.output_dir = os.path.join(environment.build_base, environment.src_dir)

class CleanAll(Clean):
    def __init__(self, environment):
        environment.output_dir = environment.build_base
        Clean.__init__(self, environment)


class Build(OldBuild):
    def scan_dependencies(self):
        src_dir = os.path.realpath(self.e.src_dir)
        self.e['deps'] = SpaceList()

        lib_dirs = [self.e.arduino_core_dir, src_dir] + list_subdirs(self.e.lib_dir) + list_subdirs(self.e.arduino_libraries_dir)
        lib_dirs.extend(self.e.extra_libs)
        inc_flags = self.recursive_inc_lib_flags(lib_dirs)

        # If lib A depends on lib B it have to appear before B in final
        # list so that linker could link all together correctly
        # but order of `_scan_dependencies` is not defined, so...

        # 1. Get dependencies of sources in arbitrary order
        used_libs = list(self._scan_dependencies(self.e.src_dir, lib_dirs, inc_flags))
        # used_libs.append(src_dir)

        # 2. Get dependencies of dependency libs themselves: existing dependencies
        # are moved to the end of list maintaining order, new dependencies are appended
        scanned_libs = set()
        while scanned_libs != set(used_libs):
            for lib in set(used_libs) - scanned_libs:
                dep_libs = self._scan_dependencies(lib, lib_dirs, inc_flags)

                i = 0
                for ulib in used_libs[:]:
                    if ulib in dep_libs:
                        # dependency lib used already, move it to the tail
                        used_libs.append(used_libs.pop(i))
                        dep_libs.remove(ulib)
                    else:
                        i += 1

                # append new dependencies to the tail
                used_libs.extend(dep_libs)
                scanned_libs.add(lib)


        self.e['used_libs'] = used_libs
        self.e['cppflags'].extend(self.recursive_inc_lib_flags(used_libs))



class BuildTests(Build):
    name = 'build-tests'
    help_line = "Build firmware from the tests directory project"
    def __init__(self, environment):
        environment.src_dir = 'tests'
        environment.output_dir = os.path.join(environment.build_base, environment.src_dir)
        Build.__init__(self, environment)

class UploadTests(Upload):
    name = 'upload-tests'
    help_line = "Upload firmware from the tests directory project"
    def __init__(self, environment):
        environment.src_dir = 'tests'
        environment.output_dir = os.path.join(environment.build_base, environment.src_dir)
        Upload.__init__(self, environment)


commands = {
    'init': Init,
    'preproc': Preprocess,
    'build': Build,
    'clean': CleanAll,
    'upload': Upload,
    'serial': Serial,
    'list-models': ListModels,
    'upload-tests': UploadTests,
    'build-tests': BuildTests
}


def build_args(environment):
    parser = argparse.ArgumentParser(prog='ino', formatter_class=FlexiFormatter, description=__doc__)
    subparsers = parser.add_subparsers()
    config = configure()

    if len(sys.argv) > 1 and sys.argv[1] in commands:
        cmd_name = sys.argv[1]
        cmd = commands[cmd_name](environment)
        subparser = subparsers.add_parser(cmd.name, formatter_class=FlexiFormatter, help=cmd.help_line)
        cmd.setup_arg_parser(subparser)
        subparser.set_defaults(func=cmd.run, **config.as_dict(cmd.name))
    else:
        cmd_name = None
        for cmd in tuple(commands.itervalues()):
            subparsers.add_parser(cmd.name, formatter_class=FlexiFormatter, help=cmd.help_line)

    return cmd_name, parser.parse_args()

def sanitize_dirs(environment):
    if not os.path.isdir(environment.src_dir):
        raise Abort("Sources directory '%s' not exists!" % environment.src_dir)

    if not os.path.isdir(environment.build_dir):
        os.makedirs(environment.build_dir)

    if not os.path.isdir(environment.lib_dir):
        os.makedirs(environment.lib_dir)
        open('lib/.holder', 'w').close()


if __name__ == "__main__":
    cmd_name, args = build_args(environment)

    try:
        environment.process_args(args)

        if cmd_name in ("preprocess", "build", "upload", "build-tests"):
            sanitize_dirs(environment)

        args.func(args)
    except Abort as exc:
        print(colorize(str(exc), 'red'))
        sys.exit(1)
    except KeyboardInterrupt:
        print('Terminated by user')
    finally:
        environment.dump()
