project('BlackHoleSimulator', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

sources = files(
  'src/main.cpp',
  'src/shader_program.cpp',
  'src/render_shader.cpp',
  'src/compute_shader.cpp',
  'src/blackhole_renderer.cpp',
  'src/gamepad.cpp',
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
  install : true,
  include_directories : 'include',
)

test('test', BlackHoleSimulator)