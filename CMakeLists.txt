cmake_minimum_required(VERSION 3.10.2)
project(gz_attach_links)

find_package(ament_cmake QUIET)

ament_environment_hooks("hooks/resource_paths.dsv.in")

# Harmonic
if("$ENV{GZ_VERSION}" STREQUAL "harmonic")
  set(GZ_TARGET_PREFIX gz)

  set(GZ_PLUGIN plugin2)
  set(GZ_COMMON common5)
  set(GZ_SIM sim8)

  message(STATUS "Compiling against Gazebo Harmonic")

# Garden
elseif("$ENV{GZ_VERSION}" STREQUAL "garden")
  set(GZ_TARGET_PREFIX gz)

  set(GZ_PLUGIN plugin2)
  set(GZ_COMMON common5)
  set(GZ_SIM sim7)

  message(STATUS "Compiling against Gazebo Garden")

# Default to Fortress
else()
  set(GZ_TARGET_PREFIX ignition)

  set(GZ_PLUGIN plugin1)
  set(GZ_COMMON common4)
  set(GZ_SIM gazebo6)

  add_definitions(-DFOR_IGNITION_FORTRESS)
  message(STATUS "Compiling against Ignition Fortress")
endif()

find_package(${GZ_TARGET_PREFIX}-${GZ_PLUGIN})
find_package(${GZ_TARGET_PREFIX}-${GZ_SIM})
find_package(${GZ_TARGET_PREFIX}-${GZ_COMMON})

add_definitions(-DGZ_NS=${GZ_TARGET_PREFIX})

add_library(attach-links SHARED src/attach_links/AttachLinks.cc)
target_include_directories(attach-links PRIVATE
    ${GZ_TARGET_PREFIX}/${GZ_PLUGIN}
    ${GZ_TARGET_PREFIX}/${GZ_SIM}
    ${GZ_TARGET_PREFIX}/${GZ_COMMON}
    )

target_link_libraries(attach-links
  ${GZ_TARGET_PREFIX}-${GZ_PLUGIN}::core
  ${GZ_TARGET_PREFIX}-${GZ_SIM}::core
  ${GZ_TARGET_PREFIX}-${GZ_COMMON}::core
)

install(DIRECTORY launch models DESTINATION share/${PROJECT_NAME})
install(TARGETS attach-links DESTINATION lib/${PROJECT_NAME})

ament_package()
