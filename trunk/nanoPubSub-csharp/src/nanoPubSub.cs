/*
 *   nanoPubSub - embedded Publish Subscribe Messaging
 *   (c) 2008 STZ Building Technology
 * 
 *   nanoPubSub is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace NanoPubSub
{
    public delegate void onMessage(String topic, String Message);

    class nanoPubSubClient
    {
        private UdpClient client = new UdpClient();
        private String clientId;
        protected Hashtable topicHandlers = new Hashtable();

        public nanoPubSubClient(String clientId)
        {
            if(clientId.IndexOfAny(new char[] { '#' }) != -1)
            {
                throw new Exception("invalid character '#' in client ID");
            }
            this.clientId = clientId;
        }

        public nanoPubSubClient(String clientId, String hostname)
        {
            this.clientId = clientId;
            this.connect(hostname);
        }


        public void connect(String hostname)
        {
            client.Connect(hostname, 11011);
            //Starting the UDP Server thread.
            Thread UdpThread = new Thread(new ThreadStart(udpReceiver));
            UdpThread.Start();

        }

        public void publish(String topic, String message)
        {
            String msg = "#msg#"+clientId+"#"+topic+"#"+message+"#";
            //FIXME size check if(msg.Length>1024)
            byte[] data = Encoding.UTF8.GetBytes(msg);
            client.Send(data, data.Length);
            
        }

        public void subscribe(String topic, onMessage handler )
        {
            String msg = "#sub#" + clientId + "#" + topic + "#";
            byte[] data = Encoding.UTF8.GetBytes(msg);
            client.Send(data, data.Length);
            
            //if(!topicHandlers.Contains(topic))
            topicHandlers.Add(topic, handler);
        }

        public void unsubscribe(String topic)
        {
            String msg = "#unsub#" + clientId + "#" + topic + "#";
            byte[] data = Encoding.UTF8.GetBytes(msg);
            client.Send(data, data.Length);
            
        }


        private void udpReceiver()
        {
        try
        {
            //Create a UDP socket.
            Socket soUdp = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            IPEndPoint localIpEndPoint = new IPEndPoint(IPAddress.Any,11011);
            soUdp.Bind(localIpEndPoint);
            while (true)
            {
            Byte[] packetBuffer = new Byte[1024];
            int bytesReceived = soUdp.Receive(packetBuffer);
            String dataReceived = System.Text.Encoding.UTF8.GetString(packetBuffer,0,bytesReceived);        
            String[] tokens = dataReceived.Split(new char[] {'#'});       
            onMessage handler = (onMessage) topicHandlers[tokens[3]];
            if (handler != null) handler(tokens[3], tokens[4]);  
            }
        }
        catch (SocketException se)
        {
            
        }
        }
 
        }

    }