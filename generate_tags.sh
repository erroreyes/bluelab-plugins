#!/bin/sh

find . -type f \( -iname \*.c -o -iname \*.h -o -iname \*.cpp -o -iname \*.hpp \) | etags -
