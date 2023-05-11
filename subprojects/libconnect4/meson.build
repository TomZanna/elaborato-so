project(
    'libconnect4',
    'c',
    version: '0.1',
    default_options: ['warning_level=3', 'c_std=c11'],
)

inc = include_directories('include')

src = ['src/lib.c']

libconnect4 = static_library('libconnect4', src, include_directories: inc)

libconnect4_dep = declare_dependency(
    include_directories: inc,
    link_with: libconnect4,
)

tests = [
    ['parse_uint', ['test/parse_uint.c', 'src/lib.c']],
]

foreach test : tests
    test_name = test[0]
    test_src = test[1]

    test_exe = executable(
        test_name,
        test_src,
        include_directories: inc,
        dependencies: libconnect4_dep,
    )

    test(test_name, test_exe)
endforeach
