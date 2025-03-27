#include "EPICStoOPCUAGateway.h"
#include "iostream"

// Lo que ejecuta el hilo de procesamiento, es decir, los hilos consumidores los workers 
void EPICStoOPCUAGateway::processQueue() {

    while(m_running.load()){
        cout << "Soy: " << this_thread::get_id() << " y he llegado a processQueue" << endl;
        // Obtener el siguiente elemento de la cola
        auto sub = m_workQueue.pop();
        if(sub){
            try{
                cout << "Salgo de m_workQueue.pop() " << endl;
                Value value = sub->pop();
                if(!value)
                    continue;
                
                cout << sub->name() << endl;
                auto it = m_pvMap.find(sub->name());
                if(it != m_pvMap.end()){
                    cout << "Llego a actualizar la variable" << endl;
                    // Convert data from EPICS to OPC UA
                    UaVariant variant = convertValueToVariant(value);
                    // Update value in server
                    m_pNodeManager->updateVariable(it->second.nodeId, variant);
                }


            } catch (exception e) {
                cerr << "Error: " << e.what() << endl;
            }
            m_workQueue.push(sub);
        }
        
    }
}

// Puede devolver un variant vacio!!!!!!!!!
UaVariant EPICStoOPCUAGateway::convertValueToVariant(const Value& value) {
    
    UaVariant variant;
    try {
        TypeCode::code_t code = value.lookup("value").type().code;
        Value valueField = value.lookup("value");
        switch(code){
            case TypeCode::Bool:
                variant.setBool(valueField.as<bool>());
                break;
            
            case TypeCode::Float64:
                variant.setDouble(valueField.as<double>());
                break;

            case TypeCode::Int32:
                variant.setInt32(valueField.as<int32_t>());
                break;

            case TypeCode::Int64:
                variant.setInt64(valueField.as<int64_t>());
                break;

            default:
                cerr << "Error converting EPICS Value to OPCUA Variant" << endl;
                cerr << "Unknown TypeCode" << endl;
                break;
        }
    } catch(exception e){
        cerr << "Error converting EPICS Value to OPCUA Variant" << endl;
        cerr << e.what() << endl;
    }   
    return variant;
}

EPICStoOPCUAGateway::EPICStoOPCUAGateway(MyNodeIOEventManager* pNodeManager)
    : m_pNodeManager(pNodeManager) {    

    m_pvxsContext = Context(Config::from_env().build());

    m_pvMap.insert(pair<string, PVMapping>("ejemplo1:Temperature", PVMapping("ejemplo1:Temperature", UaNodeId("obj1.1.Temperature", m_pNodeManager->getNameSpaceIndex()))));
}

EPICStoOPCUAGateway::~EPICStoOPCUAGateway() {
    //stop();
}

void EPICStoOPCUAGateway::addMapping(const string& pvName, const UaNodeId& nodeId) {
    pair<string, PVMapping> in(pvName, PVMapping(pvName, nodeId));
    m_pvMap.insert(in);
}

void EPICStoOPCUAGateway::start() {
    
    m_running.store(true);

    // Subscribirse a cada PV de los IOC.
    // Deben estar las variables en m_pvMap
    for(const auto & [pvName, pvMapping] : m_pvMap){

        m_subcriptions.push_back(
            m_pvxsContext.monitor(pvMapping.epicsName)
                .event([this](pvxs::client::Subscription & subscription){
                   m_workQueue.push(subscription.shared_from_this());
                }).exec()
        );
    }

    // Iniciar los hilos de procesamiento
    for(int i = 0; i<m_numThreads; ++i){
        m_workerThreads.push_back(thread([this](){processQueue();}));
    }

}

void EPICStoOPCUAGateway::stop() {
    
    m_running.store(false);

    // Señales para terminar los hilos
    for(int i = 0; i < m_numThreads; ++i){
        m_workQueue.push(nullptr);
    }

    // Join a los hilos
    for(auto & thread : m_workerThreads)
        if(thread.joinable())
            thread.join();
    
    
    m_workerThreads.clear();
}
