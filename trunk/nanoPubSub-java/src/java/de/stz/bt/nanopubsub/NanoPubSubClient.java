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
package de.stz.bt.nanopubsub;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Hashtable;
import java.util.StringTokenizer;

import org.apache.log4j.Logger;

/**
 * Client to connect via UDP to the nanoPubSub Broker
 * @author jseitter
 */
public class NanoPubSubClient {

	private static final Logger LOG = Logger.getLogger("NanoPubSubClient");
	
	private DatagramSocket clientSocket = null;
	private byte[] packetBuffer = new byte[1024];
	private DatagramPacket packet = new DatagramPacket(packetBuffer,
			packetBuffer.length);
	private String clientId;
	private InetAddress host;
	private Hashtable<String, INanoPubSubCallback> topicHandlers = new Hashtable<String, INanoPubSubCallback>();

	public NanoPubSubClient(String clientId) {
		if (clientId.contains("#") == true) {
			throw new RuntimeException("invalid character '#' in client ID");
		}
		this.clientId = clientId;
	}

	public NanoPubSubClient(String clientId, String hostname) throws UnknownHostException {
		this.clientId = clientId;
		this.connect(hostname);
	}

	public void connect(String hostname) throws UnknownHostException {
		try {
			this.host = InetAddress.getByName(hostname);
			clientSocket = new DatagramSocket();
		} catch (SocketException e) {
			throw new RuntimeException(e);
		}
	}

	public void publish(String topic, String message) {
		if(clientSocket==null) throw new RuntimeException("Client not connected");
		String msg = "#msg#" + clientId + "#" + topic + "#" + message + "#";
		byte[] data = msg.getBytes();
		packet.setAddress(host);
		packet.setPort(11011);
		packet.setData(data);
		try {
			clientSocket.send(packet);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public void subscribe(String topic, INanoPubSubCallback handler) {
		if(clientSocket==null) throw new RuntimeException("Client not connected");
		String msg = "#sub#" + clientId + "#" + topic + "#";
		byte[] data = msg.getBytes();
		packet.setAddress(host);
		packet.setPort(11011);
		packet.setData(data);
		try {
			clientSocket.send(packet);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		if (!topicHandlers.contains(topic))
			topicHandlers.put(topic, handler);
	}

	public void unsubscribe(String topic) {
		if(clientSocket==null) throw new RuntimeException("Client not connected");
		String msg = "#unsub#" + clientId + "#" + topic + "#";
		byte[] data = msg.getBytes();
		packet.setAddress(host);
		packet.setPort(11011);
		packet.setData(data);
		try {
			clientSocket.send(packet);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		if (topicHandlers.contains(topic))
			topicHandlers.remove(topic);
	}

	class UdpReceiver extends Thread {

		private boolean running = true;
		private byte[] packetBuffer=new byte[1024];
		private DatagramPacket packet = new DatagramPacket(packetBuffer,packetBuffer.length);
		private DatagramSocket receiverSocket;
		private StringTokenizer stringTokenizer;
		
		public void shutdown() {
			this.running = false;
		}

		@Override
		public void run() {

			try {
				receiverSocket = new DatagramSocket(11011);
			} catch (SocketException e) {
				throw new RuntimeException(e);
			}

			while (running) {
				try {
					receiverSocket.receive(packet);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					throw new RuntimeException(e);
				}
				
				String msg = new String(packetBuffer,0,packet.getLength());
				stringTokenizer = new StringTokenizer(msg,"#");
				String clientId=stringTokenizer.nextToken();
				String topic = stringTokenizer.nextToken();
				String message = stringTokenizer.nextToken();
				
				topicHandlers.get(topic).onMessage(topic, message);
				
			}

		}

	}

}
