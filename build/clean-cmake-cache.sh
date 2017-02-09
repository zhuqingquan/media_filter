!bash

echo "rm cmake cache begin..."
rm ./cmake_install.cmake
rm -r ./CMakeFiles
rm ./Makefile
rm ./CMakeCache.txt
rm ./media_filter_cfg.h
rm lib*.a
echo "rm cmake cache end..."
