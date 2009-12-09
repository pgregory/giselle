#global defaults
isEmpty(SWIG_DIR):SWIG_DIR = .
isEmpty(SWIG_CMD):SWIG_CMD = swig -lua -c++

#dependency to generate *_wrap.cxx from *.i
swig_cxx.name = SWIG_CXX ${QMAKE_FILE_IN}
swig_cxx.commands = $$SWIG_CMD $$SWIG_FLAGS -o $$SWIG_DIR/${QMAKE_FILE_BASE}_wrap.cxx ${QMAKE_FILE_NAME}
swig_cxx.variable_out = SOURCES
swig_cxx.output = $$SWIG_DIR/${QMAKE_FILE_BASE}_wrap.cxx
swig_cxx.input = SWIG_FILES
swig_cxx.clean = $$SWIG_DIR/${QMAKE_FILE_BASE}_wrap.cxx
QMAKE_EXTRA_COMPILERS += swig_cxx
