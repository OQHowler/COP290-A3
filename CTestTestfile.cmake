# CMake generated Testfile for 
# Source directory: /home/shoubhit/COP290A3/leveldb
# Build directory: /home/shoubhit/COP290A3/leveldb
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[leveldb_tests]=] "/home/shoubhit/COP290A3/leveldb/leveldb_tests")
set_tests_properties([=[leveldb_tests]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;365;add_test;/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;0;")
add_test([=[c_test]=] "/home/shoubhit/COP290A3/leveldb/c_test")
set_tests_properties([=[c_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;391;add_test;/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;394;leveldb_test;/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;0;")
add_test([=[env_posix_test]=] "/home/shoubhit/COP290A3/leveldb/env_posix_test")
set_tests_properties([=[env_posix_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;391;add_test;/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;402;leveldb_test;/home/shoubhit/COP290A3/leveldb/CMakeLists.txt;0;")
subdirs("third_party/googletest")
subdirs("third_party/benchmark")
