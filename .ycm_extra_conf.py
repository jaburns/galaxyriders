def FlagsForFile( filename, **kwargs ):
  return {
    'flags': [
        '-x', 'c++',
        '-Iexternal/lodepng',
        '-Iexternal/glm',
        '-Iexternal/tinyxml2',
        '-std=c++14',
        '-lstdc++',
        '-lglfw',
        '-framework OpenGL'
    ],
  }
