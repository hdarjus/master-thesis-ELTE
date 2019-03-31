def FlagsForFile (filename, **kwargs):
    return {
            'flags': ['-x', 'c++', '-std=c++17',
                '-I', '/home/dhosszejni/.local/include',
                '-D', '_DEBUG',
                #'-Wpedantic', '-Wall', '-Wextra', '-Wconversion',
                '-g', '-c', '-O0'],
            }
