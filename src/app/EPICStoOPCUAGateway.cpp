#include "EPICStoOPCUAGateway.h"
#include "iostream"

// Workers execution
void EPICStoOPCUAGateway::processQueue() {

    GatewayHandler handler(this);

    while(m_running.load()){
        //cout << "Soy: " << this_thread::get_id() << " y he llegado a processQueue" << endl;
        // Obtain the next queue element
        auto pEvent = m_workQueue.pop();
        if(pEvent){
            try{
                std::visit(handler, *pEvent);
            } catch (const exception & e) {
                cerr << "Error processing event: " << e.what() << endl;
            }
        }
    }
}


UaVariant EPICStoOPCUAGateway::convertValueToVariant(const Value& value) {
    
    UaVariant variant;
    try {
        Value valueField;
        TypeCode::code_t code;
        string id = value.id();

        // Its a NTScalar
        if (id == "epics:nt/NTScalar:1.0") {
            valueField = value["value"];
            code = valueField.type().code;
        }
        // Its a NTEnum 
        else if (id == "epics:nt/NTEnum:1.0") {
            valueField = value["value"]["index"];
            // Can not exist a mbbi or mbbo with 2 states
            if(value["value"]["choices"].as<shared_array<const string>>().size() == 2)
                code = TypeCode::Bool;
            else
                code = TypeCode::Int64;
        }
        
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
                throw runtime_error("Unsupported value data type");
                
                break;
        }
    } catch(const exception & e){
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

            // Boolean TwoStateVariable -> NTEnum
            case OpcUa_BuiltInType::OpcUaType_Boolean: {
                OpcUa_Boolean opcuaBool;
                variant.toBool(opcuaBool);
                if(opcuaBool)
                //value = nt::NTScalar{TypeCode::Bool}.create().update("value", rawBool);
                    value = nt::NTEnum{}.create().update("value.index", 1);
                else
                    value = nt::NTEnum{}.create().update("value.index", 0);
                cout << value << endl;
                break;
            }

            // Double -> Int64, longint, double?
            case OpcUa_BuiltInType::OpcUaType_Double: {
                double doubleValue;
                variant.toDouble(doubleValue);
                value = nt::NTScalar{TypeCode::Float64}.create().update("value", doubleValue);
                break;
            }

            // Int64 is used in MultiStateVariable -> NTEnum
            case OpcUa_BuiltInType::OpcUaType_Int64: {
                int64_t integer;
                variant.toInt64(integer);
                value = nt::NTEnum{}.create().update("value.index", integer);
                break;
            }

            default: {
                throw runtime_error("Unsupported variant data type");
                break;
            }
        }
    } catch(const exception & e){
        cerr << "Error converting OPCUA Variant to EPICS Value" << endl;
        cerr << e.what() << endl;
    }
    return value;
}

EPICStoOPCUAGateway::EPICStoOPCUAGateway(MyNodeIOEventManager* pNodeManager, int numThreads)
    : m_pNodeManager(pNodeManager), m_numThreads(numThreads) {    

    m_pvxsContext = Context(Config::from_env().build());

    //addMapping( "ejemplo1:Temperature", PVMapping("ejemplo1:Temperature", UaNodeId("Ejemplo1.Temperature", m_pNodeManager->getNameSpaceIndex())));
    addMapping( "ejemplo1:FanSpeed", PVMapping("ejemplo1:FanSpeed", UaNodeId("Ejemplo1.FanSpeed", m_pNodeManager->getNameSpaceIndex())));

    addMapping( "ejemplo2:OpenCmd", PVMapping("ejemplo2:OpenCmd", UaNodeId("Ejemplo2.OpenCmd", m_pNodeManager->getNameSpaceIndex())));
    //addMapping( "ejemplo2:Status", PVMapping("ejemplo2:Status", UaNodeId("Ejemplo2.Status", m_pNodeManager->getNameSpaceIndex())));

    //addMapping( "ejemplo3:int64out", PVMapping("ejemplo3:int64out", UaNodeId("Ejemplo3.int64out", m_pNodeManager->getNameSpaceIndex())));
    //addMapping( "ejemplo3:int64in", PVMapping("ejemplo3:int64in", UaNodeId("Ejemplo3.int64in", m_pNodeManager->getNameSpaceIndex())));
    //addMapping( "ejemplo3:longin", PVMapping("ejemplo3:longin", UaNodeId("Ejemplo3.longin", m_pNodeManager->getNameSpaceIndex())));
    //addMapping( "ejemplo3:longout", PVMapping("ejemplo3:longout", UaNodeId("Ejemplo3.longout", m_pNodeManager->getNameSpaceIndex())));
    //addMapping( "ejemplo3:mbbi", PVMapping("ejemplo3:mbbi", UaNodeId("Ejemplo3.mbbi", m_pNodeManager->getNameSpaceIndex())));
    addMapping( "ejemplo3:mbbo", PVMapping("ejemplo3:mbbo", UaNodeId("Ejemplo3.mbbo", m_pNodeManager->getNameSpaceIndex())));
}

