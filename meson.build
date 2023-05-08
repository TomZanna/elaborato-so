project(
    'connect4',
    'c',
    version: '0.1',
    default_options: ['warning_level=3', 'c_std=c11'],
)

meson.add_dist_script('tools/pre-dist.sh')

subproject('libconnect4')

subproject('client')
subproject('server')
