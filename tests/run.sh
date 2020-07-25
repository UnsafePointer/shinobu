#!/usr/bin/env bash

SCRIPT=`realpath $0`
SCRIPT_PATH=`dirname ${SCRIPT}`
echo $SCRIPT_PATH
PROJECT_PATH=`dirname ${SCRIPT_PATH}`
cat "${SCRIPT_PATH}/test-locations.txt" | xargs -P10 -I {} "${PROJECT_PATH}/build/shinobu" --rom {}
