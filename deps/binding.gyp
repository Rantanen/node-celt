# Build external deps.
{
    'variables': { 'target_arch%': 'x64' },

    'target_defaults': {
        'default_configuration': 'Debug',
        'configuration': {
            'Debug': {
                'defines': [ 'DEBUG', '_DEBUG' ],
                'msvs_settings': {
                    'VSSLCompilerTool': {
                        'RuntimeLibrary': 1, #static debug
                    },
                },
            },
            'Release': {
                'defines': [ 'NODEBUG' ],
                'msvs_settings': {
                    'VSSLCompilerTool': {
                        'RuntimeLibrary': 0, #static release
                    },
                },
            },
        },
        'msvs_settings': {
            'VCLinkerTool': {
                'GenerateDebugInformation': 'true',
            },
        },
    },

    'targets': [
        {
            'target_name': 'libcelt',
            'type': 'static_library',
            'sources': [
                'celt-0.7.1/libcelt/bands.c',
                'celt-0.7.1/libcelt/celt.c',
                'celt-0.7.1/libcelt/cwrs.c',
                #'celt-0.7.1/libcelt/dump_modes.c',
                'celt-0.7.1/libcelt/ecintrin.h',
                'celt-0.7.1/libcelt/entcode.c',
                'celt-0.7.1/libcelt/entdec.c',
                'celt-0.7.1/libcelt/entenc.c',
                'celt-0.7.1/libcelt/header.c',
                'celt-0.7.1/libcelt/kiss_fft.c',
                'celt-0.7.1/libcelt/laplace.c',
                'celt-0.7.1/libcelt/mdct.c',
                'celt-0.7.1/libcelt/modes.c',
                'celt-0.7.1/libcelt/pitch.c',
                #'celt-0.7.1/libcelt/plc.c',
                'celt-0.7.1/libcelt/quant_bands.c',
                'celt-0.7.1/libcelt/rangedec.c',
                'celt-0.7.1/libcelt/rangeenc.c',
                'celt-0.7.1/libcelt/rate.c',
                #'celt-0.7.1/libcelt/testcelt.c',
                'celt-0.7.1/libcelt/vq.c',
            ],
            'cflags': [
                '-fvisibility=hidden',
                '-W',
                '-Wstrict-prototypes',
                '-Wall',
                '-Wextra',
                '-Wcast-align',
                '-Wnested-externs',
                '-Wshadow',
                '-Wno-parentheses',
                '-Wno-unused-parameter',
                '-Wno-sign-compare',
            ],
            'include_dirs': [
                'config/celt-0.7.1/<(OS)/<(target_arch)',
                'celt-0.7.1/libcelt'
                'celt-0.7.1'
            ],
            'defines': [
                'PIC',
                'HAVE_CONFIG_H',
            ]
        }
    ]
}
