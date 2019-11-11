##########################
#                        #
#       ABALONE-PC       #
#                        #
# (C)1993 Colin Springer #
#  All Rights Reserved   #
##########################


c_dir = ..\c
include_dir = ..\h

wcc_options = /mc /w3 /zq /oaxt /d1 /2 /zt /i=$(include_dir)
wlink_options = option map=ab debug line

object_files = ab.obj &
               data.obj &
               eval.obj &
               feature.obj &
               init.obj &
               intrface.obj &
               move.obj &
               search.obj &
	       sysdept.obj &
               tables.obj &
	       vga.obj

..\bin\ab.exe : $(object_files)
               wlink $(wlink_options) @$^&

.c:            $(c_dir)
.c.obj:
               wcc $[* $(wcc_options)

ab.obj       : $(c_dir)\ab.c
data.obj     : $(c_dir)\data.c
eval.obj     : $(c_dir)\eval.c
feature.obj  : $(c_dir)\feature.c
init.obj     : $(c_dir)\init.c
intrface.obj : $(c_dir)\intrface.c
move.obj     : $(c_dir)\move.c
search.obj   : $(c_dir)\search.c
sysdept.obj  : $(c_dir)\sysdept.c
tables.obj   : $(c_dir)\tables.c
vga.obj      : $(c_dir)\vga.c

