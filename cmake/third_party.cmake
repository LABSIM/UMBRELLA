#
# UMBRELLA : acronyme AH ?
# Copyright (C) 2026-present Nawfel KINANI
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.
#

# # push policies
# cmake_policy(PUSH)
# cmake_policy(SET CMP0069 NEW)
# cmake_policy(SET CMP0077 NEW)

# # strict C17 & C++23
# set(CMAKE_C_STANDARD            17 )
# set(CMAKE_C_EXTENSIONS          OFF)
# set(CMAKE_C_STANDARD_REQUIRED   ON )
# set(CMAKE_CXX_STANDARD          23 )
# set(CMAKE_CXX_EXTENSIONS        OFF)
# set(CMAKE_CXX_STANDARD_REQUIRED ON )

# # include necessary cmake functions
# include(FetchContent)

# # make it verbose by default... 
# set(FETCHCONTENT_QUIET FALSE)

# # =====================================================================
# # Google RPC / v1.70.1
# # [ https://github.com/grpc/grpc/releases/tag/v1.70.1 ]
# # ---------------------------------------------------------------------
# FetchContent_Declare(
#   gRPC
#   GIT_REPOSITORY
#     https://github.com/grpc/grpc
#   GIT_TAG
#     v1.70.1
#   GIT_PROGRESS
#     TRUE
#   GIT_SHALLOW
#     TRUE
#   GIT_SUBMODULES_RECURSE 
#     FALSE
#   GIT_SUBMODULES         
#     "third_party/re2" 
#     "third_party/abseil-cpp"
#     "third_party/protobuf"
#   USES_TERMINAL_DOWNLOAD 
#     TRUE
#   OVERRIDE_FIND_PACKAGE
# )
# # ---------------------------------------------------------------------
# set(BUILD_TESTING                      OFF       CACHE BOOL   INTERNAL)
# set(BUILD_SHARED_LIBS                  OFF       CACHE BOOL   INTERNAL)
# # ---------------------------------------------------------------------
# set(ABSL_BUILD_TESTING                 OFF       CACHE BOOL   INTERNAL)
# set(ABSL_BUILD_TEST_HELPERS            OFF       CACHE BOOL   INTERNAL)
# set(ABSL_BUILD_MONOLITHIC_SHARED_LIBS  OFF       CACHE BOOL   INTERNAL)
# set(ABSL_USE_EXTERNAL_GOOGLETEST       OFF       CACHE BOOL   INTERNAL)
# set(ABSL_USE_GOOGLETEST_HEAD           OFF       CACHE BOOL   INTERNAL)
# set(ABSL_USE_SYSTEM_INCLUDES           ON        CACHE BOOL   INTERNAL)
# set(ABSL_ENABLE_INSTALL                ON        CACHE BOOL   INTERNAL)
# set(ABSL_PROPAGATE_CXX_STD             OFF       CACHE BOOL   INTERNAL)
# # ---------------------------------------------------------------------
# set(protobuf_BUILD_CONFORMANCE         OFF       CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_EXAMPLES            OFF       CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_LIBPROTOC           ON        CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_PROTOC_BINARIES     ON        CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_PROTOBUF_BINARIES   ON        CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_SHARED_LIBS         OFF       CACHE BOOL   INTERNAL)
# set(protobuf_BUILD_TESTS               OFF       CACHE BOOL   INTERNAL)
# set(protobuf_DISABLE_RTTI              ON        CACHE BOOL   INTERNAL)
# set(protobuf_INSTALL                   ON        CACHE BOOL   INTERNAL)
# set(protobuf_INSTALL_EXAMPLES          OFF       CACHE BOOL   INTERNAL)
# set(protobuf_MSVC_STATIC_RUNTIME       OFF       CACHE BOOL   INTERNAL)
# set(protobuf_WITH_ZLIB                 ON        CACHE BOOL   INTERNAL)
# set(protobuf_ALLOW_CCACHE              OFF       CACHE BOOL   INTERNAL)
# set(protobuf_REMOVE_INSTALLED_HEADER   OFF       CACHE BOOL   INTERNAL)
# set(protobuf_TEST_XML_OUTDIR           OFF       CACHE BOOL   INTERNAL)
# set(protobuf_USE_EXTERNAL_GTEST        OFF       CACHE BOOL   INTERNAL)
# set(protobuf_ABSL_PROVIDER             "module"  CACHE STRING INTERNAL)
# set(protobuf_JSONCPP_PROVIDER          "module"  CACHE STRING INTERNAL)
# # ---------------------------------------------------------------------
# set(RE2_BUILD_TESTING                  OFF       CACHE BOOL   INTERNAL)
# # ---------------------------------------------------------------------
# set(gRPC_BACKWARDS_COMPATIBILITY_MODE  OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_CODEGEN                 ON        CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_CSHARP_EXT              OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_CPP_PLUGIN         ON        CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_CSHARP_PLUGIN      OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_NODE_PLUGIN        OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_PHP_PLUGIN         OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_PYTHON_PLUGIN      OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPC_RUBY_PLUGIN        OFF       CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_TESTS                   OFF       CACHE BOOL   INTERNAL)
# set(gRPC_INSTALL                       ON        CACHE BOOL   INTERNAL)
# set(gRPC_USE_PROTO_LITE                ON        CACHE BOOL   INTERNAL)
# set(gRPC_BUILD_GRPCPP_OTEL_PLUGIN      OFF       CACHE BOOL   INTERNAL)
# set(gRPC_DOWNLOAD_ARCHIVES             OFF       CACHE BOOL   INTERNAL)
# set(gRPC_ABSL_PROVIDER                 "module"  CACHE STRING INTERNAL)
# set(gRPC_CARES_PROVIDER                "package" CACHE STRING INTERNAL)
# set(gRPC_PROTOBUF_PACKAGE_TYPE         "MODULE"  CACHE STRING INTERNAL)
# set(gRPC_PROTOBUF_PROVIDER             "module"  CACHE STRING INTERNAL)
# set(gRPC_RE2_PROVIDER                  "module"  CACHE STRING INTERNAL)
# set(gRPC_SSL_PROVIDER                  "package" CACHE STRING INTERNAL)
# set(gRPC_ZLIB_PROVIDER                 "package" CACHE STRING INTERNAL)
# # ---------------------------------------------------------------------
# FetchContent_MakeAvailable(gRPC)
# # =====================================================================
# # POCO C++ / 1.14.1
# # [ https://github.com/pocoproject/poco/releases/tag/poco-1.14.1-release ]
# # ---------------------------------------------------------------------
# FetchContent_Declare(
#   Poco
#   GIT_REPOSITORY 
#     https://github.com/pocoproject/poco.git
#   GIT_TAG        
#     poco-1.14.1-release
#   GIT_PROGRESS   
#     TRUE
#   GIT_SHALLOW
#     TRUE
#   GIT_SUBMODULES_RECURSE 
#     FALSE
#   GIT_SUBMODULES         
#     ""
#   USES_TERMINAL_DOWNLOAD 
#     TRUE
#   OVERRIDE_FIND_PACKAGE
# )
# # ---------------------------------------------------------------------
# set(ENABLE_ACTIVERECORD              OFF CACHE BOOL INTERNAL)
# set(ENABLE_ACTIVERECORD_COMPILER     OFF CACHE BOOL INTERNAL)
# set(ENABLE_APACHECONNECTOR           OFF CACHE BOOL INTERNAL)
# set(ENABLE_COMPILER_WARNINGS         OFF CACHE BOOL INTERNAL)
# set(ENABLE_CPPPARSER                 ON  CACHE BOOL INTERNAL)
# set(ENABLE_CRYPTO                    ON  CACHE BOOL INTERNAL)
# set(ENABLE_DATA                      OFF CACHE BOOL INTERNAL)
# set(ENABLE_DATA_MYSQL                OFF CACHE BOOL INTERNAL)
# set(ENABLE_DATA_ODBC                 OFF CACHE BOOL INTERNAL)
# set(ENABLE_DATA_POSTGRESQL           OFF CACHE BOOL INTERNAL)
# set(ENABLE_DATA_SQLITE               OFF CACHE BOOL INTERNAL)
# set(ENABLE_DATA_SQLITE_FTS5          OFF CACHE BOOL INTERNAL)
# set(ENABLE_ENCODINGS                 ON  CACHE BOOL INTERNAL)
# set(ENABLE_ENCODINGS_COMPILER        ON  CACHE BOOL INTERNAL)
# set(ENABLE_FOUNDATION                ON  CACHE BOOL INTERNAL)
# set(ENABLE_JSON                      ON  CACHE BOOL INTERNAL)
# set(ENABLE_JWT                       ON  CACHE BOOL INTERNAL)
# set(ENABLE_MONGODB                   ON  CACHE BOOL INTERNAL)
# set(ENABLE_NET                       ON  CACHE BOOL INTERNAL)
# set(ENABLE_NETSSL                    ON  CACHE BOOL INTERNAL)
# set(ENABLE_NETSSL_WIN                OFF CACHE BOOL INTERNAL)
# set(ENABLE_PAGECOMPILER              OFF CACHE BOOL INTERNAL)
# set(ENABLE_PAGECOMPILER_FILE2PAGE    OFF CACHE BOOL INTERNAL)
# set(ENABLE_PDF                       OFF CACHE BOOL INTERNAL)
# set(ENABLE_POCODOC                   OFF CACHE BOOL INTERNAL)
# set(ENABLE_PROMETHEUS                ON  CACHE BOOL INTERNAL)
# set(ENABLE_REDIS                     ON  CACHE BOOL INTERNAL)
# set(ENABLE_SAMPLES                   OFF CACHE BOOL INTERNAL)
# set(ENABLE_SEVENZIP                  ON  CACHE BOOL INTERNAL)
# set(ENABLE_TESTS                     OFF CACHE BOOL INTERNAL)
# set(ENABLE_TEST_DEPRECATED           OFF CACHE BOOL INTERNAL)
# set(ENABLE_UTIL                      ON  CACHE BOOL INTERNAL)
# set(ENABLE_XML                       ON  CACHE BOOL INTERNAL)
# set(ENABLE_ZIP                       OFF CACHE BOOL INTERNAL)
# set(POCO_ENABLE_STD_MUTEX            ON  CACHE BOOL INTERNAL)
# # ---------------------------------------------------------------------
# FetchContent_MakeAvailable(Poco)
# # =====================================================================

# # pop policy back to normal
# cmake_policy(POP)