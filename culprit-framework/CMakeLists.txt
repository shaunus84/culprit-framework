
add_library(culprit-framework STATIC

			include/culprit-framework/CommandBase.h
			include/culprit-framework/ContextBase.h
			include/culprit-framework/Creator.hpp
			include/culprit-framework/CulpritFramework.h
			include/culprit-framework/IUpdatable.h
			include/culprit-framework/Notifier.hpp
			include/culprit-framework/Signal.hpp
			include/culprit-framework/SignalResponder.h
			include/culprit-framework/Signals.h
			include/culprit-framework/UniqueKeyGenerator.h

			src/CommandBase.cpp
			src/ContextBase.cpp
			src/SignalResponder.cpp)
			
target_include_directories(culprit-framework
	PUBLIC
	$<INSTALL_INTERFACE:include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(culprit-framework PRIVATE cxx_std_17)

# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	# Run macro
	GroupSources(culprit-framework)

# Install target + headers
install(TARGETS culprit-framework
    EXPORT CulpritFrameworkTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# Export the target (NO config file needed if you're not using find_package)
install(EXPORT CulpritFrameworkTargets
    FILE CulpritFrameworkTargets.cmake
    NAMESPACE culprit::
    DESTINATION lib/cmake/CulpritFramework
)

# === CMake Package Config ===
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritFrameworkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CulpritFrameworkConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritFrameworkConfig.cmake"
    INSTALL_DESTINATION lib/cmake/CulpritFramework
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritFrameworkConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CulpritFrameworkConfigVersion.cmake"
    DESTINATION lib/cmake/CulpritFramework
)
