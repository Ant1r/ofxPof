#!/bin/bash

# Update example patchs (example/pd folder) to PofDroid and exampleStandAlone datas.

rm -f -r PofDroid/bin/data/pd
rm -f -r exampleStandalone/bin/data/pd
cp -r example/pd PofDroid/bin/data/
cp -r example/pd exampleStandalone/bin/data

