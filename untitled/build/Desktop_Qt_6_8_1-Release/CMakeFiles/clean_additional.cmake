# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/JoystickIdentifier_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/JoystickIdentifier_autogen.dir/ParseCache.txt"
  "CMakeFiles/siyi-sdk_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/siyi-sdk_autogen.dir/ParseCache.txt"
  "JoystickIdentifier_autogen"
  "QJoysticks/CMakeFiles/QJoysticks_autogen.dir/AutogenUsed.txt"
  "QJoysticks/CMakeFiles/QJoysticks_autogen.dir/ParseCache.txt"
  "QJoysticks/QJoysticks_autogen"
  "VideoReceiver/CMakeFiles/VideoReceiver_autogen.dir/AutogenUsed.txt"
  "VideoReceiver/CMakeFiles/VideoReceiver_autogen.dir/ParseCache.txt"
  "VideoReceiver/VideoReceiver_autogen"
  "siyi-sdk_autogen"
  )
endif()
