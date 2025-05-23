TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Example1
# Example1.dbd will be created and installed
DBD += Example1.dbd

# Example1.dbd will be made up from these files:
Example1_DBD += base.dbd

# Include dbd files from all support applications:
#Example1_DBD += xxx.dbd

# Include dbd files for qsrv2 server 
Example1_DBD += pvxsIoc.dbd

# Add libraries for qsrv server and PVAccess
Example1_LIBS += pvxs pvxsIoc
Example1_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Add all the support libraries needed by this IOC
#Example1_LIBS += xxx

# Example1_registerRecordDeviceDriver.cpp derives from Example1.dbd
Example1_SRCS += Example1_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Example1_SRCS_DEFAULT += Example1Main.cpp
Example1_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Example1_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Example1_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

