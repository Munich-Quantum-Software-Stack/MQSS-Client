include(FetchContent)

set(ENABLED_MODULES "bull")
set(BUILD_BACKEND_TESTS OFF)

FetchContent_Declare(
    bull_nisq_compiler
    GIT_REPOSITORY https://github.com/ArnaudAtos/MQSS-QDMI-Devices-Suite.git
    GIT_TAG develop
)

FetchContent_MakeAvailable(bull_nisq_compiler)

FetchContent_GetProperties(bull_nisq_compiler)

