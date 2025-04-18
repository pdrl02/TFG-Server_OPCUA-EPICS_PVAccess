/**
 * @file EPICStoOPCUAGateway.h
 * @brief Declaration of the EPICStoOPCUAGateway class.
 * 
 * This file contains the declaration of the EPICStoOPCUAGateway class,
 * which acts as a bridge between an OPC UA server and an EPICS (Experimental Physics and Industrial Control System)
 * control system. 
 * 
 * The class is responsible for managing subscriptions to EPICS channels via PVXS
 * (a C++ implementation of the PVAccess protocol), enabling synchronized data exchange
 * between both systems.
 * 
 * The class provides an interface to:
 * - Start and stop the gateway.
 * - Enqueue write task to EPICS channels.
 * - Register mappings between EPICS process variables and OPC UA node Ids.
 * - Convert data between PVXS and OPC UA representations.
 * 
 * It is designed to integrate with a custom node manager from OPC UA (MyNodeIOEventManager).
 * 
 * It do not implement behaviour for OPC UA events like alarms or conditions. 
 * 
 * @author Pablo Del Río López
 * @date 2025-06-01
 */

#ifndef __EPICSTOOPCUAGATEWAY_H__
#define __EPICSTOOPCUAGATEWAY_H__

#include <iostream>
#include <pvxs/client.h>
#include <pvxs/util.h>
#include <string>
#include <uanodeid.h>
#include <myNodeIOEventManager.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <variant>
#include <uabasenodes.h>
#include <uadatavalue.h>
#include <pvxs/nt.h>

using namespace pvxs;
using namespace pvxs::client;
using namespace std;

/**
 * @struct PutRequest
 * @brief Represents a write operation to an EPICS process variable.
 * 
 * Encapsulates the necesary information to perform the a "put" operation.
 * It is enqueued as a task to perform by the gateway's worker thread(s).
 * 
 */
struct PutRequest {
    /**
     * @brief Pointer to the OPC UA Variable associated with the write operation.
     * 
     */
    const UaVariable * variable;

    /**
     * @brief UaDataValue which contains the value to be written to the EPICS process variable.
     * 
     */
    UaDataValue dataValue;

    /**
     * @brief Construct a new and empty PutRequest object.
     * 
     */
    PutRequest() = default;

    /**
     * @brief Construct a new PutRequest object with a given UaVariable and UaDataValue.
     * 
     * @param var Pointer to the UaVariable.
     * @param val Data value to be written to the EPICS process variable.
     */
    PutRequest(const UaVariable * var, UaDataValue val)
    : variable(var), dataValue(val) {}

};

using GatewayEvent = std::variant<shared_ptr<pvxs::client::Subscription>, shared_ptr<PutRequest>>;

/**
 * @struct PVMapping
 * @brief Represents a mapping between an EPICS PV name and an OPC UA NodeId.
 * 
 * This structure is used to establish a link between a named EPICS process variable
 * and its corresponding OPC UA node.
 * 
 */
struct PVMapping {
    /**
     * @brief The name of the EPICS process variable.
     * 
     */
    string epicsName;

    /**
     * @brief The associated UaNodeId
     * 
     */
    UaNodeId nodeId;

    /**
     * @brief Construct a new PVMapping object.
     * 
     */
    PVMapping() = default;

    /**
     * @brief Construct a new PVMapping object with a given string and UaNodeId
     * 
     * @param name The EPICS process variable name
     * @param node The associated UaNodeId
     * 
     */
    PVMapping(string name, UaNodeId node) 
    : epicsName(name), nodeId(node) {}
};


/**
 * @class EPICStoOPCUAGateway
 * @brief Main gateway class to bridge EPICS with PVXS and OPC UA.
 * 
 * Manages subscriptions and put operations between EPICS process variables and OPC UA nodes.
 * Handles data conversion, mapping, and task processing through background worker(s).
 * 
 */
class EPICStoOPCUAGateway {

private:
    
    /**
     * @brief Thread-safe, bounded, multi-producer, multi-consumer FIFO queue. 
     * The GatewayEvent that this queue manages could be pvxs::client::Subscription or PutRequest.
     * 
     */
    MPMCFIFO<shared_ptr<GatewayEvent>> m_workQueue{100};

    /**
     * @brief An independent PVA protocol client instance.
     * 
     */
    Context m_pvxsContext;

    /**
     * @brief A vector to store references to pvxs::client::Subscription to prevent them from being released.
     * 
     */
    vector<shared_ptr<Subscription>> m_subcriptions;

    /**
     * @brief Pointer to MyNodeIoEventManager.
     * 
     */
    MyNodeIOEventManager * m_pNodeManager;

    /**
     * @brief Stores mappings from EPICS PV names to their corresponding OPC UA nodes.
     * 
     * Key: EPICS process variable name (string).  
     * Value: PVMapping structure containing the OPC UA NodeId.
     */
    unordered_map<string, PVMapping> m_pvMapName;

