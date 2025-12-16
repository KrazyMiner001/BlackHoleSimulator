project('BlackHoleSimulator', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

sources = files(
        'src/main.cpp'
)

dependencies = [
        dependency('sdl3'),
        dependency('glm'),
        dependency('imgui'),
        dependency('glbinding'),
        dependency('gl'),
]

BlackHoleSimulator = executable(
        'BlackHoleSimulator',
        sources,
        dependencies : dependencies,
        install : true
)

test('test', BlackHoleSimulator)