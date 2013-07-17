#  Copyright Olivier Parcollet 2011 : modified to remove pyalps install
#  Copyright Matthias Troyer 2009 - 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# libboost
#

set (Boost_ROOT_DIR ${BOOST_SOURCE_DIR})

set(BOOST_SOURCES "")
set(BOOST_MPI_PYTHON_SOURCES "")
set(BOOST_LINK_LIBS "")

# Boost.Date_Time
if(BUILD_BOOST_DATE_TIME)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/date_time/src")
  set(SOURCES gregorian/date_generators.cpp gregorian/greg_month.cpp gregorian/greg_weekday.cpp
      gregorian/gregorian_types.cpp posix_time/posix_time_types.cpp)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_DATE_TIME)

# Boost.Filesystem
if(BUILD_BOOST_FILESYSTEM)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/filesystem")
  set(SOURCES
    codecvt_error_category.cpp operations.cpp path.cpp path_traits.cpp
    portability.cpp unique_path.cpp utf8_codecvt_facet.cpp 
    windows_file_codecvt.cpp v2_operations.cpp v2_path.cpp v2_portability.cpp)
  foreach (D ${DIRECTORY}/src ${DIRECTORY}/v2/src ${DIRECTORY}/v3/src)
    foreach(S ${SOURCES})
      if(EXISTS ${D}/${S})
        set(BOOST_SOURCES ${BOOST_SOURCES} ${D}/${S})
      endif(EXISTS ${D}/${S})
    endforeach(S)
  endforeach(D)
endif(BUILD_BOOST_FILESYSTEM)

# Boost.ProgramOptions
if(BUILD_BOOST_PROGRAM_OPTIONS)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/program_options/src")
  set(SOURCES cmdline.cpp config_file.cpp options_description.cpp parsers.cpp
      variables_map.cpp value_semantic.cpp positional_options.cpp utf8_codecvt_facet.cpp
      convert.cpp winmain.cpp)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_PROGRAM_OPTIONS)

# Boost.Regex
if(BUILD_BOOST_REGEX)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/regex/src")
  set(SOURCES c_regex_traits.cpp cpp_regex_traits.cpp cregex.cpp fileiter.cpp icu.cpp
      instances.cpp posix_api.cpp regex.cpp regex_debug.cpp regex_raw_buffer.cpp
      regex_traits_defaults.cpp static_mutex.cpp usinstances.cpp w32_regex_traits.cpp
      wc_regex_traits.cpp wide_posix_api.cpp winstances.cpp)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_REGEX)


# Boost.Serialization
if(BUILD_BOOST_SERIALIZATION)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/serialization/src")
  set(SOURCES archive_exception.cpp basic_archive.cpp
      basic_iarchive.cpp basic_iserializer.cpp basic_oarchive.cpp
      basic_oserializer.cpp basic_pointer_iserializer.cpp
      basic_pointer_oserializer.cpp basic_serializer_map.cpp
      basic_text_iprimitive.cpp basic_text_oprimitive.cpp
      basic_text_wiprimitive.cpp basic_text_woprimitive.cpp
      basic_xml_archive.cpp binary_iarchive.cpp binary_oarchive.cpp
      binary_wiarchive.cpp binary_woarchive.cpp codecvt_null.cpp
      extended_type_info.cpp extended_type_info_no_rtti.cpp
      extended_type_info_typeid.cpp polymorphic_iarchive.cpp
      polymorphic_oarchive.cpp shared_ptr_helper.cpp stl_port.cpp
      text_iarchive.cpp text_oarchive.cpp text_wiarchive.cpp
      text_woarchive.cpp utf8_codecvt_facet.cpp void_cast.cpp
      xml_archive_exception.cpp xml_grammar.cpp xml_iarchive.cpp
      xml_oarchive.cpp xml_wgrammar.cpp xml_wiarchive.cpp
      xml_woarchive.cpp)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_SERIALIZATION)

# Boost.Python
if(BUILD_BOOST_PYTHON)
  set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS} ${PYTHON_LIBRARY} ${PYTHON_EXTRA_LIBS})
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/python/src")
  set(SOURCES dict.cpp errors.cpp exec.cpp import.cpp list.cpp long.cpp
      module.cpp numeric.cpp object_operators.cpp object_protocol.cpp slice.cpp
      str.cpp tuple.cpp wrapper.cpp converter/arg_to_python_base.cpp
      converter/builtin_converters.cpp converter/from_python.cpp
      converter/registry.cpp converter/type_id.cpp object/class.cpp
      object/enum.cpp object/function.cpp object/function_doc_signature.cpp
      object/inheritance.cpp object/iterator.cpp object/life_support.cpp
      object/pickle_support.cpp object/stl_iterator.cpp
  )
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
  add_definitions(-DBOOST_PYTHON_SOURCE)
