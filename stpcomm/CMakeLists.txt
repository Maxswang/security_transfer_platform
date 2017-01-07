set(STPCOMM_SRCS
mutex.cc
thread.cc 
connection.cc
libevent_thread.cc
event_notifier.cc
tcp_event_client.cc
tcp_event_server.cc
)	                                                                                                                                                                            
add_library(stpcomm ${STPCOMM_SRCS})
target_link_libraries(stpcomm pthread rt glog.a event.a)		                                                                                                                                                                            
install(TARGETS stpcomm DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpcomm)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
