Use cygwin to compiler your source sode on Windows

--------------------------------------------------Shell script content start
#!d:\cygwin\bin\bash --login 
set -ex
cd /cygdrive/c/Users/Victor/.jenkins/workspace/TEST
pwd
make
--------------------------------------------------Shell script content end