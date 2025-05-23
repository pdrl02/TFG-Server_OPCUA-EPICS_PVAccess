# Configure environment
# Set EPICS base directory and architecture
epicsEnvSet("EPICS_BASE", "/home/pdrl/TFG-Server_OPCUA-EPICS_PVAccess/libs/base-7.0.8.1")  
epicsEnvSet("EPICS_HOST_ARCH", "linux-x86_64")

# Configure the IOC
epicsEnvSet("IOC", "Example1")
epicsEnvSet("TOP", "$(PWD)/Example1")
epicsEnvSet("DBD", "$(TOP)/dbd")

# Define configuration variables
#epicsEnvSet("EPICS_PVA_SERVER_PORT", "5064")  # Puerto PVA
epicsEnvSet("EPICS_PVAS_BEACON_PERIOD", "5.0")  # Período de beacon

# Load database
dbLoadDatabase("$(DBD)/Example1.dbd")

# Register device's drivers.
Example1_registerRecordDeviceDriver(pdbbase)

# Load registries from PVDatabase.db
dbLoadRecords("$(TOP)/Example1App/Db/PVDatabase.db")

# Start the IOC
iocInit()