EPICStoOPCUAGateway::~EPICStoOPCUAGateway() {
    stop();
}

void EPICStoOPCUAGateway::start() {
    
    m_running.store(true);

    // Subscribirse to each PV.
    // They have to be in m_pvMap
    for(const auto & [pvName, pvMapping] : m_pvMapName){

        m_subcriptions.push_back(
            m_pvxsContext.monitor(pvMapping.epicsName)
                .event([this](pvxs::client::Subscription & subscription){
                    auto eventSub = make_shared<GatewayEvent>(subscription.shared_from_this());
                    m_workQueue.push(eventSub);
                }).exec()
        );
    }

    // Start workers
    for(int i = 0; i<m_numThreads; ++i){
        m_workerThreads.push_back(thread([this](){processQueue();}));
    }

}

void EPICStoOPCUAGateway::stop() {
    
    m_running.store(false);

    // Signal to stop workers
    for(int i = 0; i < m_numThreads; ++i){
        m_workQueue.push(nullptr);
    }

    // Join to threads
    for(auto & thread : m_workerThreads)
        if(thread.joinable())
            thread.join();
    
    
    m_workerThreads.clear();
}

void EPICStoOPCUAGateway::enqueuePutTask(const UaVariable * variable, const UaDataValue& value) {

    auto it = m_pvMapUaNode.find(variable->nodeId().toXmlString().toUtf8());
    if(it != m_pvMapUaNode.end()){
        auto request = std::make_shared<PutRequest>(variable, value);
        auto eventPut = make_shared<GatewayEvent>(request);
        m_workQueue.push(eventPut);
    } else {
        cerr << "Variable not found in the UaNodeId mapping." << endl;
    }
    
}

bool EPICStoOPCUAGateway::addMapping(const string& name, const PVMapping& pvMapping) {

    auto result = m_pvMapName.emplace(name, pvMapping);
    if (result.second) {
        auto result = m_pvMapUaNode.emplace(pvMapping.nodeId.toXmlString().toUtf8(), pvMapping);
        return result.second;  
    }
    return false;
}

bool EPICStoOPCUAGateway::isMapped(const string& str){
    return (m_pvMapName.find(str) != m_pvMapName.end());
}

bool EPICStoOPCUAGateway::isMapped(const UaNodeId& nodeId){ 
    return (m_pvMapUaNode.find(nodeId.toXmlString().toUtf8()) != m_pvMapUaNode.end()); 
}

void EPICStoOPCUAGateway::GatewayHandler::operator()(shared_ptr<Subscription> & subscription) const {
    if(subscription){
        try{
            //cout << "Empieza Evento de monitoreo " << endl;
            Value value = subscription->pop();
            if(!value)
                return;
            
            cout << subscription->name() << endl;
            auto it = m_self->m_pvMapName.find(subscription->name());
            if(it != m_self->m_pvMapName.end()){
                //cout << "Llego a actualizar la variable" << endl;
                // Convert data from EPICS to OPC UA
                UaVariant variant = m_self->convertValueToVariant(value);
                // Update value in server
                UaStatus ret = m_self->m_pNodeManager->updateVariable(it->second.nodeId, variant);
                if(ret.isBad())
                    throw runtime_error("Error in monitored variable: Error updating value in server.");
            }


        } catch (const exception & e) {
            cerr << "Error: " << e.what() << endl;
        }
        m_self->m_workQueue.push(make_shared<GatewayEvent>(subscription));
    }
}

void EPICStoOPCUAGateway::GatewayHandler::operator()(shared_ptr<PutRequest> & putRequest) const {
    if(putRequest->variable != nullptr){
        //cout << "Procesando put request" << endl;
        auto it = m_self->m_pvMapUaNode.find(putRequest->variable->nodeId().toXmlString().toUtf8());
        // Conver tdata from OPC UA to EPICS
        Value value = m_self->convertUaDataValueToPvxsValue(putRequest->dataValue);
        // Update value in IOC
        try{
            if(value["value"].valid()){
                if(value.id() == "epics:nt/NTScalar:1.0")
                    m_self->m_pvxsContext.put(it->second.epicsName)
                    .set("value", value["value"])
                    .exec()->wait(1);

                if(value.id() == "epics:nt/NTEnum:1.0")
                    m_self->m_pvxsContext.put(it->second.epicsName)
                    .set("value.index", value["value.index"])
                    .exec()->wait(1);
                // Success
            }
        } 
        catch (const exception & e) {
            cerr << "Error in input request hadler: Error in pvxs put operation" << endl;
            cerr << e.what() << endl;
            // Notify NodeManager???
        }
    }
}
