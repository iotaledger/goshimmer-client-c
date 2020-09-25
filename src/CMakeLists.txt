# ########## Shimmer wallet library ################

add_library(qian_bao STATIC)
# the required minimum C standard
set_target_properties(qian_bao PROPERTIES C_STANDARD_REQUIRED NO C_STANDARD 99)

target_sources(
 qian_bao
  PRIVATE "client/api/get_node_info.c"
          "client/api/json_utils.c"
          "client/api/response_error.c"
          "client/api/get_funds.c"
          "client/network/http_buffer.c"
          "client/network/http_curl.c"
          "core/address.c"
          "core/balance.c"
          "utils/iota_str.c"
          "utils/bitmask.c"
          "wallet/address_manager.c"
  PUBLIC "client/api/get_node_info.h"
         "client/api/json_utils.h"
         "client/api/response_error.h"
         "client/api/get_funds.h"
         "client/network/http_buffer.h"
         "client/network/http.h"
         "core/address.h"
         "core/balance.h"
         "utils/iota_str.h"
         "utils/bitmask.h"
         "wallet/address_manager.h"
)

target_include_directories(qian_bao PUBLIC 
  "${PROJECT_SOURCE_DIR}/src"
  "${CURL_INCLUDE_DIRS}"
  "${CMAKE_INSTALL_PREFIX}/include"
  "${CMAKE_INSTALL_PREFIX}/include/cjson" # esp32 compatibility
)

add_dependencies(qian_bao sodium ext_base58 ext_uthash ext_cjson)

target_link_libraries(qian_bao INTERFACE base58 sodium ${CURL_LIBRARIES} cjson)

if(HAS_ASAN_ENABLED)
  target_link_libraries(qian_bao PRIVATE asan)
endif()

# install client lib and headers
install(TARGETS qian_bao DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/src/core/"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/include/core"
  FILES_MATCHING
  PATTERN "*.h")

# ########## IOTA wallet app ################

# add_executable(wallet_app)