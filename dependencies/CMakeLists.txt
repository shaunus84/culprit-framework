include(FetchContent)

# googletest
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)


add_subdirectory(googletest)