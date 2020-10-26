# ########## Shimmer wallet library ################

add_library(qian_bao STATIC)
# the required minimum C standard
set_target_properties(qian_bao PROPERTIES C_STANDARD_REQUIRED NO C_STANDARD 99)

target_sources(
 qian_bao
  PRIVATE "client/api/get_funds.c"
          "client/api/get_node_info.c"
          "client/api/get_unspent_outputs.c"
          "client/api/send_transaction.c"
          "client/api/json_utils.c"
          "client/api/response_error.c"
          "client/network/http_buffer.c"
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
         "client/network/http_buffer.h"
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