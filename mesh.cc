/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include <bits/stdc++.h>

using namespace std;
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

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
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (nd.Get (1));
  
  UdpEchoClientHelper echoClient(intf[4].GetAddress(0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps = echoClient.Install (nd.Get (3));
  
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  AnimationInterface anim ("mesh.xml");
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


