#!/usr/bin/env bash
set -eu -o pipefail

find $MESON_DIST_ROOT \( -name *.h -o -name *.c \) -exec sed -i '1i \
/************************************\
 * Matricola\
 * Nome e cognome\
 * Data di realizzazione\
 ************************************/' {} \+
