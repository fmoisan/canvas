set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

# warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")

# no rtti
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")

# exceptions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

if (MSVC11)
	# fix std::tuple error from googletest usage
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_VARIADIC_MAX=10")
endif()
