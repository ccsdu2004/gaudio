project(
  'gaudio',
  ['c', 'cpp'],
  license : 'MIT',
  meson_version : '>= 0.52'
)

include_directories('.')
include_directories('..\\gaudio-sdk\\include')
include_directories('libresample')
include_directories('bs2b')

files = [
        'g168effect.cpp'
		,'g3dcalc.cpp'
		,'gaddons.cpp'
		,'gaddress.cpp'
		,'gaiffile.cpp'
		,'gaudio.cpp'
		,'gaudio_private.h'
		,'gaufile.cpp'
		,'gcontext.cpp'
		,'gdecoderfactory.cpp'
		,'gdecpprocess.cpp'
		,'gdevice.cpp'
		,'geffect.cpp'
		,'geffectfactory.cpp'
		,'gerror.cpp'
		,'gfade.cpp'
		,'gfilter.cpp'
		,'glistener.cpp'
		,'gloop.cpp'
		,'gmain.cpp'
		,'gmeta.cpp'
		,'gmeta.h'
		,'gmixer.cpp'
		,'gmutex.cpp'
		,'gpanprocess.cpp'
		,'gqueue.cpp'
		,'grawpcm.cpp'
		,'grecorder.cpp'
		,'grecorder.h'
		,'grecorderfactory.cpp'
		,'gresample.cpp'
		,'gsource.cpp'
		,'gsourcecls.cpp'
		,'gspeedchanger.cpp'
		,'gstring.cpp'
		,'gthread.cpp'
		,'gwavefile.cpp'
		,'gwriter.cpp'
		,'gwriterfactory.cpp'
		,'streamBuffer.cpp'
		,'vadilator.cpp'
]

shared_library('gaudio',files)