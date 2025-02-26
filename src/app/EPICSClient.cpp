#include "EPICSClient.h"
#include "iostream"

#include "pv/pvAccess.h"
#include "pv/clientFactory.h"


#include "pva/client.h"

using namespace std;

using namespace epics::pvAccess;
using namespace epics::pvData;
using namespace epics::pvaClient;

using namespace pvac;

// class SimpleChannelListRequester : public epics::pvAccess::ChannelListRequester {
//     public:
//         std::string getRequesterName() {
//             return "SimpleChannelListRequester";
//         }
    
//         void message(std::string const &msg, epics::pvData::MessageType type) {
//             std::cout << "Mensaje (" << type << "): " << msg << std::endl;
//         }
    
//         void channelListResult(
//             const Status &status,
//             ChannelFind::shared_pointer const & channelFind,
//             PVStringArray::const_svector const & channelNames,
//             bool hasDynamic ) override 
//         {
//             if (!status.isSuccess()) {
//                 std::cerr << "Error obteniendo la lista de canales: " << status.getMessage() << std::endl;
//                 return;
//             }
    
//             std::cout << "Canales disponibles:" << std::endl;
//             for (size_t i = 0; i < channelNames.size(); i++) {
//                 std::cout << " - " << channelNames[i] << " (Tipo: " << ")" << std::endl;
//             }
//         }
//     };

EPICSClient::EPICSClient() {

    PvaClientPtr client = PvaClient::get("pva");
    
    try {
        PvaClientChannelPtr channel = client->channel("ejemplo1:FanSpeed", "pva", 5.0);
        channel->connect();
        std::cout << "Canal encontrado: " << channel->getChannelName() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    // // Add "pva" to registry
    // epics::pvAccess::ClientFactory::start();
    

    // // Create a new client instance
    // ChannelProvider::shared_pointer provider = ChannelProviderRegistry::clients()->createProvider("pva");

    // if(provider == NULL){
    //     throw std::runtime_error("PVA provider not registered");
    // }

    // ChannelListRequester::shared_pointer channelListRequester = make_shared<SimpleChannelListRequester>();

    // provider->channelList(channelListRequester);
    
}

EPICSClient::~EPICSClient() {}
