#!/usr/bin/env bash

SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname ${SCRIPT}`
PROJECT_PATH=`dirname ${SCRIPT_PATH}`
TESTS_PATH=`realpath $1`
echo $TESTS_PATH
cat "${TESTS_PATH}" | xargs -P10 -I {} "${PROJECT_PATH}/build/shinobu" --rom {}
