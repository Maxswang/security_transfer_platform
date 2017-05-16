set(CODEC_SRCS
rpc.pb.cc
proto_msg_dispatcher.cc
proto_msg_serialization.cc
)	                                                                                                                                                                            
add_library(codec ${CODEC_SRCS})
target_link_libraries(codec pthread rt glog.a event.a protobuf.a)		                                                                                                                                                                            
install(TARGETS codec DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/codec)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
