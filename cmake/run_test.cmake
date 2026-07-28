set (COMPARE ${CMAKE_COMMAND} -E compare_files ${name}.out ${reference})

message("Command for the test: ${cmd}")

# Pass ${cmd} as a CMake list so paths containing spaces are preserved
execute_process(
 COMMAND ${cmd}
  RESULT_VARIABLE not_successful
  OUTPUT_FILE ${name}.out
  ERROR_VARIABLE err
  TIMEOUT 600
)

if(not_successful)
 message(SEND_ERROR "Error runing test '${name}': ${err}; command: ${cmd}; shell output: ${not_successful}!")
endif(not_successful)

# if no reference file, stop here
if (reference)

 string (REPLACE ";" " " COM_STR "${COMPARE}")
 message( "About to compare using: ${COM_STR}")

 # Little fix to turn -0 into 0 (--0 is not replaced)
 FILE(READ ${name}.out temp)
 STRING(REGEX REPLACE "([^-])-0([^.])" "\\10\\2" temp_after "${temp}")
 FILE(WRITE ${name}.out ${temp_after})

 # Run compare command
 execute_process(
  COMMAND ${COMPARE}
   RESULT_VARIABLE not_successful
   OUTPUT_VARIABLE out
   ERROR_VARIABLE err
   TIMEOUT 600
 )

 if(not_successful)
  execute_process(COMMAND diff ${name}.out ${reference} OUTPUT_VARIABLE diff)
  message(SEND_ERROR "Output does not match for test '${name}': ${err}\n File Differences: \n${diff}")
 endif(not_successful)

endif(reference)
