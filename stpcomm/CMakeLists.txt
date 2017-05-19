set(STPCOMM_SRCS
stp_token.cc
)	                                                                                                                                                                            
add_library(stpcomm ${STPCOMM_SRCS})
target_link_libraries(stpcomm pthread rt glog.a event.a)		                                                                                                                                                                            
install(TARGETS stpcomm DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpcomm)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
