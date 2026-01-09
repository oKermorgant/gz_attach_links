cmake_minimum_required(VERSION 3.10.2)
project(gz_attach_links)

find_package(ament_cmake QUIET)

ament_environment_hooks("hooks/resource_paths.dsv.in")

# Gazebo version, use env variable or ROS 2 official pairing for LTS
# https://gazebosim.org/docs/latest/ros_installation/
if(DEFINED ENV{GZ_VERSION})
  set(GZ_VERSION "$ENV{GZ_VERSION}")
elseif(DEFINED ENV{IGN_VERSION})
  set(GZ_VERSION "$ENV{IGN_VERSION}")
elseif("$ENV{ROS_DISTRO}" STRGREATER_EQUAL "lyrical")
  set(GZ_VERSION "jetty")
elseif("$ENV{ROS_DISTRO}" STRGREATER_EQUAL "jazzy")
  set(GZ_VERSION "harmonic")
else()
  set(GZ_VERSION "fortress")
endif()

message(STATUS "Compiling against Gazebo ${GZ_VERSION}")
set(GZ_TARGET_PREFIX gz)

if("${GZ_VERSION}" STREQUAL "jetty")
  set(GZ_PLUGIN plugin4)
  set(GZ_COMMON common7)
  set(GZ_SIM sim10)

elseif("${GZ_VERSION}" STREQUAL "harmonic")
  set(GZ_PLUGIN plugin2)
  set(GZ_COMMON common5)
  set(GZ_SIM sim8)

elseif("${GZ_VERSION}" STREQUAL "garden")
  set(GZ_PLUGIN plugin2)
  set(GZ_COMMON common5)
  set(GZ_SIM sim7)

else()
  set(GZ_TARGET_PREFIX ignition)

  set(GZ_PLUGIN plugin1)
  set(GZ_COMMON common4)
  set(GZ_SIM gazebo6)
  add_definitions(-DFOR_IGNITION_FORTRESS)
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
