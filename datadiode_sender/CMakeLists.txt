cmake_minimum_required (VERSION 3.14)
project (DATADIODE_SENDER VERSION 0.1.0)

cmake_policy(SET CMP0115 NEW)

add_executable(datadiode_sender src/DataDiode_Sender.cpp)
target_include_directories(datadiode_sender
  PRIVATE
  src
)
target_sources(datadiode_sender
  PRIVATE
  src/DataDiode_SenderControl.cpp
  src/i2c/i2c.cpp
)

# Copy script which is responsible to set device's IP address upon profinet controller request to build tree.
# Note that this is only necessary if one wants to run the build files. The install files search at the correct position.
file (COPY ${CMAKE_INSTALL_PREFIX}/bin/profipp_network_parameters DESTINATION ${DATADIODE_SENDER_BINARY_DIR}/ FILE_PERMISSIONS WORLD_EXECUTE)

target_compile_features(datadiode_sender PRIVATE cxx_std_17)



# Add i2c slave libary
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
find_package(pigpio)
if (PIGPIO_FOUND)
    message("pigpio library found.")
else()
    message("pigpio library not found. You may need to specify its location.")
endif()

# Add thread support (needed by profi++)
find_package(Threads REQUIRED)
target_link_libraries(datadiode_sender PUBLIC pigpio INTERFACE Threads::Threads)

# Add profinet support via profipp v1.0 or higher
find_package(profipp 1.0 REQUIRED)
#target_link_libraries(datadiode_sender PUBLIC /usr/local/lib/profipp1.0/Release/libpnet.a)
target_link_libraries(datadiode_sender PUBLIC profipp)

# Install the example application
install(TARGETS datadiode_sender DESTINATION bin)
# Also install any additional files which might be required due to anything compiler specific
include(InstallRequiredSystemLibraries)
