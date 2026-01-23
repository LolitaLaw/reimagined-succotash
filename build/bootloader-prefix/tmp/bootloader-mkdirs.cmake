# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/esp/v4.4.3/esp-idf/components/bootloader/subproject"
  "D:/ost/sample_project/build/bootloader"
  "D:/ost/sample_project/build/bootloader-prefix"
  "D:/ost/sample_project/build/bootloader-prefix/tmp"
  "D:/ost/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "D:/ost/sample_project/build/bootloader-prefix/src"
  "D:/ost/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/ost/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
