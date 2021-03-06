include(Teensy)

file(GLOB REAL_SRC
     *.cpp
     commands/*.cpp)

file(GLOB_RECURSE REAL_LIBS_SRC
     lib/*.cpp)

file(GLOB REAL_LIB_DIRS lib/*)

include_directories(${CMAKE_SOURCE_DIR} ${REAL_LIB_DIRS} . commands ../common/Eigen)
add_definitions(-DEMBED)

import_arduino_library(SPI)
import_arduino_library(EEPROM)

add_teensy_core()
add_teensy_library(real_com ${COM_SRC} ${CORE_COM_SRC} ${COM_COMMAND_SRC} ${REAL_SRC} ${REAL_LIBS_SRC})
set_target_properties(real_com PROPERTIES COMPILE_FLAGS "-include ${UTIL_HEADER}")

set(mains
    main/test_everything.cpp
    main/measure_analog_curves.cpp
    main/as5048a_test.cpp
    main/analog_test.cpp
    main/test_localization.cpp
    main/main.cpp
    main/measure_motor_curve.cpp
    main/execute_velocity_profile.cpp
    main/motor_and_battery_test.cpp
    main/profile_encoders.cpp
    main/measure_motor_response.cpp
    main/measure_w_freq_res_changes.cpp
    main/eigen_test.cpp)

foreach (main ${mains})
  get_filename_component(main_target ${main} NAME_WE)
  add_teensy_executable(${main_target} ${main})
  target_link_libraries(${main_target} real_com)
  set_target_properties(${main_target} PROPERTIES COMPILE_FLAGS "-include ${UTIL_HEADER}")
endforeach ()
