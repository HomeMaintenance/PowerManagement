add_executable(test
    test.cpp
    TestBatteryInverter.h
    TestBatteryInverter.cpp
    TestSink.h
    TestSource.h
    )
target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test PUBLIC PowerManagement)
