set(STP_CRYPTO_SRCS
stp_crypto.cc
)	                                                                                                                                                                            
add_library(stpcrypto ${STP_CRYPTO_SRCS})
#target_link_libraries(stpinterface pthread rt glog.a event.a)		                                                                                                                                                                            
install(TARGETS stpcrypto DESTINATION lib)
file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/stpinterface)		                                                                                                                                                                            
if(NOT CMAKE_BUILD_NO_EXAMPLES)
 add_subdirectory(tests)
endif()    
