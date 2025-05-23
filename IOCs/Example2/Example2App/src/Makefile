TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Example2
# Example2.dbd will be created and installed
DBD += Example2.dbd

# Example2.dbd will be made up from these files:
Example2_DBD += base.dbd

# Include dbd files for qsrv2 server
Example2_DBD += pvxsIoc.dbd
# Add libraries for qsrv server and PVAccess
Example2_LIBS += pvxs pvxsIoc
Example2_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Include dbd files from all support applications:
#Example2_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#Example2_LIBS += xxx

# Example2_registerRecordDeviceDriver.cpp derives from Example2.dbd
Example2_SRCS += Example2_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Example2_SRCS_DEFAULT += Example2Main.cpp
Example2_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Example2_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Example2_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

