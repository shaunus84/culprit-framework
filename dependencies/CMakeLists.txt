include(FetchContent)

# googletest
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)


add_subdirectory(googletest)