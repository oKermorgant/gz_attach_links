cmake_minimum_required(VERSION 3.10.2)
project(gz_attach_links)

find_package(ament_cmake QUIET)

ament_environment_hooks("hooks/resource_paths.dsv.in")

# Harmonic
if("$ENV{GZ_VERSION}" STREQUAL "harmonic")
  find_package(gz-transport13 REQUIRED)
  find_package(gz-msgs10 REQUIRED)
  find_package(sdformat14 REQUIRED)
  set(SDF_VER ${sdformat14_VERSION_MAJOR})
  set(GZ_TARGET_PREFIX gz)
  set(GZ_MSGS_VER ${gz-msgs10_VERSION_MAJOR})
  set(GZ_TRANSPORT_VER ${gz-transport13_VERSION_MAJOR})
  set(GZ_SIM_PATH gz-sim8)
  message(STATUS "Compiling against Gazebo Harmonic")

# Garden
elseif("$ENV{GZ_VERSION}" STREQUAL "garden")
  find_package(gz-transport12 REQUIRED)
  find_package(gz-msgs9 REQUIRED)
  find_package(sdformat13 REQUIRED)
  set(SDF_VER ${sdformat13_VERSION_MAJOR})
  set(GZ_TARGET_PREFIX gz)
  set(GZ_MSGS_VER ${gz-msgs9_VERSION_MAJOR})
  set(GZ_TRANSPORT_VER ${gz-transport12_VERSION_MAJOR})
  set(GZ_SIM_PATH gz-sim7)
  message(STATUS "Compiling against Gazebo Garden")

# Default to Fortress
else()
  find_package(ignition-transport11 REQUIRED)
  find_package(ignition-msgs8 REQUIRED)
  find_package(sdformat12 REQUIRED)
  set(SDF_VER ${sdformat12_VERSION_MAJOR})
  set(GZ_TARGET_PREFIX ignition)
  set(GZ_MSGS_VER ${ignition-msgs8_VERSION_MAJOR})
  set(GZ_TRANSPORT_VER ${ignition-transport11_VERSION_MAJOR})
  set(GZ_SIM_PATH ignition-gazebo6)
  add_definitions(-DGZ_FORTRESS)
  message(STATUS "Compiling against Ignition Fortress")
endif()

add_definitions(-DGZ_NS=${GZ_TARGET_PREFIX})

set(GZ_MSGS_VERSION_MAJOR ${${GZ_TARGET_PREFIX}-msgs${GZ_MSGS_VER}_VERSION_MAJOR})
set(GZ_MSGS_VERSION_MINOR ${${GZ_TARGET_PREFIX}-msgs${GZ_MSGS_VER}_VERSION_MINOR})
set(GZ_MSGS_VERSION_PATCH ${${GZ_TARGET_PREFIX}-msgs${GZ_MSGS_VER}_VERSION_PATCH})
set(GZ_MSGS_VERSION_FULL ${GZ_MSGS_VERSION_MAJOR}.${GZ_MSGS_VERSION_MINOR}.${GZ_MSGS_VERSION_PATCH})

add_library(attach-links src/attach_links/AttachLinks.cc)
target_include_directories(attach-links PRIVATE
    ${GZ_TARGET_PREFIX}/sdformat${SDF_VER}
    ${GZ_TARGET_PREFIX}/msgs${GZ_MSGS_VER}
    ${GZ_TARGET_PREFIX}/transport${GZ_TRANSPORT_VER}
    )

target_link_libraries(attach-links
  ${GZ_TARGET_PREFIX}-msgs${GZ_MSGS_VER}::core
  ${GZ_TARGET_PREFIX}-transport${GZ_TRANSPORT_VER}::core
  sdformat${SDF_VER}::sdformat${SDF_VER}
)

install(TARGETS attach-links DESTINATION lib/${PROJECT_NAME})


ament_package()
