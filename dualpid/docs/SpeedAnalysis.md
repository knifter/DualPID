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

## lv_timer_handler on, Output task removed, running, with graph
src/main.cpp.loop: time1_event:    2
src/main.cpp.loop: time2_guihandle:1
src/main.cpp.loop: time3_guiloop:  37
src/main.cpp.loop: time4_setman:   1
src/main.cpp.loop: time5_pidloops: 6
src/main.cpp.loop: time6_cli:      0
src/main.cpp.loop: 47 us total. 19080 loops per sec. 52 us per loop: 5 extra

## same but with fixed output on
src/main.cpp.loop: time1_event:    2
src/main.cpp.loop: time2_guihandle:1
src/main.cpp.loop: time3_guiloop:  53
src/main.cpp.loop: time4_setman:   1
src/main.cpp.loop: time5_pidloops: 8
src/main.cpp.loop: time6_cli:      0
src/main.cpp.loop: 65 us total. 14679 loops per sec. 68 us per loop: 3 extra

## Moving the menu's
src/main.cpp.loop: time1_event:    3
src/main.cpp.loop: time2_guihandle:111
src/main.cpp.loop: time3_guiloop:  3132
src/main.cpp.loop: time4_setman:   1
src/main.cpp.loop: time5_pidloops: 77
src/main.cpp.loop: time6_cli:      0
src/main.cpp.loop: 3324 us total. 302 loops per sec. 3307 us per loop: -17 extra

src/main.cpp.loop: time1_event:    23
src/main.cpp.loop: time2_guihandle:6246
src/main.cpp.loop: time3_guiloop:  178325
src/main.cpp.loop: time4_setman:   7
src/main.cpp.loop: time5_pidloops: 4025
src/main.cpp.loop: time6_cli:      1
src/main.cpp.loop: 188627 us total. 5 loops per sec. 187500 us per loop: -1127 extra