    /**
     * @brief Stores mappings from UaNodeIds (as string) to EPICS PV names.
     * 
     * This allow quick reverse lookups from OPC UA nodes to their associated EPICS names.
     * 
     * Key: OPC UA NodeId (string format).  
     * Value: PVMapping structure containing the EPICS PV name.
     * 
     */
    unordered_map<string, PVMapping> m_pvMapUaNode;

    /**
     * @brief Number of workers threads used to process queued events.
     * 
     */
    int m_numThreads;

    /**
     * @brief Thread pool for processing the work queue asynchonously.
     * 
     * Each thread pulls tasks from the queue and executes the appropriate handler.
     * 
     */
    vector<thread> m_workerThreads;

    /**
     * @brief Flag indicating whether the gateway is currently running.
     * 
     * Used for safely starting and stopping the processing loop across threads.
     * 
     */
    atomic<bool> m_running{false};

    /**
     * @brief Internal loop that processes events from the work queue.
     * 
     * Runs in a dedicated thread. Handles both Put operations and Subscription updates
     * using a variant-based dispatcher.
     * 
     */
    void processQueue();

    /**
     * @brief Converts a PVXS Value to an OPC UA UaVariant.
     * 
     * @param value The PVXS Value to be converted.
     * @return UaVariant with the corresponding Value. Can be empty.
     */
    UaVariant convertValueToVariant(const Value & value);

    /**
     * @brief Converts a OPC UA UaVariant to a PVXS Value.
     * 
     * @param dataValue The OPC UA data to convert.
     * @return Value with the corresponding value. Can be empty.
     */
    Value convertUaDataValueToPvxsValue(const UaDataValue & dataValue);


public:

    /**
     * @brief Construct a new EPICStoOPCUAGateway object.
     * 
     * @param pNodeManager Pointer to MyNodeIOEventManager.
     * @param numThreads Number of workers attending the event queue.
     */
    EPICStoOPCUAGateway(MyNodeIOEventManager * pNodeManager, int numThreads = 1);

    /**
     * @brief Destroy the EPICStoOPCUAGateway object.
     * 
     */
    ~EPICStoOPCUAGateway();

    /**
     * @brief Start the gateway and its internal work thread(s).
     * Initializes the processing queue and begins handling EPICS subscriptions
     * and OPC UA write tasks.
     */
    void start();

    /**
     * @brief Stops the gateway and join all runnings threads.
     * 
     */
    void stop();

    /**
     * @brief Enqueue a Put task to be processed asynchronously.
     * 
     * Adds a write operation to the internal work queue to update an EPICS PV based on the given OPC UA variable
     * and value.
     * 
     * @param variable Pointer to the source UaVariable.
     * @param value The data value to be written.
     */
    void enqueuePutTask(const UaVariable * variable, const UaDataValue& value);

    /**
     * @brief Registers a mapping between an EPICS PV name and an OPC UA node.
     * 
     * @param name The EPICS process variable.
     * @param pvMapping The mapping structure containing name and node ID.
     * @return true if the mapping was added successfully.
     * @return false if the name already exists or in any error.
     */
    bool addMapping(const string & name, const PVMapping & pvMapping);

    /**
     * @brief Checks if an EPICS name is already mapped.
     * 
     * @param str EPICS PV name.
     * @return true if the name is mapped.
     * @return false otherwise.
     */
    bool isMapped(const string & str);

    /**
     * @brief Checks if an OPC UA node is already mapped.
     * 
     * @param nodeId OPC UA UaNodeId.
     * @return true if the node is mapped.
     * @return false otherwise.
     */
    bool isMapped(const UaNodeId & nodeId);


    /**
     * @class GatewayHandler
     * @brief Internal handler class used to process variant-based work items.
     * 
     * Implements calleable operators for handling Subscriptions and PutRequest events. 
     * 
     */
    class GatewayHandler {
        private:
            /**
             * @brief A pointer to the owning EPICStoOPCUAGateway instance.
             * 
             */
            EPICStoOPCUAGateway* m_self;

        public:
            /**
             * @brief Construct a new Gateway Handler object.
             * 
             * @param ptr Pointer to the owning EPICStoOPCUAGateway.
             */
            explicit GatewayHandler(EPICStoOPCUAGateway* ptr) : m_self(ptr) {}

            /**
             * @brief Handles a Subscription event.
             * 
             * Called when a new EPICS subscriptions message is received.
             * Reenqueue the subscription when it has been processed.
             * 
             * @param subscription Shared pointer to the subscription object.
             */
            void operator()(shared_ptr<Subscription> & subscription) const;

            /**
             * @brief Handles a PutRequest event.
             * 
             * Called when a write (Put) operation is dequeued.
             * 
             * @param putRequest Shared pointer to the PutRequest.
             */
            void operator()(shared_ptr<PutRequest> & putRequest) const;
    };

};

#endif  // __EPICSTOOPCUAGATEWAY_H__
