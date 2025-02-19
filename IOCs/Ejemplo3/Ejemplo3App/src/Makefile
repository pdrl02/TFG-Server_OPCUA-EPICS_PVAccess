TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Ejemplo3
# Ejemplo3.dbd will be created and installed
DBD += Ejemplo3.dbd

# Ejemplo3.dbd will be made up from these files:
Ejemplo3_DBD += base.dbd

# Include dbd files for qsrv server and PVAccess
Ejemplo3_DBD += PVAServerRegister.dbd
Ejemplo3_DBD += qsrv.dbd


# Add libraries for qsrv server and PVAccess
Ejemplo3_LIBS += qsrv
Ejemplo3_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Include dbd files from all support applications:
#Ejemplo3_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#Ejemplo3_LIBS += xxx

# Ejemplo3_registerRecordDeviceDriver.cpp derives from Ejemplo3.dbd
Ejemplo3_SRCS += Ejemplo3_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Ejemplo3_SRCS_DEFAULT += Ejemplo3Main.cpp
Ejemplo3_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Ejemplo3_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Ejemplo3_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

