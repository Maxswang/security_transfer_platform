set(STPINTERFACE_SRCS
stp_interface.cc
)	                                                                                                                                                                            
add_library(stpinterface ${STPINTERFACE_SRCS})
#target_link_libraries(stpinterface pthread rt glog.a event.a)		                                                                                                                                                                            
install(TARGETS stpinterface DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpinterface)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
