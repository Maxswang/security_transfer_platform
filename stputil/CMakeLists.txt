set(STPUTIL_SRCS
json/jsoncpp.cc
json_parser.cc
mutex.cc
thread.cc 
connection.cc
event_notifier.cc
tcp_event_client.cc
tcp_event_server.cc
security/aes_core.c
security/md5.cpp
security/base64.cpp
shm.cc
mysql_connection_pool.cc
)	                                                                                                                                                                            
add_library(stputil ${STPUTIL_SRCS})
target_link_libraries(stputil pthread rt glog.a event.a /usr/lib64/mysql/libmysqlclient.so dl boost_system)		                                                                                                                                                                            
install(TARGETS stputil DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpcomm)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
