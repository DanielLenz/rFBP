# If the pyx file is a C++ file, we should specify that here.

set(PYC_SRC_DIR    ${PYC_DIR}/source       CACHE PATH "Path where find pyx files" )
set(PYC_INC_DIR    ${PYC_DIR}/include      CACHE PATH "Path where find pxd files" )

set_source_files_properties( ${PYC_SRC_DIR}/rFBP.pyx PROPERTIES CYTHON_IS_CXX TRUE )

# Include *.pxd directory

include_directories(${PYC_INC_DIR})

# Multi-file cython modules do not appear to be working at the moment.
cython_add_module( rFBP  ${PYC_SRC_DIR}/rFBP.pyx
                                                  ${INC_DIR}/rfbp.h
                                                  ${INC_DIR}/pattern.h
                                                  ${INC_DIR}/utils.h
                                                  ${INC_SCORER}/scorer.h
                                                                        ${SRC_DIR}/rfbp.cpp
                                                                        ${SRC_DIR}/pattern.cpp
                                                                        ${SRC_DIR}/fprotocol.cpp
                                                                        ${SRC_DIR}/utils.cpp
                                                                        )


# Installation of targets
install(TARGETS rFBP            DESTINATION ${LIB_DIR}/ReplicatedFocusingBeliefPropagation)