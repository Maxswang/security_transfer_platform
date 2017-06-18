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
mysql/connection.cc
mysql/connection_pool.cc
mysql/statement.cc
mysql/resultset.cc
)	                                                                                                                                                                            
add_library(stputil ${STPUTIL_SRCS})
target_link_libraries(stputil pthread rt glog.a event.a /usr/lib64/mysql/libmysqlclient.a dl boost_system dl )		                                                                                                                                                                            
install(TARGETS stputil DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpcomm)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
