add_executable(culprit-framework-tests
				src/main.cpp
				src/FrameworkTests.cpp)

target_link_libraries(culprit-framework-tests PRIVATE gtest_main culprit-framework)