endif(BUILD_BOOST_PYTHON)

# Boost.System
if(BUILD_BOOST_SYSTEM)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/system/src")
  set(SOURCES error_code.cpp)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_SYSTEM)

# Boost.MPI
if(BUILD_BOOST_MPI)
  IF(NOT WIN32)
    set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS} ${MPI_LIBRARIES})
  ENDIF(NOT WIN32)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/mpi/src")
  set(SOURCES broadcast.cpp communicator.cpp computation_tree.cpp 
      content_oarchive.cpp environment.cpp exception.cpp graph_communicator.cpp
      group.cpp intercommunicator.cpp mpi_datatype_cache.cpp 
      mpi_datatype_oarchive.cpp packed_iarchive.cpp packed_oarchive.cpp
      packed_skeleton_iarchive.cpp packed_skeleton_oarchive.cpp
      point_to_point.cpp request.cpp timer.cpp
  )
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_MPI)

# Boost.Thread
if(BUILD_BOOST_THREAD)
  if (NOT WIN32)
    set(BOOST_LINK_LIBS ${BOOST_LINK_LIBS} ${PTHREAD_LIBRARY})
  endif (NOT WIN32)
  set(DIRECTORY "${Boost_ROOT_DIR}/libs/thread/src")
  if (WIN32)
    set(SOURCES win32/thread.cpp win32/tss_dll.cpp win32/tss_pe.cpp)
  else (WIN32)
    set(SOURCES pthread/once.cpp pthread/thread.cpp)
  endif(WIN32)
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_SOURCES ${BOOST_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)
endif(BUILD_BOOST_THREAD)


# Boost.MPI Python bindings
if (BUILD_BOOST_MPI AND BUILD_BOOST_PYTHON AND NOT ALPS_BUILD_SOURCE AND Boost_ROOT_DIR)

  set(DIRECTORY "${Boost_ROOT_DIR}/libs/mpi/src/python")
  set(SOURCES collectives.cpp py_communicator.cpp datatypes.cpp 
      documentation.cpp py_environment.cpp py_nonblocking.cpp py_exception.cpp
      module.cpp py_request.cpp skeleton_and_content.cpp status.cpp py_timer.cpp 
      serialize.cpp
  )
  foreach(S ${SOURCES})
    if(EXISTS ${DIRECTORY}/${S})
      set(BOOST_MPI_PYTHON_SOURCES ${BOOST_MPI_PYTHON_SOURCES} ${DIRECTORY}/${S})
    endif(EXISTS ${DIRECTORY}/${S})
  endforeach(S)

  if (BOOST_MPI_PYTHON_SOURCES)
    python_add_module(mpi ${BOOST_MPI_PYTHON_SOURCES})
    if(BUILD_SHARED_LIBS)
      set_target_properties(mpi PROPERTIES COMPILE_DEFINITIONS "${ALPS_SHARED_CPPFLAGS}")
    if(WIN32 AND NOT UNIX)
      set_target_properties(mpi PROPERTIES SUFFIX ".pyd")
    endif(WIN32 AND NOT UNIX)
    endif (BUILD_SHARED_LIBS)

    set_target_properties(mpi PROPERTIES PREFIX "")
    target_link_libraries(mpi ${ALPS_BOOST_LIBRARY_NAME} ${BOOST_LINK_LIBS})

  endif(BOOST_MPI_PYTHON_SOURCES)
  
endif (BUILD_BOOST_MPI AND BUILD_BOOST_PYTHON AND NOT ALPS_BUILD_SOURCE AND Boost_ROOT_DIR)


#######################################################################
# install
#######################################################################

# Setting "boost" target not needed when precompiled Boost libraries found.

