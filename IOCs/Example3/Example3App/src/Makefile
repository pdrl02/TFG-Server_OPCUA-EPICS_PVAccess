TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Example3
# Example3.dbd will be created and installed
DBD += Example3.dbd

# Example3.dbd will be made up from these files:
Example3_DBD += base.dbd

# Include dbd files from all support applications:
#Example3_DBD += xxx.dbd

Example3_DBD += pvxsIoc.dbd

Example3_LIBS += pvxs pvxsIoc
Example3_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

# Add all the support libraries needed by this IOC
#Example3_LIBS += xxx

# Example3_registerRecordDeviceDriver.cpp derives from Example3.dbd
Example3_SRCS += Example3_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Example3_SRCS_DEFAULT += Example3Main.cpp
Example3_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Example3_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Example3_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

