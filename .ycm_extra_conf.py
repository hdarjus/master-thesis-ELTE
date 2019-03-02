def FlagsForFile (filename, **kwargs):
    return {
            'flags': ['-std=c++17',
                '-I', '/home/dhosszejni/.local/include',
                '-I', '/home/dhosszejni/Development/vdf/include',
                '-fopenmp=libomp', '-fpic',
                '-pedantic', '-Wall',
                '-g', '-c'],
            }
