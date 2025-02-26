cmake_minimum_required(VERSION 3.10)

#Comando para VSCode detecte las librerias
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "CMake generator" FORCE)

project(TFG-ServerOPCUA-PVAccess)

# Especificar el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directorios de código fuente y compilación
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Directorios de la librería OPC UA SDK
set(OPCUA_SDK_DIR "libs/uasdkcppbundlepubsub-bin-EVAL-debian12-x64-gcc12.2.0-v1.8.6-641")
set(OPCUA_INCLUDE_DIR "${OPCUA_SDK_DIR}/include")
set(OPCUA_LIB_DIR "${OPCUA_SDK_DIR}/lib")

# Directorios de la librería de EPICS
set(EPICS_BASE_DIR "libs/base-7.0.8.1")
set(EPICS_LIB_DIR ${EPICS_BASE_DIR}/lib/linux-x86_64)
set(EPICS_INCLUDE_DIR ${EPICS_BASE_DIR}/include)

# Especificar la carpeta de compilación
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin")

# Incluir directorios de encabezados
include_directories(
                    # Directorios propios
                    ${INCLUDE_DIR}
                    ${INCLUDE_DIR}/utilities
                    ${INCLUDE_DIR}/app
                    # Directorios de OPC UA SDK
                    ${OPCUA_INCLUDE_DIR}
                    ${OPCUA_INCLUDE_DIR}/uastack
                    ${OPCUA_INCLUDE_DIR}/uabasecpp
                    ${OPCUA_INCLUDE_DIR}/uapkicpp
                    ${OPCUA_INCLUDE_DIR}/xmlparsercpp
                    ${OPCUA_INCLUDE_DIR}/uaservercpp
                    # Directorios de EPICS
                    ${EPICS_INCLUDE_DIR}
                    ${EPICS_INCLUDE_DIR}/os/Linux
                    ${EPICS_INCLUDE_DIR}/compiler/gcc
)

# Crear ejecutables
add_executable(Prueba
    ${SRC_DIR}/main.cpp
    # App
    ${SRC_DIR}/app/myNodeIOEventManager.cpp
    ${SRC_DIR}/app/EPICSClient.cpp
    # Utilities
    ${SRC_DIR}/utilities/opcserver.cpp
    ${SRC_DIR}/utilities/shutdown.cpp
)

# Definir las rutas a las bibliotecas para los ejecutables
target_link_directories(Prueba PRIVATE ${OPCUA_LIB_DIR} ${EPICS_LIB_DIR})

# Enlazar librerías necesarias
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

target_link_libraries( Prueba ${OPCUA_LIBS} ${EPICS_LIBS})

# Realizar las definiciones necesarias 
target_compile_definitions(Prueba 
    PRIVATE
    OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15=1
    OPCUA_SUPPORT_SECURITYPOLICY_BASIC256=1
    OPCUA_SUPPORT_SECURITYPOLICY_NONE=1
    OPCUA_SUPPORT_PKI=1
    SUPPORT_XML_PARSER=1
    _UA_STACK_USE_DLL
)

