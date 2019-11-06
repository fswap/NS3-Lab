

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

//NS_LOG_COMPONENT_DEFINE ("client_server");


int main (int argc, char *argv[])
{
  double lat = 2.0;
  uint64_t rate = 5000000; // Data rate in bps
  double interval = 0.05;
  NodeContainer n;
  n.Create(5);
  InternetStackHelper internet;
  internet.Install (n);
  
  
  PointToPointHelper p2p;
  
  p2p.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (lat)));
  p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (rate)));
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (1400));
  
  NetDeviceContainer dev = p2p.Install (n.Get(0), n.Get(1));
  NetDeviceContainer dev2 = p2p.Install (n.Get(1), n.Get(2));
  NetDeviceContainer dev3 = p2p.Install (n.Get(1), n.Get(3));
  NetDeviceContainer dev4 = p2p.Install (n.Get(1), n.Get(4));
  
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (dev);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i2 = ipv4.Assign (dev2);
  
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3 = ipv4.Assign (dev3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4 = ipv4.Assign (dev4);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t port1 = 8000; 
  uint16_t port2 = 8001;
  uint16_t port3 = 8002;
  UdpServerHelper server1 (port1);
  UdpServerHelper server2 (port2);
  UdpServerHelper server3 (port3);
  
  ApplicationContainer apps;
  apps = server1.Install (n.Get (2));
  apps = server2.Install (n.Get (2));
  apps = server3.Install (n.Get (4));

  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));

  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (interval);
  uint32_t maxPacketCount = 320;
  UdpClientHelper client1 (i2.GetAddress (1), port1);
  UdpClientHelper client2 (i2.GetAddress (1), port2);
  UdpClientHelper client3 (i4.GetAddress (1), port3);

  client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  client3.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client3.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client3.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
	
  apps = client1.Install (n.Get (0));
  apps = client2.Install (n.Get (0));
  apps = client3.Install (n.Get (3));
  AnimationInterface anim ("threenode.xml");
  
  anim.SetConstantPosition(n.Get(0),200,200);
  anim.SetConstantPosition(n.Get(1),150,200);
  anim.SetConstantPosition(n.Get(2),100,200); 
  anim.SetConstantPosition(n.Get(3),150,100);
  anim.SetConstantPosition(n.Get(4),150,300); 
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));


  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
