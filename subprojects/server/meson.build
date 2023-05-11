project(
    'server',
    'c',
    version: '0.1',
    default_options: ['warning_level=3', 'c_std=c11'],
)

inc = include_directories('include')

src = [
    'src/main.c',
    'src/parse_args.c',
    'src/sigint_utils.c',
    'src/shm_utils.c',
    'src/sem_utils.c',
]

libconnect4_proj = subproject('libconnect4')
libconnect4_dep = libconnect4_proj.get_variable('libconnect4_dep')

exe = executable(
    'connect4-server',
    src,
    include_directories: inc,
    dependencies: libconnect4_dep,
    install: true,
)

test('basic', exe, should_fail: true)

tests = [
    ['parse_args', ['test/parse_args.c', 'src/parse_args.c']],
]

foreach test : tests
    test_name = test[0]
    test_src = test[1]

    test_exe = executable(
        test_name,
        test_src,
        include_directories: inc,
        dependencies: libconnect4_dep,
        c_args: [
            '-Wno-discarded-qualifiers',
            '-Wno-ignored-qualifiers',
            '-Wno-missing-prototypes',
            '-Wno-missing-declarations',
        ],
    )

    test(test_name, test_exe)
endforeach
