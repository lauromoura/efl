#!/bin/sh

echo $LD_LIBRARY_PATH
MONO_LOG_LEVEL=debug $MONO $MONO_BUILDPATH/src/tests/eo_mono/eo_mono.exe

