add_executable(atomic_test atomic_test.cc)
target_link_libraries(atomic_test stpcomm)

add_executable(mutex_test mutex_test.cc)
target_link_libraries(mutex_test stpcomm)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test stpcomm)

add_executable(tcp_server_test tcp_server_test.cc)
target_link_libraries(tcp_server_test stpcomm)


