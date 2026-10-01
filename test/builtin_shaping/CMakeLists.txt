add_executable(BuiltinShapingTest
  BuiltinShapingTest.cpp
  ${REPO_ROOT}/lib/EpdFont/BuiltinShaping.cpp
  ${REPO_ROOT}/lib/EpdFont/ComplexShaper.cpp
  ${REPO_ROOT}/lib/EpdFont/EpdFont.cpp
  ${REPO_ROOT}/lib/EpdFont/FlashBlobCache.cpp
  ${REPO_ROOT}/lib/EpdFont/FontAlloc.c
  ${REPO_ROOT}/lib/Utf8/Utf8.cpp
  ${REPO_ROOT}/lib/Utf8/IndicReorder.cpp
  ${OT_SHAPER_SOURCES}
)
target_include_directories(BuiltinShapingTest PRIVATE
  ${REPO_ROOT}/lib/EpdFont
  ${REPO_ROOT}/lib/Utf8
  ${OT_SHAPER_INCLUDE_DIR}
  ${REPO_ROOT}/test/complex_shaper/stubs
)
target_link_libraries(BuiltinShapingTest PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(BuiltinShapingTest)
