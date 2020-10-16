# PATCH

* metz_metadata.patch

    参考 [Wiki: Metadata 延迟调试]

    因为 HOST 那边 Calc 调度得不及时，导致 Metadata 解码速度跟不上视频解码速度
    通过增加 Metadata HOST 端缓存的方式尝试解决该问题

* malone_mjpeg_support_on_sx8b.patch

    从 SX7 到 SX8 移植过程中，设计 MJPEG 相关的代码没有过来。

* monitor_es_buffer.patch

    顾名思义。