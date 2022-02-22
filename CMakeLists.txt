cmake_minimum_required(VERSION 3.20)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

project(PowerManagement)

add_subdirectory(test)

add_library(
    PowerManagement
    PowerSource.h
    PowerSource.cpp
    PowerSink.h
    PowerSink.cpp
    PowerRange.h
    PowerRange.cpp
    BatteryManager.h
    BatteryManager.cpp
    BatteryInverter.h
    BatteryInverter.cpp
    PowerManager.h
    PowerManager.cpp
    PowerGrid.h)
target_include_directories(PowerManagement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(PowerManagement PRIVATE PWR_MGNT_DEBUG=1)

find_path(CPP_HTTPLIB_INCLUDE_DIRS "httplib.h")
target_include_directories(PowerManagement PUBLIC ${CPP_HTTPLIB_INCLUDE_DIRS})

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(PowerManagement PUBLIC
    jsoncpp_lib
    jsoncpp_object
    JsonCpp::JsonCpp
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia
    )
    set(DOXYGEN_EXTRACT_PRIVATE TRUE)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED FALSE)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C TRUE)

    doxygen_add_docs(doxygen_target
        ${CMAKE_SOURCE_DIR}
        COMMENT "Run doxygen_target"
    )
endif()
