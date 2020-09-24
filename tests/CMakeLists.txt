# function for test cases
function(test_case_add test_src test_name)
  add_executable(${test_name} "${test_src}")
  target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src ${CMAKE_INSTALL_PREFIX}/include)
  add_dependencies(${test_name} ext_unity)
  if(HAS_ASAN_ENABLED)
    target_link_libraries(${test_name} PRIVATE asan)
  endif()
  add_dependencies(${test_name} qian_bao)
  target_link_libraries(${test_name} PRIVATE qian_bao unity)
  add_test(${test_name} ${test_name})
endfunction(test_case_add)

test_case_add("client/test_http_client.c" http_client)

test_case_add("utils/test_bitmask.c" utils_bitmask)