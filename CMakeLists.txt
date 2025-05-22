cmake_minimum_required(VERSION 3.10)

#Command for VSCode to detect libraries
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "CMake generator" FORCE)

project(TFG-ServerOPCUA-PVAccess)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source and compilation directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# OPC UA SDK library directories
set(OPCUA_SDK_DIR "libs/uasdkcppbundlepubsub-bin-EVAL-debian12-x64-gcc12.2.0-v1.8.6-641")
set(OPCUA_INCLUDE_DIR "${OPCUA_SDK_DIR}/include")
set(OPCUA_LIB_DIR "${OPCUA_SDK_DIR}/lib")

# EPICS library directories
set(EPICS_BASE_DIR "libs/base-7.0.8.1")
set(EPICS_LIB_DIR ${EPICS_BASE_DIR}/lib/linux-x86_64)
set(EPICS_INCLUDE_DIR ${EPICS_BASE_DIR}/include)

# PVXS library directories
set(PVXS_BASE_DIR "libs/pvxs")
set(PVXS_LIB_DIR ${PVXS_BASE_DIR}/lib/linux-x86_64)
set(PVXS_INCLUDE_DIR ${PVXS_BASE_DIR}/include)

# Specify the compilation folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin")

# Include header directories
include_directories(
                    # Own directories
                    ${INCLUDE_DIR}
                    ${INCLUDE_DIR}/utilities
                    ${INCLUDE_DIR}/app
                    # OPC UA SDK directories
                    ${OPCUA_INCLUDE_DIR}
                    ${OPCUA_INCLUDE_DIR}/uastack
                    ${OPCUA_INCLUDE_DIR}/uabasecpp
                    ${OPCUA_INCLUDE_DIR}/uapkicpp
                    ${OPCUA_INCLUDE_DIR}/xmlparsercpp
                    ${OPCUA_INCLUDE_DIR}/uaservercpp
                    # EPICS directories
                    ${EPICS_INCLUDE_DIR}
                    ${EPICS_INCLUDE_DIR}/os/Linux
                    ${EPICS_INCLUDE_DIR}/compiler/gcc
                    # PVXS directories
                    ${PVXS_INCLUDE_DIR}

)

# Create executables
add_executable(Prueba
    ${SRC_DIR}/main.cpp
    # App
    ${SRC_DIR}/app/myNodeIOEventManager.cpp
    ${SRC_DIR}/app/EPICStoOPCUAGateway.cpp
    ${SRC_DIR}/app/opcServer.cpp
    ${SRC_DIR}/app/PVDiscovery.cpp
    # Utilities
    ${SRC_DIR}/utilities/shutdown.cpp
    ${SRC_DIR}/utilities/iocBasicObject.cpp
)

# Define paths to libraries for executables
target_link_directories(Prueba PRIVATE ${OPCUA_LIB_DIR} ${EPICS_LIB_DIR} ${PVXS_LIB_DIR})

# Link required libraries
set(OPCUA_LIBS 
        libuamoduled.a
        libcoremoduled.a
        libuapkicpp.a
        libuabasecppd.a
        libuastackd.so
        libxmlparsercppd.a
        -lssl
        -lxml2
        -lcrypto)

set(EPICS_LIBS
        pvAccess
        pvData
        Com
        pvaClient)

set(PVXS_LIBS
        pvxs)

target_link_libraries( Prueba ${OPCUA_LIBS} ${EPICS_LIBS} ${PVXS_LIBS})

# Make necessary definitions 
target_compile_definitions(Prueba 
    PRIVATE
    OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15=1
    OPCUA_SUPPORT_SECURITYPOLICY_BASIC256=1
    OPCUA_SUPPORT_SECURITYPOLICY_NONE=1
    OPCUA_SUPPORT_PKI=1
    SUPPORT_XML_PARSER=1
    _UA_STACK_USE_DLL
)

