rm -rf CMakeCache.txt CMakeFiles >/dev/null 2>&1
cmake -G "Unix Makefiles" ../../src && make -j4
