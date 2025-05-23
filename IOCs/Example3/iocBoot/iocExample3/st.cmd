# Configure Environment
# Set EPICS base directory and architecture
epicsEnvSet("EPICS_BASE", "/home/pdrl/TFG-Server_OPCUA-EPICS_PVAccess/libs/base-7.0.8.1")  
epicsEnvSet("EPICS_HOST_ARCH", "linux-x86_64")

# Configure IOC
epicsEnvSet("IOC", "Example3")
epicsEnvSet("TOP", "$(PWD)/Example3")
epicsEnvSet("DBD", "$(TOP)/dbd")

# Define configuration variables
#epicsEnvSet("EPICS_PVA_SERVER_PORT", "5064")  # Puerto PVA
epicsEnvSet("EPICS_PVAS_BEACON_PERIOD", "5.0")  # Período de beacon

# Load database
dbLoadDatabase("$(DBD)/Example3.dbd")

# Registry device drivers
Example3_registerRecordDeviceDriver(pdbbase)

# Load registries from PVDatabase.db
dbLoadRecords("$(TOP)/Example3App/Db/PVDatabase.db")

# Init IOC
iocInit()

