tool_tests = [
  'bt',
  'lpc',
  'pci',
  'net',
  'updater',
  'helper']

foreach t : tool_tests
  test(
    t,
    executable(
      t.underscorify(), 'tools_' + t + '_unittest.cpp',
      build_by_default: false,
      implicit_include_directories: false,
      include_directories: [root_inc, tools_inc],
      dependencies: [updater_dep, gtest, gmock]))
endforeach

io_test = executable(
  'io',
  'io_unittest.cpp',
  build_by_default: false,
  implicit_include_directories: false,
  include_directories: [root_inc, tools_inc],
  dependencies: [updater_dep, gtest, gmock])
test('io', io_test)
