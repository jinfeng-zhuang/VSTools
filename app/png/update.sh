#!/bin/sh

export P111TV=$1
padb cn
padb remount
padb push zhuang /bin
padb shell sync
padb dcn
