#!/bin/bash

# Update example patchs (example/pd folder) to projects datas.
DIRLIST="PofDroid exampleStandalone iPof"

for thisdir in $DIRLIST ; do
	rm -f -r $thisdir/bin/data/pd
	cp -r example/pd $thisdir/bin/data/
done

