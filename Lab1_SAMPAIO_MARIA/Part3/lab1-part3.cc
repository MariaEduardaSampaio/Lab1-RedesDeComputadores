#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi = 3;
  uint32_t nPackets = 1;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("nPackets", "Number of packets", nPackets);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (nWifi < 1 || nWifi > 9)
  {
    std::cout << "Error: Number of WiFi STA nodes must be between 1 and 9." << std::endl;
    return 1;
  }

  if (nPackets < 1 || nPackets > 20)
  {
    std::cout << "Error: Number of packets must be between 1 and 20." << std::endl;
    return 1;
  }

  if (verbose)
  {
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi);

  NodeContainer wifiApNode2 = p2pNodes.Get (1);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1;
  phy1.SetChannel (channel1.Create ());

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2;
  phy2.SetChannel (channel2.Create ());

  WifiMacHelper mac;
  Ssid ssid1 = Ssid ("ns-3-ssid-1");
  Ssid ssid2 = Ssid ("ns-3-ssid-2");

  WifiHelper wifi;

  NetDeviceContainer staDevices;
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid1),
               "ActiveProbing", BooleanValue (false));
  staDevices = wifi.Install (phy1, mac, wifiStaNodes);

  NetDeviceContainer apDevices;

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));
  apDevices = wifi.Install (phy1, mac, wifiApNode);

  NetDeviceContainer staDevices2;

  mac.SetType ("ns3::StaWifiMac",
              "Ssid", SsidValue (ssid2),
              "ActiveProbing", BooleanValue (false));

  staDevices2 = wifi.Install (phy2, mac, wifiStaNodes2);

  NetDeviceContainer apDevices2;

  mac.SetType ("ns3::ApWifiMac",
              "Ssid", SsidValue (ssid2));
  apDevices2 = wifi.Install (phy2, mac, wifiApNode2);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);
  mobility.Install (wifiStaNodes2);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  mobility.Install (wifiApNode2);

  InternetStackHelper stack;

  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  stack.Install (wifiApNode2);
  stack.Install (wifiStaNodes2);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4InterfaceContainer wifiStaInterfaces2;
  wifiStaInterfaces2 = address.Assign (staDevices2);

  Ipv4InterfaceContainer wifiApInterface2;
  wifiApInterface2 = address.Assign (apDevices2);

  address.SetBase ("10.1.3.0", "255.255.255.0");

  Ipv4InterfaceContainer wifiStaInterfaces;
  wifiStaInterfaces = address.Assign (staDevices);

  Ipv4InterfaceContainer wifiApInterface;
  wifiApInterface = address.Assign (apDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps =
  echoServer.Install (wifiStaNodes2.Get (nWifi - 1));

  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (30.0));

  UdpEchoClientHelper echoClient(wifiStaInterfaces2.GetAddress (nWifi - 1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (nPackets));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (30.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (30.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
