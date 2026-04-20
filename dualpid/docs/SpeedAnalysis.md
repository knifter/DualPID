## dead sensor, sPWM output
src/main.cpp.loop: time1_event:    3
src/main.cpp.loop: time2_guihandle:1
src/main.cpp.loop: time3_guiloop:  19
src/main.cpp.loop: time4_setman:   1
src/main.cpp.loop: time5_pidloops: 5
src/main.cpp.loop: time6_cli:      14
src/main.cpp.loop: 43 us total. 20688 loops per sec. 48 us per loop: 5 extra

## sht31 enabled, sPWM
src/main.cpp.loop: time1_event:    3
src/main.cpp.loop: time2_guihandle:1
src/main.cpp.loop: time3_guiloop:  18
src/main.cpp.loop: time4_setman:   2
src/main.cpp.loop: time5_pidloops: 6
src/main.cpp.loop: time6_cli:      14
src/main.cpp.loop: 44 us total. 20430 loops per sec. 48 us per loop: 4 extra

## comment lv_timer_handler() & cli_loop()
src/main.cpp.loop: time1_event:    2
src/main.cpp.loop: time2_guihandle:1
src/main.cpp.loop: time3_guiloop:  3
src/main.cpp.loop: time4_setman:   1
src/main.cpp.loop: time5_pidloops: 5
src/main.cpp.loop: time6_cli:      0
src/main.cpp.loop: 12 us total. 60211 loops per sec. 16 us per loop: 4 extra

