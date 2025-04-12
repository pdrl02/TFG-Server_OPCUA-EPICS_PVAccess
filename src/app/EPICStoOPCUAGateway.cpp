#include "EPICStoOPCUAGateway.h"
#include "iostream"

// Lo que ejecuta el hilo de procesamiento, es decir, los hilos consumidores los workers 
void EPICStoOPCUAGateway::processQueue() {

    GatewayHandler handler(this);

    while(m_running.load()){
        cout << "Soy: " << this_thread::get_id() << " y he llegado a processQueue" << endl;
        // Obtener el siguiente elemento de la cola
        auto pEvent = m_workQueue.pop();
        if(pEvent){
            try{
                std::visit(handler, *pEvent);
                //if(m_running.load())
                //    m_workQueue.push(pEvent);
            } catch (exception e) {
                cerr << "Error processing event: " << e.what() << endl;
                //if(m_running.load())
                //    m_workQueue.push(pEvent);
            }
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

Value EPICStoOPCUAGateway::convertUaDataValueToPvxsValue(const UaDataValue& dataValue) {

    UaVariant variant(*dataValue.value());
    Value value;
    try {
        switch (variant.type()){
            case OpcUa_BuiltInType::OpcUaType_Boolean: {
                OpcUa_Boolean opcuaBool;
                variant.toBool(opcuaBool);
                bool rawBool = !(opcuaBool != OpcUa_False);
                value = nt::NTScalar{TypeCode::Bool}.create().update("value", rawBool);
                break;
            }

            case OpcUa_BuiltInType::OpcUaType_Double: {
                double doubleValue;
                variant.toDouble(doubleValue);
                value = nt::NTScalar{TypeCode::Float64}.create().update("value", doubleValue);
                break;
            }

            default: {
                cout <<"variant type:::::" << variant.type() << endl;
                break;
            }
        }
    } catch(exception e){
        cerr << "Error converting OPCUA Variant to EPICS Value" << endl;
        cerr << e.what() << endl;
    }
    return value;
}

EPICStoOPCUAGateway::EPICStoOPCUAGateway(MyNodeIOEventManager* pNodeManager)
    : m_pNodeManager(pNodeManager) {    

    m_pvxsContext = Context(Config::from_env().build());
    addMapping( "ejemplo1:FanSpeed", PVMapping("ejemplo1:FanSpeed", UaNodeId("obj1.1.FanSpeed", m_pNodeManager->getNameSpaceIndex())));
}

EPICStoOPCUAGateway::~EPICStoOPCUAGateway() {
    //stop();
}

void EPICStoOPCUAGateway::start() {
    
    m_running.store(true);

    // Subscribirse a cada PV de los IOC.
    // Deben estar las variables en m_pvMap
    for(const auto & [pvName, pvMapping] : m_pvMapName){

        m_subcriptions.push_back(
            m_pvxsContext.monitor(pvMapping.epicsName)
                .event([this](pvxs::client::Subscription & subscription){
                    auto eventSub = make_shared<GatewayEvent>(subscription.shared_from_this());
                    m_workQueue.push(eventSub);
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

void EPICStoOPCUAGateway::enqueuePutTask(const UaVariable * variable, const UaDataValue& value) {
    
    auto it = m_pvMapUaNode.find(variable->nodeId());
    if(it != m_pvMapUaNode.end()){
        PutRequest request(variable, value);
        auto eventPut = make_shared<GatewayEvent>(request);
        m_workQueue.push(eventPut);
    } else {
        cerr << "Variable not found in the UaNodeId mapping." << endl;
    }
    
}

bool EPICStoOPCUAGateway::addMapping(const string& name, const PVMapping& pvMapping) {
    auto result = m_pvMapName.emplace(name, pvMapping);
    if (result.second) {
        m_pvMapUaNode.emplace(pvMapping.nodeId, pvMapping);
        return true;  
    }
    return false;
}

void EPICStoOPCUAGateway::GatewayHandler::operator()(shared_ptr<Subscription> & subscription) const {
    if(subscription){
        try{
            cout << "Salgo de m_workQueue.pop() " << endl;
            Value value = subscription->pop();
            if(!value)
                return;
            
            cout << subscription->name() << endl;
            auto it = m_self->m_pvMapName.find(subscription->name());
            if(it != m_self->m_pvMapName.end()){
                cout << "Llego a actualizar la variable" << endl;
                // Convert data from EPICS to OPC UA
                UaVariant variant = m_self->convertValueToVariant(value);
                // Update value in server
                m_self->m_pNodeManager->updateVariable(it->second.nodeId, variant);
            }


        } catch (exception e) {
            cerr << "Error: " << e.what() << endl;
        }
        m_self->m_workQueue.push(make_shared<GatewayEvent>(subscription));
    }
}

void EPICStoOPCUAGateway::GatewayHandler::operator()(PutRequest& putRequest) const {
    if(putRequest.variable != nullptr){
        try {
            cout << "Procesando put request" << endl;
    
        } catch (exception e) {
            cerr << "Error in input request handler: " << e.what() << endl;
        }
        m_self->m_workQueue.push(make_shared<GatewayEvent>(putRequest));
    }
}
