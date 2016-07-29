#!/bin/bash

rsync -avvvr --dry-run --existing "$1"/src `pwd`/src 
