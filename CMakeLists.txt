# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

find_package(DL REQUIRED)

set(LUA_CORE_SRC src/lapi.c src/lcode.c src/ldebug.c
	src/ldo.c src/ldump.c src/lfunc.c src/lgc.c
	src/llex.c src/lmem.c src/lobject.c src/lopcodes.c
	src/lparser.c src/lstate.c src/lstring.c src/ltable.c
	src/ltm.c src/lundump.c src/lvm.c src/lzio.c)
set(LUA_LIB_SRC src/lauxlib.c src/lbaselib.c src/ldblib.c
	src/liolib.c src/lmathlib.c src/loslib.c src/ltablib.c
	src/lstrlib.c src/loadlib.c src/linit.c)

add_definitions(-DLUA_USE_POSIX=1)
add_definitions(-DLUA_USE_DLOPEN=1)
add_definitions(-DLUA_ROOT="${CMAKE_HAKA_INSTALL_PREFIX}/")


add_library(liblua SHARED ${LUA_CORE_SRC} ${LUA_LIB_SRC})
target_link_libraries(liblua m ${DL_LIBRARIES})
set_target_properties(liblua PROPERTIES VERSION 5.1.5 SOVERSION 5)
set_target_properties(liblua PROPERTIES OUTPUT_NAME lua)

install(TARGETS liblua LIBRARY DESTINATION ${HAKA_INSTALL_PREFIX}/lib)
install(FILES src/lua.h src/luaconf.h src/lualib.h src/lauxlib.h DESTINATION ${HAKA_INSTALL_PREFIX}/include)

add_executable(luac ${LUA_CORE_SRC} ${LUA_LIB_SRC} src/luac.c src/print.c)
target_link_libraries(luac m ${DL_LIBRARIES})

set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_CURRENT_BINARY_DIR}/src/ PARENT_SCOPE)
set(LUA_LIBRARIES liblua PARENT_SCOPE)

set(LUA_COMPILER $<TARGET_FILE:luac> PARENT_SCOPE)
set(LUA_FLAGS_NONE "" PARENT_SCOPE)
set(LUA_FLAGS_DEBUG "" PARENT_SCOPE)
set(LUA_FLAGS_MEMCHECK "" PARENT_SCOPE)
set(LUA_FLAGS_RELEASE "-s" PARENT_SCOPE)
set(LUA_FLAGS_RELWITHDEBINFO "" PARENT_SCOPE)
set(LUA_FLAGS_MINSIZEREL "-s" PARENT_SCOPE)

set(LUA_DEPENDENCY liblua luac PARENT_SCOPE)
