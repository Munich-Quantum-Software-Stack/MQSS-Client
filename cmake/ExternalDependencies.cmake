include(FetchContent)
set(FETCH_PACKAGES "")

#if(BUILD_DOCUMENTATION)
  set(CMAKE_POLICY_DEFAULT_CMP0116
      NEW
      CACHE STRING
            "Set the default CMP0116 policy to NEW for documentation builds")
  set(DOXYGEN_VERSION
      1.15.0
      CACHE STRING "Doxygen version")
  set(DOXYGEN_REV
      "7cca38ba5185457e6d9495bf963d4cdeacebc25a"
      CACHE STRING "Doxygen identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    Doxygen
    GIT_REPOSITORY https://github.com/doxygen/doxygen.git
    GIT_TAG ${DOXYGEN_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_VERSION})
  list(APPEND FETCH_PACKAGES Doxygen)

  set(DOXYGEN_AWESOME_VERSION
      2.4.1
      CACHE STRING "Doxygen Awesome version")
  set(DOXYGEN_AWESOME_REV
      "1f3620084ff75734ed192101acf40e9dff01d848"
      CACHE STRING "Doxygen Awesome identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG ${DOXYGEN_AWESOME_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_AWESOME_VERSION})
  list(APPEND FETCH_PACKAGES doxygen-awesome-css)
#endif()

if(FETCH_PACKAGES)
  FetchContent_MakeAvailable(${FETCH_PACKAGES})
endif()
