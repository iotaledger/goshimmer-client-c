# ########## Pollen wallet library ################

add_library(goshimmer_client STATIC)
# the required minimum C standard
set_target_properties(goshimmer_client PROPERTIES C_STANDARD_REQUIRED NO C_STANDARD 99)

target_sources(
 goshimmer_client
  PRIVATE "client/api/get_funds.c"
          "client/api/get_node_info.c"
          "client/api/get_unspent_outputs.c"
          "client/api/send_transaction.c"
          "client/api/json_utils.c"
          "client/api/response_error.c"
          "client/network/http_curl.c"
          "core/address.c"
          "core/balance.c"
          "core/signatures.c"
          "core/transaction.c"
          "core/output_ids.c"
          "core/unspent_outputs.c"
          "utils/iota_str.c"
          "utils/bitmask.c"
          "utils/byte_buffer.c"
          "utils/base64.c"
          "wallet/address_manager.c"
          "wallet/wallet.c"
  PUBLIC "client/api/get_funds.h"
         "client/api/get_node_info.h"
         "client/api/get_unspent_outputs.h"
         "client/api/send_transaction.h"
         "client/api/json_utils.h"
         "client/api/response_error.h"
         "client/network/http.h"
         "core/address.h"
         "core/balance.h"
         "core/message.h"
         "core/signatures.h"
         "core/transaction.h"
         "core/output_ids.h"
         "core/unspent_outputs.h"
         "utils/iota_str.h"
         "utils/bitmask.h"
         "utils/byte_buffer.h"
         "utils/base64.h"
         "wallet/address_manager.h"
         "wallet/wallet.h"
)

target_include_directories(goshimmer_client PUBLIC 
  "${PROJECT_SOURCE_DIR}/src"
  "${CURL_INCLUDE_DIRS}"
  "${CMAKE_INSTALL_PREFIX}/include"
  "${CMAKE_INSTALL_PREFIX}/include/cjson" # esp32 compatibility
)

add_dependencies(goshimmer_client sodium ext_base58 ext_uthash ext_cjson)

target_link_libraries(goshimmer_client INTERFACE base58 sodium ${CURL_LIBRARIES} cjson)

if(HAS_ASAN_ENABLED)
  target_link_libraries(goshimmer_client PRIVATE asan)
endif()

# install client lib and headers
install(TARGETS goshimmer_client DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
install(
  DIRECTORY "${PROJECT_SOURCE_DIR}/src/core/"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/include/core"
  FILES_MATCHING
  PATTERN "*.h")
