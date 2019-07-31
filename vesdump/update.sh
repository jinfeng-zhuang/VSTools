#!/bin/sh

ADB=padb

export P111TV=10.86.79.109

$ADB cn
$ADB remount
$ADB push ./avtool /data
$ADB shell sync
$ADB dcn
