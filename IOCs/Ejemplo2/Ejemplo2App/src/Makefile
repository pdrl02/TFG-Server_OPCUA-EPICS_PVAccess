TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Ejemplo2
# Ejemplo2.dbd will be created and installed
DBD += Ejemplo2.dbd

# Ejemplo2.dbd will be made up from these files:
Ejemplo2_DBD += base.dbd

# Include dbd files from all support applications:
#Ejemplo2_DBD += xxx.dbd

# Include dbd files for qsrv server and PVAccess
Ejemplo2_DBD += PVAServerRegister.dbd
Ejemplo2_DBD += qsrv.dbd

# Add libraries for qsrv server and PVAccess
Ejemplo2_LIBS += qsrv
Ejemplo2_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Add all the support libraries needed by this IOC
#Ejemplo2_LIBS += xxx

# Ejemplo2_registerRecordDeviceDriver.cpp derives from Ejemplo2.dbd
Ejemplo2_SRCS += Ejemplo2_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Ejemplo2_SRCS_DEFAULT += Ejemplo2Main.cpp
Ejemplo2_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Ejemplo2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Ejemplo2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

