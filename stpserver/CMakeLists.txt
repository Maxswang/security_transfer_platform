set(STPSVR_SRCS
config_parser.cc
stpsvr_msg_dispatcher.cc
stp_server.cc
main.cc
)	 
ADD_EXECUTABLE(stpserver ${STPSVR_SRCS})                                                                                                                                                                           

target_link_libraries(stpserver pthread rt glog.a event.a stpcomm codec)                                                                                                                              
	                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    


