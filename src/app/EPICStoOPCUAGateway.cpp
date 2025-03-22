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
                cout << "Thread nº " << this_thread::get_id << ": Ejecutando función processQueue" << endl;
                // Convertir dato de epics a opcua
                UaVariant variant = convertValueToVariant(value);
                // Actualizar el valor en opcua
                //m_pNodeManager->updateVariable(it->second.nodeId, variant);
            }
        } catch(exception e){
            cerr << "Error processing value change: " << e.what() << endl;
            cerr << "Possible data inconsistency between the IOC and the server" << endl;
        }

    }
}

// Puede devolver un variant vacio!!!!!!!!!
UaVariant EPICStoOPCUAGateway::convertValueToVariant(const Value& value) {
    
    UaVariant variant;

    if(!variant.isArray() && !variant.isEmpty()){
        try {
            switch(value.type().code){
                case TypeCode::Bool:
                    variant.setBool(value.as<bool>());
                    break;
                
                case TypeCode::Float64:
                    variant.setDouble(value.as<double>());
                    break;

                case TypeCode::Int32:
                    variant.setInt32(value.as<int32_t>());
                    break;

                case TypeCode::Int64:
                    variant.setInt64(value.as<int64_t>());
                    break;

                default:
                    break;
            }
        } catch(exception e){
            cerr << "Error converting EPICS Value to OPCUA Variant" << endl;
        }   
    }

    return variant;
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

void EPICStoOPCUAGateway::start() {
    
    m_running = true;

    // Subscribirse a cada PV de los IOC.
    // Deben estar las variables en m_pvMap
    for(const auto & [pvName, pvMapping] : m_pvMap){

        auto subscription = m_pvxsContext.monitor(pvMapping.epicsName)
            .event([this, pvName](pvxs::client::Subscription & subscription){
                try {
                    auto value = subscription.pop();
                    if(value.valid())
                        m_workQueue.push(pair<string, Value>(pvName, value));
                } catch (exception e){
                    cerr << "Error in subscription for: " << pvName << endl;
                    cerr << "Error type: " << e.what() << endl;
                }
            }).exec(); 
    }

    // Iniciar los hilos de procesamiento
    for(int i = 0; i<m_numThreads; ++i){
        m_workerThreads.push_back(thread([this](){processQueue();}));
    }



}
