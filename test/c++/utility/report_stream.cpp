#include <triqs/test_tools/arrays.hpp>
#include <triqs/utility/report_stream.hpp>
#include <sstream>

using triqs::utility::report_stream;

TEST(ReportStreamTest, WritesWhenVerbosityPositive) {
  std::ostringstream output;
  report_stream stream(output, 1);
  stream << "Hello World!";
  EXPECT_EQ(output.str(), "Hello World!");
}

TEST(ReportStreamTest, SkipsWriteWhenVerbosityZero) {
  std::ostringstream output;
  report_stream stream(output, 0);
  stream << "This should not appear.";
  EXPECT_EQ(output.str(), "");
}

TEST(ReportStreamTest, WritesMultipleValues) {
  std::ostringstream output;
  report_stream stream(output, 2);
  stream << "Value: " << 42 << ", Pi: " << 3.14;
  EXPECT_EQ(output.str(), "Value: 42, Pi: 3.14");
}

TEST(ReportStreamTest, AppliesEndlCorrectly) {
  std::ostringstream output;
  report_stream stream(output, 1);
  stream << "Line1" << std::endl << "Line2";
  EXPECT_EQ(output.str(), "Line1\nLine2");
}

TEST(ReportStreamTest, OperatorCallReducesVerbosity) {
  std::ostringstream output;
  report_stream stream(output, 3);
  auto lower_verbosity = stream(2);
  lower_verbosity << "Visible";
  EXPECT_EQ(output.str(), "Visible");
}

TEST(ReportStreamTest, NestedVerbosityDropsBelowZero) {
  std::ostringstream output;
  report_stream stream(output, 1);
  auto silent = stream(3);
  silent << "This should not be printed.";
  EXPECT_EQ(output.str(), "");
}

TEST(ReportStreamTest, PointerConstructorStillWorks) {
  std::ostringstream output;
  report_stream stream(&output, 1);
  stream << "Pointer ctor still works";
  EXPECT_EQ(output.str(), "Pointer ctor still works");
}

MAKE_MAIN;
