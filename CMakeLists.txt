cmake_minimum_required(VERSION 3.12)
project(just_post CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)
include(GNUInstallDirs)

add_subdirectory(third_party/userver)


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/hello/hello.hpp
    src/hello/hello.cpp
    src/echo/echo.hpp
    src/echo/echo.cpp

    src/users/sign_up.hpp
    src/users/sign_up.cpp
    src/users/get_id.hpp
    src/users/get_id.cpp
    src/users/change_passwd.cpp
    src/users/change_passwd.hpp
    src/users/sign_in.cpp
    src/users/sign_in.hpp

    src/user_data/add_user_data.hpp
    src/user_data/add_user_data.cpp
    src/user_data/update_user_data.hpp
    src/user_data/update_user_data.cpp
    
    src/posts/create_post.hpp
    src/posts/create_post.cpp
    src/posts/get_last_n_post_id.cpp
    src/posts/get_last_n_post_id.hpp
    src/posts/get_post_info.cpp
    src/posts/get_post_info.hpp

    src/tools/verify_parameter.cpp
    src/tools/verify_parameter.hpp

    src/posts/delete_post.cpp
    src/posts/delete_post.hpp

    src/comments/create_comment.cpp
    src/comments/create_comment.hpp
    src/comments/delete_comment.cpp
    src/comments/delete_comment.hpp
    src/comments/get_comment_info.cpp
    src/comments/get_comment_info.hpp
    src/comments/get_n_comments.cpp
    src/comments/get_n_comments.hpp
)


target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver-core userver-postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/hello/hello_test.cpp
)
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver-utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Benchmarks
add_executable(${PROJECT_NAME}_benchmark
	src/hello/hello_benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver-ubench)
add_google_benchmark_tests(${PROJECT_NAME}_benchmark)

# Functional Tests
add_subdirectory(tests)

if(DEFINED ENV{PREFIX})
	message(STATUS "Set install prefix: $ENV{PREFIX}")
	file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
	set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

set(CONFIG_VAR_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/config_vars.yaml)
set(CONFIG_FALLBACK_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/dynamic_config_fallback.json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml)

FILE(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
