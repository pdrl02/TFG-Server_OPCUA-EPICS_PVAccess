#!/bin/bash

# Se necesita screen para ejecutar el script.

# Variables de entorno
export EPICS_BASE=/home/pdrl/TFG-Server_OPCUA-EPICS_PVAccess/libs/base-7.0.8.1
export EPICS_HOST_ARCH=linux-x86_64
export LD_LIBRARY_PATH=$EPICS_BASE/lib/$EPICS_HOST_ARCH:$LD_LIBRARY_PATH
export PATH=$EPICS_BASE/bin/$EPICS_HOST_ARCH:$PATH

# Función para manejar la señal de Ctrl+C (SIGINT)
cleanup() {
    echo "Recibido Ctrl+C. Deteniendo los IOCs..."
    # Matar todos los IOCs usando screen -X
    for pid in "${pids[@]}"; do
        echo "Matar IOC en la sesión screen: $pid"
        screen -X -S $pid quit  # Detener la sesión de screen
    done
    exit 0
}

# Inicializar un arreglo para almacenar los nombres de las sesiones de screen
pids=()

# Iniciar el primer IOC en un "screen" y almacenar el nombre de la sesión
echo "Iniciando IOC Example1"
screen -dmS Example1 ./Example1/bin/linux-x86_64/Example1 Example1/iocBoot/iocExample1/st.cmd
pids+=("Example1")  

echo "Iniciando IOC Example2"
screen -dmS Example2 ./Example2/bin/linux-x86_64/Example2 Example2/iocBoot/iocExample2/st.cmd
pids+=("Example2")

echo "Iniciando IOC Example3"
screen -dmS Example3 ./Example3/bin/linux-x86_64/Example3 Example3/iocBoot/iocExample3/st.cmd
pids+=("Example3")

# Si tienes más IOCs, puedes agregarlos aquí, por Example:
# screen -dmS Example2 ./Example2/bin/linux-x86_64/Example2 Example2/iocBoot/iocExample2/st.cmd
# pids+=("Example2")  # Almacenar el nombre de la sesión del segundo IOC

# Registrar el manejador de la señal Ctrl+C
trap cleanup SIGINT

# Esperar a que los procesos de los IOCs terminen
echo "Esperando a que los IOCs terminen. Para detenerlos, presiona Ctrl+C."

# Este comando mantiene el script activo hasta que se presiona Ctrl+C
while true; do
    sleep 1  # Mantener el script en ejecución
done
