# Configuración de entorno
# Establece el directorio base de EPICS y la arquitectura
epicsEnvSet("EPICS_BASE", "/home/pdrl/TFG-Server_OPCUA-EPICS_PVAccess/libs/base-7.0.8.1")  
epicsEnvSet("EPICS_HOST_ARCH", "linux-x86_64")

# Configura el IOC
epicsEnvSet("IOC", "Ejemplo3")
epicsEnvSet("TOP", "$(PWD)/Ejemplo3")
epicsEnvSet("DBD", "$(TOP)/dbd")

# Define las variables de configuración
#epicsEnvSet("EPICS_PVA_SERVER_PORT", "5064")  # Puerto PVA
epicsEnvSet("EPICS_PVAS_BEACON_PERIOD", "15.0")  # Período de beacon

# Cargar la base de datos de dispositivos
dbLoadDatabase("$(DBD)/Ejemplo3.dbd")

# Registrar los controladores de dispositivos
Ejemplo3_registerRecordDeviceDriver(pdbbase)

# Cargar los registros desde PVDatabase.db
dbLoadRecords("$(TOP)/Ejemplo3App/Db/PVDatabase.db")

# Inicialización del IOC
iocInit()

