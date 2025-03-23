#include "EPICStoOPCUAGateway.h"
#include "iostream"

// Lo que ejecuta el hilo de procesamiento, es decir, los hilos consumidores los workers 
void EPICStoOPCUAGateway::processQueue() {

    while(m_running){
        cout << "Soy: " << this_thread::get_id() << " y he llegado a proccessQueue." << endl;
        // Obtener el siguiente elemento de la cola
        auto item = m_workQueue.pop();

        // Introducimos "" en el nombre como centinela para parar el hilo
        if(!m_running && item.first.empty())
            break;

        try {
            const auto & pvName = item.first;
            const auto & value = item.second;

            auto it = m_pvMap.find(pvName);
            if(it != m_pvMap.end()){
                cout << "Thread nº " << this_thread::get_id << ": Ejecutando función processQueue" << endl;
                // Convertir dato de epics a opcua
                UaVariant variant = convertValueToVariant(value);
                // Actualizar el valor en opcua
                m_pNodeManager->updateVariable(it->second.nodeId, variant);
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
    try {
        cout << "LLegamos a converValueToVariant" << endl;
        switch(value.type().code){
            case TypeCode::Bool:
                cout << value.as<bool>() << endl;
                variant.setBool(value.as<bool>());
                break;
            
            case TypeCode::Float64:
                cout << value.as<double>() << endl;
                variant.setDouble(value.as<double>());
                break;

            case TypeCode::Int32:
                cout << value.as<int32_t>() << endl;
                variant.setInt32(value.as<int32_t>());
                break;

            case TypeCode::Int64:
                cout << value.as<int64_t>() << endl;
                variant.setInt64(value.as<int64_t>());
                break;

            default:
                cerr << "Error converting EPICS Value to OPCUA Variant" << endl;
                break;
        }
    } catch(exception e){
        cerr << "Error converting EPICS Value to OPCUA Variant" << endl;
    }   
    return variant;
}

EPICStoOPCUAGateway::EPICStoOPCUAGateway(MyNodeIOEventManager* pNodeManager)
    : m_pNodeManager(pNodeManager) {

    m_pvxsContext = Context(Config::from_env().build());

    m_pvMap.insert(pair<string, PVMapping>("obj1.1.Temperature", PVMapping("ejemplo1:Temperature", UaNodeId("obj1.1.Temperature", m_pNodeManager->getNameSpaceIndex()))));
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

        cout << pvMapping.nodeId.identifierString() << endl;
        cout << pvMapping.nodeId.namespaceIndex() << endl;

        auto subscription = m_pvxsContext.monitor(pvMapping.epicsName)
            .event([this, pvName](pvxs::client::Subscription & subscription){
                cout << "Eventoooo: " << endl;    // Esto no va
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

void EPICStoOPCUAGateway::stop() {
    m_running = false;

    // Señales para terminar los hilos
    for(int i = 0; i < m_numThreads; ++i){
        m_workQueue.push(pair<string, Value>("", Value()));
    }

    // Join a los hilos
    for(auto & thread : m_workerThreads)
        if(thread.joinable())
            thread.join();
    
    
    m_workerThreads.clear();
}
