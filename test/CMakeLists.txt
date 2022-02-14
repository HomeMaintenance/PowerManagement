add_executable(PowerManager_test
    test.cpp
    TestBatteryInverter.h
    TestBatteryInverter.cpp
    TestSink.h
    TestSource.h
    )
target_include_directories(PowerManager_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PowerManager_test PUBLIC PowerManagement)
