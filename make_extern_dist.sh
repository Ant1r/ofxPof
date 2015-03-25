#!/bin/bash

# Build a dist package containing pof external binary, help files and example patchs.

rm pof.zip
cd help
zip -r ../pof.zip *
cd ..
zip -r pof.zip example
zip -j pof.zip buildExternal/bin/pof.*

