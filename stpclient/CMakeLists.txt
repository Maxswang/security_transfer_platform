set(STPCLNT_SRCS
config_parser.cc
stpclnt_msg_dispatcher.cc
stp_client.cc
main.cc
)	 
ADD_EXECUTABLE(stpclient ${STPCLNT_SRCS})                                                                                                                                                                           

target_link_libraries(stpclient pthread rt glog.a event.a stputil codec)                                                                                                                              
	                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    


