#!/bin/sh

export P111TV=10.86.79.108
padb cn
padb remount
padb push ./vesdump /data
padb shell sync
padb dcn
