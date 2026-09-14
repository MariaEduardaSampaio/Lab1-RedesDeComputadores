#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

bool ValidateSizeOfClientsAndPackets(uint32_t nClients, uint32_t nPackets){
  if(nClients < 1 || nClients > 5){
    std::cout << "Error: Number of clients must be between 1 and 5." << std::endl;
    return false;
  }
  if(nPackets < 1 || nPackets > 5){
    std::cout << "Error: Number of packets must be between 1 and 5." << std::endl;
    return false;
  }
  return true;
}

int
main (int argc, char *argv[])
{
  uint32_t nClients = 1;
  uint32_t nPackets = 1;

  CommandLine cmd (__FILE__);
  cmd.AddValue("nClients", "Number of clients", nClients);
  cmd.AddValue("nPackets", "Number of packets", nPackets);
  cmd.Parse (argc, argv);

  if(nClients < 1 || nClients > 5){
    std::cout << "Error: Number of clients must be between 1 and 5." << std::endl;
    return 1;
  }
  if(nPackets < 1 || nPackets > 5){
    std::cout << "Error: Number of packets must be between 1 and 5." << std::endl;
    return 1;
  }
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer server;
  server.Create(1);

  NodeContainer clients;
  clients.Create(nClients);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  InternetStackHelper stack;
  stack.Install(server);
  stack.Install(clients);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  for (uint32_t i = 0; i < nClients; i++){
    NodeContainer pair(clients.Get(i), server.Get(0));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(pair);

    address.Assign(devices);
    address.NewNetwork();
  }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  UdpEchoServerHelper echoServer (9);
  echoServer.SetAttribute("Port", UintegerValue(15));

  ApplicationContainer serverApps = echoServer.Install(server.Get(0));

  serverApps.Start(Seconds(1.0));
  serverApps.Stop(Seconds(20.0));

  Ipv4Address serverAddress("10.1.1.2");

  UdpEchoClientHelper echoClient (serverAddress, 15);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (nPackets));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  Ptr<UniformRandomVariable> randomStartTime = CreateObject<UniformRandomVariable>();

  for (uint32_t i = 0; i < nClients; i++){
    ApplicationContainer clientApp = echoClient.Install(clients.Get(i));

    double startTime = randomStartTime->GetValue(2.0, 7.0);

    clientApp.Start(Seconds(startTime));
    clientApp.Stop(Seconds(20.0));    
  }

  Simulator::Stop(Seconds(20.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}