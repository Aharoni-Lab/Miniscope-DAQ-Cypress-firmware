#!/bin/bash

cd "$(dirname "$0")"
set -e

source_files=`find Miniscope_DAQ/ -type f \( -name '*.c' -o -name '*.h' \) | grep -v 'Miniscope_DAQ/cyfxtx.c'`

clang-format -i $source_files
