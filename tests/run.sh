#!/usr/bin/env bash

SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname ${SCRIPT}`
PROJECT_PATH=`dirname ${SCRIPT_PATH}`
TESTS_PATH=`realpath $1`
cat "${TESTS_PATH}" | xargs -P16 -I {} "${PROJECT_PATH}/build/shinobu" --rom {}
