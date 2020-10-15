#!/bin/sh

export P111TV=10.86.79.108
padb cn
padb remount
padb pull /data/dump-002.pts ./
padb shell sync
padb dcn
