adb connect $1
adb root
adb remount
adb push ./avtool /bin/avtool
adb shell sync
adb disconnect
