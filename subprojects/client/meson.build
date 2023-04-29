project(
    'client',
    'c',
    version: '0.1',
    default_options: ['warning_level=3', 'c_std=c11'],
)

src = ['src/main.c']

libconnect4_proj = subproject('libconnect4')
libconnect4_dep = libconnect4_proj.get_variable('libconnect4_dep')

exe = executable(
    'connect4-client',
    src,
    dependencies: libconnect4_dep,
    install: true,
)

test('basic', exe)
