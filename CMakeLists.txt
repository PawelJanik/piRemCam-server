cmake_minimum_required(VERSION 2.6)
project(piremcam)

add_executable(piremcam main.cpp camera.cpp cameralist.cpp process.cpp)

find_package(nlohmann_json 3.2.0 REQUIRED)

#target_link_libraries(piremcam ${catkin_LIBRARIES} libmosquitto.so libdaemoncpp.so)
target_link_libraries(piremcam libmosquitto.so libdaemoncpp.so nlohmann_json::nlohmann_json libgphoto2.so)

#install(TARGETS piremcam RUNTIME DESTINATION bin)

# Directory with configuration files
set (DAEMON_CONF_DIR "/etc/piremcam")

# Directory with systemd unit files
set (SYSTEMD_UNIT_DIR "/usr/lib/systemd/system/")

# Macro for installing configuration files
function(install_conf src dest)
  if(NOT IS_ABSOLUTE "${src}")
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
  endif()
  get_filename_component(src_name "${src}" NAME)
  if (NOT IS_ABSOLUTE "${dest}")
    set(dest "${CMAKE_INSTALL_PREFIX}/${dest}")
  endif()
  install(CODE "
    if(NOT EXISTS \"\$ENV{DESTDIR}${dest}/${src_name}\")
      #file(INSTALL \"${src}\" DESTINATION \"${dest}\")
      message(STATUS \"Installing: \$ENV{DESTDIR}${dest}/${src_name}\")
      execute_process(COMMAND \${CMAKE_COMMAND} -E copy \"${src}\"
                      \"\$ENV{DESTDIR}${dest}/${src_name}\"
                      RESULT_VARIABLE copy_result
                      ERROR_VARIABLE error_output)
      if(copy_result)
        message(FATAL_ERROR \${error_output})
      endif()
    else()
      message(STATUS \"Skipping  : \$ENV{DESTDIR}${dest}/${src_name}\")
    endif()
  ")
endfunction(install_conf)

# Install configuration file
install_conf (./piremcam.conf ${DAEMON_CONF_DIR})

# Install systemd unit files 
install_conf (./piremcam.service ${SYSTEMD_UNIT_DIR})

install (TARGETS piremcam RUNTIME DESTINATION bin)
