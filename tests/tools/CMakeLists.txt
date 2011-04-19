# Add test apps and test executions to this part
# Test apps are compiled as any other app in the
# project. Test executions are run using "make test"
#
IF(BUILD_TESTING)

  # clitkImageInfo
  ADD_EXECUTABLE(clitkImageInfoTest clitkImageInfoTest.cxx)
  TARGET_LINK_LIBRARIES(clitkImageInfoTest ITKIO)
  ADD_TEST(NAME clitkImageInfoTest COMMAND clitkImageInfoTest)

  # clitkWriteDicomSeries
  ADD_EXECUTABLE(clitkWriteDicomSeriesTest clitkWriteDicomSeriesTest.cxx)
  TARGET_LINK_LIBRARIES(clitkWriteDicomSeriesTest ITKIO)
  ADD_TEST(NAME clitkWriteDicomSeriesTest COMMAND clitkWriteDicomSeriesTest)

ENDIF(BUILD_TESTING)
