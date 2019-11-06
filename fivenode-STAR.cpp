

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


//
// Enable logging for UdpClient and UdpServer
//
//  LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
//  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

//
// Explicitly create the nodes required by the topology (shown above).
//
	//std::cout<<"Create nodes\n";
  //NS_LOG_INFO ("Create nodes.");
  NodeContainer n;
  n.Create (5);
	//std::cout<<"Create Channel\n";
  //NS_LOG_INFO ("Create channels.");
//
// Explicitly create the channels required by the topology (shown above).
//
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

//
// We've got the "hardware" in place.  Now we need to add IP addresses.
//

//
// Install Internet Stack
//
  
  Ipv4AddressHelper ipv4;

	//std::cout<<"Assign Ip Addressess\n";
  //NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (dev);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i2 = ipv4.Assign (dev2);
  
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3 = ipv4.Assign (dev3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4 = ipv4.Assign (dev4);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	//std::cout<<"Create Applications\n";
  //NS_LOG_INFO ("Create Applications.");
//
// Create one udpServer application on node one.
//
  uint16_t port1 = 8000; // Need different port numbers to ensure there is no conflict
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

//
// Create one UdpClient application to send UDP datagrams from node zero to
// node one.
//
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

  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));


//
// Tracing
//
  /*
  AsciiTraceHelper ascii;
  p2p.EnableAscii(ascii.CreateFileStream ("p2p_cli_ser.tr"), dev);
  p2p.EnablePcap("p2p_cli_ser", dev, false);
  */

//
// Calculate Throughput using Flowmonitor
//
  /*FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();
*/

//
// Now, do the actual simulation.
//
	//std::cout<<"Run Simulation\n";
 // NS_LOG_INFO ("Run Simulation.");
  //Simulator::Stop (Seconds(11.0));
  
  //std::cout<<"Create Xml File\n";
  AnimationInterface anim ("threenode.xml");
  
  anim.SetConstantPosition(n.Get(0),200,200);
  anim.SetConstantPosition(n.Get(1),150,200);
  anim.SetConstantPosition(n.Get(2),100,200); 
  anim.SetConstantPosition(n.Get(3),150,100);
  anim.SetConstantPosition(n.Get(4),150,300); 

// anim.EnablePacketMetadata ();
  
  Simulator::Run ();

  /*monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress=="10.1.1.1" && t.destinationAddress == "10.1.2.2"))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      	  std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }



  monitor->SerializeToXmlFile("p2p_cli_ser.flowmon", true, true);

	std::cout<<"Destroy Simulations\n";*/
  //Simulator::Destroy ();
//  std::cout<<"Done Simulation\n";
  //NS_LOG_INFO ("Done.");*/
  return 0;
}
