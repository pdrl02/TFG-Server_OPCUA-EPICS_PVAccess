#include "EPICStoOPCUAGateway.h"
#include "iostream"

// Lo que ejecuta el hilo de procesamiento, es decir, los hilos consumidores los workers 
void EPICStoOPCUAGateway::processQueue() {

    while(m_running){
        // Obtener el siguiente elemento de la cola
        auto item = m_workQueue.pop();

        try {
            const auto & pvName = item.first;
            const auto & value = item.second;

            auto it = m_pvMap.find(pvName);
            if(it != m_pvMap.end()){
                // Convertir dato de epics a opcua
                UaVariant variant = valueToVariant(value);
                // Actualizar el valor en opcua
                //m_pNodeManager->updateVariable(it->second.nodeId, variant);
            }
        } catch(exception e){
            cerr << "Error processing value change: " << e.what() << endl;
            cerr << "Possible data inconsistency between the IOC and the server" << endl;
        }

    }
}

EPICStoOPCUAGateway::EPICStoOPCUAGateway(MyNodeIOEventManager* pNodeManager)
    : m_pNodeManager(pNodeManager) {
  for (int i = 0; i < m_numThreads; ++i) {
    // m_workerThreads.push_back(new thread())
  }
}

EPICStoOPCUAGateway::~EPICStoOPCUAGateway() {}

void EPICStoOPCUAGateway::addMapping(const string& pvName, const UaNodeId& nodeId) {
    pair<string, PVMapping> in(pvName, PVMapping(pvName, nodeId));
    m_pvMap.insert(in);
}
