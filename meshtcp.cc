#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include <bits/stdc++.h>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TCP_MESH");

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("TcpSocket", LOG_LEVEL_INFO);
  //LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_ALL);
  //LogComponentEnable ("TcpSocketImpl", LOG_LEVEL_ALL);
  //LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (250));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("5kb/s"));

  NodeContainer nd;
  nd.Create(4);
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  vector<NetDeviceContainer> v;
  for(int i=0;i<4;i++)
  	for(int j=i+1;j<4;j++)
  		v.push_back(p2p.Install(nd.Get(i),nd.Get(j)));
  
  InternetStackHelper stack;
  stack.Install (nd);
  vector<Ipv4InterfaceContainer> intf;
  Ipv4AddressHelper address;
  for(int i=0;i<6;i++)
  {
    string s="10.1.";
    s+=((char)'0'+i+1);
    s+=".0";
  	address.SetBase (s.c_str(), "255.255.255.0");
  	intf.push_back(address.Assign(v[i]));
  }
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), 9));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (nd.Get(1));
  sinkApp.Start (Seconds (1.0));
  sinkApp.Stop (Seconds (10.0));

  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer clientApps;
  AddressValue remoteAddress(InetSocketAddress (intf[3].GetAddress (0), 9));
  clientHelper.SetAttribute ("Remote",remoteAddress);
  clientApps=clientHelper.Install (nd.Get (3));
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (10.0));
  AnimationInterface anim ("meshtcp.xml");
  anim.EnablePacketMetadata ();
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