if (NOT Boost_FOUND)
  add_library(${ALPS_BOOST_LIBRARY_NAME} ${BOOST_SOURCES})
  target_link_libraries(${ALPS_BOOST_LIBRARY_NAME} ${BOOST_LINK_LIBS})

    # Boost.Test
    if(BUILD_BOOST_TEST AND NOT ALPS_FOR_VISTRAILS)
      set(DIRECTORY "${Boost_ROOT_DIR}/libs/test/src")
      
      set(SOURCES compiler_log_formatter.cpp debug.cpp 
          execution_monitor.cpp framework.cpp 
          interaction_based.cpp logged_expectations.cpp plain_report_formatter.cpp
          progress_monitor.cpp results_collector.cpp results_reporter.cpp
          test_tools.cpp unit_test_log.cpp unit_test_main.cpp
          unit_test_monitor.cpp unit_test_parameters.cpp unit_test_suite.cpp
          xml_log_formatter.cpp xml_report_formatter.cpp
      )
      set (TEST_EXEC_MON_SOURCES "")
      foreach(S ${SOURCES})
        if(EXISTS ${DIRECTORY}/${S})
          set(TEST_EXEC_MON_SOURCES ${TEST_EXEC_MON_SOURCES} ${DIRECTORY}/${S})
        endif(EXISTS ${DIRECTORY}/${S})
      endforeach(S)

      set(SOURCES ${SOURCES} exception_safety.cpp)
      set (UTF_SOURCES "")
      foreach(S ${SOURCES})
        if(EXISTS ${DIRECTORY}/${S})
          set(UTF_SOURCES ${UTF_SOURCES} ${DIRECTORY}/${S})
        endif(EXISTS ${DIRECTORY}/${S})
      endforeach(S)

      set(SOURCES execution_monitor.cpp debug.cpp cpp_main.cpp  )
      set (PRG_EXEC_MON_SOURCES "")
      foreach(S ${SOURCES})
        if(EXISTS ${DIRECTORY}/${S})
          set(PRG_EXEC_MON_SOURCES ${PRG_EXEC_MON_SOURCES} ${DIRECTORY}/${S})
        endif(EXISTS ${DIRECTORY}/${S})
      endforeach(S)

      add_library(boost_unit_test_framework ${UTF_SOURCES})
      add_library(boost_test_exec_monitor ${TEST_EXEC_MON_SOURCES})
      add_library(boost_prg_exec_monitor ${PRG_EXEC_MON_SOURCES})
      
      if(BUILD_SHARED_LIBS)
        set_target_properties(boost_unit_test_framework PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK=1)
        set_target_properties(boost_test_exec_monitor PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK=1)
        set_target_properties(boost_prg_exec_monitor PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK=1)
      endif(BUILD_SHARED_LIBS)

    endif(BUILD_BOOST_TEST AND NOT ALPS_FOR_VISTRAILS)


  if(WIN32)
    if(BUILD_SHARED_LIBS)
      set_target_properties(boost PROPERTIES COMPILE_DEFINITIONS BOOST_THREAD_BUILD_DLL=1)
    else(BUILD_SHARED_LIBS)
      set_target_properties(boost PROPERTIES COMPILE_DEFINITIONS BOOST_THREAD_BUILD_LIB=1)
    endif(BUILD_SHARED_LIBS)
  endif(WIN32)

  if (NOT ALPS_BUILD_SOURCE)
    if(MSVC)
      get_target_property(LIB_NAME ${ALPS_BOOST_LIBRARY_NAME} LOCATION)
      add_custom_command(TARGET ${ALPS_BOOST_LIBRARY_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${PROJECT_BINARY_DIR}/bin)
    endif(MSVC)

    if (ALPS_FOR_VISTRAILS)
      SET_TARGET_PROPERTIES(${ALPS_BOOST_LIBRARY_NAME}
        PROPERTIES BUILD_WITH_INSTALL_RPATH ON
        INSTALL_NAME_DIR "@executable_path/../Frameworks"
      )
    endif (ALPS_FOR_VISTRAILS)

    if (ALPS_INSTALL_VISTRAILS_PACKAGES)
      install(TARGETS ${ALPS_BOOST_LIBRARY_NAME}  COMPONENT vistrails
              RUNTIME DESTINATION ${VISTRAILS_DYLIB_DIR}
              ARCHIVE DESTINATION ${VISTRAILS_DYLIB_DIR}
              LIBRARY DESTINATION ${VISTRAILS_DYLIB_DIR})
    endif (ALPS_INSTALL_VISTRAILS_PACKAGES)
    if (NOT ALPS_FOR_VISTRAILS)
      install(TARGETS ${ALPS_BOOST_LIBRARY_NAME} COMPONENT libraries
              RUNTIME DESTINATION bin
              ARCHIVE DESTINATION lib
              LIBRARY DESTINATION lib)
      if (ALPS_INSTALL_BOOST_TEST)
        install(TARGETS boost_unit_test_framework boost_test_exec_monitor boost_prg_exec_monitor
              COMPONENT libraries
              RUNTIME DESTINATION bin
              ARCHIVE DESTINATION lib
              LIBRARY DESTINATION lib)
      endif(ALPS_INSTALL_BOOST_TEST)
    endif (NOT ALPS_FOR_VISTRAILS)
  endif (NOT ALPS_BUILD_SOURCE)
endif (NOT Boost_FOUND)
