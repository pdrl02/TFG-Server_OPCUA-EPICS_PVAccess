# Environment configuration
# Set the EPICS base directory and host architecture
epicsEnvSet("EPICS_BASE", "/home/pdrl/TFG-Server_OPCUA-EPICS_PVAccess/libs/base-7.0.8.1")  
epicsEnvSet("EPICS_HOST_ARCH", "linux-x86_64")

# Configure the IOC
epicsEnvSet("IOC", "Example2")
epicsEnvSet("TOP", "$(PWD)/Example2")
epicsEnvSet("DBD", "$(TOP)/dbd")

# Define configuration variables
epicsEnvSet("EPICS_PVA_SERVER_PORT", "5064")      # PVA port
epicsEnvSet("EPICS_PVAS_BEACON_PERIOD", "5.0")     # Beacon period

# Load the device database
dbLoadDatabase("$(DBD)/Example2.dbd")

# Register the device drivers
Example2_registerRecordDeviceDriver(pdbbase)

# Load the records from PVDatabase.db
dbLoadRecords("$(TOP)/Example2App/Db/PVDatabase.db")

# Initialize the IOC
iocInit()
