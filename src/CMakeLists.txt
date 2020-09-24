# ########## Shimmer wallet library ################

add_library(qian_bao STATIC)
# the required minimum C standard
set_target_properties(qian_bao PROPERTIES C_STANDARD_REQUIRED NO C_STANDARD 99)

target_sources(
 qian_bao
  PRIVATE "core/address.c"
          "core/balance.c"
          "utils/iota_str.c"
          "utils/bitmask.c"
  PUBLIC "core/address.h"
         "core/balance.h"
         "utils/iota_str.h"
         "utils/bitmask.h"
)

target_include_directories(qian_bao PUBLIC "${PROJECT_SOURCE_DIR}/src" "${CMAKE_INSTALL_PREFIX}/include")

add_dependencies(qian_bao sodium ext_base58 ext_uthash)

target_link_libraries(qian_bao INTERFACE base58 sodium)

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