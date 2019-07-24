#!/bin/sh

ADB=padb

export P111TV=10.86.79.94

$ADB cn
$ADB remount
$ADB push ./printmsg /data
$ADB shell sync
$ADB dcn
