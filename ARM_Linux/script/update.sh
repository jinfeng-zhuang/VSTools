#!/bin/sh

export P111TV=$1
padb cn
padb remount
padb push $2 /data
padb shell sync
padb dcn
