#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fltk::fltk" for configuration "Release"
set_property(TARGET fltk::fltk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fltk::fltk PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfltk.a"
  )

list(APPEND _cmake_import_check_targets fltk::fltk )
list(APPEND _cmake_import_check_files_for_fltk::fltk "${_IMPORT_PREFIX}/lib/libfltk.a" )

# Import target "fltk::forms" for configuration "Release"
set_property(TARGET fltk::forms APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fltk::forms PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfltk_forms.a"
  )

list(APPEND _cmake_import_check_targets fltk::forms )
list(APPEND _cmake_import_check_files_for_fltk::forms "${_IMPORT_PREFIX}/lib/libfltk_forms.a" )

# Import target "fltk::images" for configuration "Release"
set_property(TARGET fltk::images APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fltk::images PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfltk_images.a"
  )

list(APPEND _cmake_import_check_targets fltk::images )
list(APPEND _cmake_import_check_files_for_fltk::images "${_IMPORT_PREFIX}/lib/libfltk_images.a" )

# Import target "fltk::gl" for configuration "Release"
set_property(TARGET fltk::gl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fltk::gl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfltk_gl.a"
  )

list(APPEND _cmake_import_check_targets fltk::gl )
list(APPEND _cmake_import_check_files_for_fltk::gl "${_IMPORT_PREFIX}/lib/libfltk_gl.a" )

# Import target "fltk::fluid" for configuration "Release"
set_property(TARGET fltk::fluid APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fltk::fluid PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/fluid"
  )

list(APPEND _cmake_import_check_targets fltk::fluid )
list(APPEND _cmake_import_check_files_for_fltk::fluid "${_IMPORT_PREFIX}/bin/fluid" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
