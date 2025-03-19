TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Ejemplo1
# Ejemplo1.dbd will be created and installed
DBD += Ejemplo1.dbd

# Ejemplo1.dbd will be made up from these files:
Ejemplo1_DBD += base.dbd

# Include dbd files from all support applications:
#Ejemplo1_DBD += xxx.dbd

# Include dbd files for qsrv2 server 
Ejemplo1_DBD += pvxsIoc.dbd

# Add libraries for qsrv server and PVAccess
Ejemplo1_LIBS += pvxs pvxsIoc
Ejemplo1_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Add all the support libraries needed by this IOC
#Ejemplo1_LIBS += xxx

# Ejemplo1_registerRecordDeviceDriver.cpp derives from Ejemplo1.dbd
Ejemplo1_SRCS += Ejemplo1_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Ejemplo1_SRCS_DEFAULT += Ejemplo1Main.cpp
Ejemplo1_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Ejemplo1_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Ejemplo1_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

