include(FetchContent)

# Might be used for worlds instead of whatever Stack uses
# Include zlib-ng
FetchContent_Declare(
        zlib_ng
        GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng
        GIT_TAG        6539b769e677bc1e6f4954d4457aba0cd138468b
)
FetchContent_GetProperties(zlib_ng)
if(NOT zlib_ng_POPULATED)
  FetchContent_Populate(zlib_ng)

  # Set custom variables, policies, etc.
  set(ZLIB_ENABLE_TESTS OFF)
  set(WITH_NATIVE_INSTRUCTIONS ON)

  add_subdirectory(${zlib_ng_SOURCE_DIR} ${zlib_ng_BINARY_DIR})
endif()

# Formatting
# Include fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY  https://github.com/fmtlib/fmt
    GIT_TAG         7.1.3
)
FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
    FetchContent_Populate(fmt)

    # Disable tests
    set(FMT_TEST OFF)

    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()