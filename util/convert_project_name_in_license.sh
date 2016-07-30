#!/bin/bash

find . -path ./.git -prune -o -type f -exec sed -i 's/Sherwin'\''s Adventure/GBA Project Template/g' {} +
