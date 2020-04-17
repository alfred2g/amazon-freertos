    project ("taskpool unit test")
    cmake_minimum_required (VERSION 3.13)

    set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
    set(freertos_plus_dir "${AFR_ROOT_DIR}/libraries/freertos_plus")
    set(abstraction_dir "${AFR_ROOT_DIR}/libraries/abstractions")
    set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")
    set(common_dir "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common")

    list(APPEND mock_list
                "${kernel_dir}/include/timers.h"
                "${kernel_dir}/include/queue.h"
                "${kernel_dir}/include/task.h"
            )
    create_mock_list(taskpool_mock "${mock_list}"
            )
    target_include_directories(taskpool_mock  PUBLIC
            )
    target_compile_definitions(taskpool_mock PUBLIC
                configSUPPORT_STATIC_ALLOCATION=1
            )
    target_compile_definitions(taskpool_mock PUBLIC
                portHAS_STACK_OVERFLOW_CHECKING=1
            )
    target_compile_definitions(taskpool_mock PUBLIC
                portUSING_MPU_WRAPPERS=1
            )
    target_compile_definitions(taskpool_mock PUBLIC
                MPU_WRAPPERS_INCLUDED_FROM_API_FILE
            )

    add_library(taskpool_real STATIC
                "../taskpool_freertos.c"
            )

    target_include_directories(taskpool_real PUBLIC
                .
                ..
                ../include
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
                "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
                "${AFR_ROOT_DIR}/freertos_kernel/include/"
                "${CMAKE_CURRENT_BINARY_DIR}/mocks"
            )
    target_compile_definitions(taskpool_real PUBLIC
                configTASKPOOL_NUMBER_OF_WORKERS=1
            )
    target_compile_definitions(taskpool_real PUBLIC
                configTASKPOOL_WORKER_PRIORITY=2
            )

    set_target_properties(taskpool_real PROPERTIES
                COMPILE_FLAGS "-Wall -fPIC -ggdb3 -Og \
                    -fprofile-arcs -ftest-coverage -fprofile-generate \
                    -include portableDefs.h -Wno-unused-but-set-variable"
                LINK_FLAGS "-fPIC -fprofile-arcs -ftest-coverage \
                    -fprofile-generate -ggdb3 -Og"
                ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
            )

    add_dependencies(taskpool_real taskpool_mock)
    target_link_libraries(taskpool_real PUBLIC
                          -ltaskpool_mock
                          -lgcov
            )
    list(APPEND link_list
                -ltaskpool_mock
                libtaskpool_real.a
                libutils.so
            )
    list(APPEND dep_list
                taskpool_real
            )
    create_test(taskpool_utest
                taskpool_utest.c
                "${link_list}"
                "${dep_list}"
            )
    target_include_directories(taskpool_utest PUBLIC
                ../
                ../include
                "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
                "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
            )
    target_compile_definitions(taskpool_utest PUBLIC
                configTASKPOOL_NUMBER_OF_WORKERS=1
            )
    target_compile_definitions(taskpool_utest PUBLIC
                TASKPOOL_KEEP_COUNTS=1
            )
