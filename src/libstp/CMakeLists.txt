set(LIBSTP_SRCS
mutex.cc
thread.cc 
tcp_event_server.cc
)	                                                                                                                                                                            
add_library(stp ${LIBSTP_SRCS})
target_link_libraries(stp pthread rt glog.a event.a)		                                                                                                                                                                            
install(TARGETS stp DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stp)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
