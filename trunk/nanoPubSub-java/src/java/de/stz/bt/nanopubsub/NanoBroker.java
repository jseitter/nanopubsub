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
import java.io.Serializable;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.Logger;

/**
 * Nano Publish Subscribe Broker
 * 
 * Can be used standalone or embedded in a java app
 * 
 * @author jseitter
 * 
 */
public class NanoBroker {

	private static final Logger LOG = Logger.getLogger("NanoBroker");
	private BrokerUDPReceiverWorker receiverWorker;
	private BrokerUDPSenderWorker senderWorker;
	private BrokerLocalCallWorker localCallWorker;
	// private BrokerClientKeepAliveWorker
	private MessageQueue netOutgoingQueue = new MessageQueue("netOutgoingQueue");
	private MessageQueue localCallQueue = new MessageQueue("localCallQueue");

	/**
	 * contains topics registered by network clients
	 */
	private Hashtable<String, ArrayList> topics = new Hashtable<String, ArrayList>();
	/**
	 * contains topics registered by localCLient Ids
	 */
	private Hashtable<String, ArrayList<String>> localTopics = new Hashtable<String, ArrayList<String>>();
	/**
	 * Contains mappings between CLientId and InetAddress
	 */
	private Hashtable<String, InetAddress> clients = new Hashtable<String, InetAddress>();
	/**
	 * Contains mapping between localClients and INanoPubSub Handlers
	 */
	private Hashtable<String, INanoPubSubCallback> localClients = new Hashtable<String, INanoPubSubCallback>();

	/**
	 * Main Function for running the broker standalone
	 */
	public static void main(String[] args) {
		BasicConfigurator.configure();
		NanoBroker b = new NanoBroker();
	}

	public NanoBroker() {
		// initialize the Broker		
		senderWorker = new BrokerUDPSenderWorker();
		senderWorker.start();
		localCallWorker = new BrokerLocalCallWorker();
		localCallWorker.start();
		receiverWorker = new BrokerUDPReceiverWorker();
		receiverWorker.start();
	}
	
	public NanoBroker(int receiverUdpPort){
		// initialize the Broker	
		senderWorker = new BrokerUDPSenderWorker();
		senderWorker.start();
		localCallWorker = new BrokerLocalCallWorker();
		localCallWorker.start();
		receiverWorker = new BrokerUDPReceiverWorker();
		receiverWorker.port=receiverUdpPort;
		receiverWorker.start();
	}

	/**
	 * Subscribes a client handler for the given topic
	 * 
	 * @param localClientId
	 *            identifier to separate callers for message filtering
	 * @param topic
	 *            name of the topic to subscribe
	 * @param handler
	 *            callback Handler implementing the INanoPubSub interface
	 */
	public void subscribeTopic(String localClientId, String topic,
			INanoPubSubCallback handler) {

		// does the handler for the client already exist
		if (localClients.containsKey(localClientId)) {
			// yes, so do nothing
		} else {
			localClients.put(localClientId, handler);
		}
		// does the topic already exist locally ?
		if (localTopics.get(topic) == null) {
			// no, create the topic and the handler list
			ArrayList<String> list = new ArrayList<String>();
			list.add(localClientId);
			localTopics.put(topic, list);
		} else {
			// yes, just add the clientId to the topics client list
			ArrayList<String> list = localTopics.get(topic);
			list.add(localClientId);
		}
	}

	/**
	 * Unsubscribes a client handler for the given topic
	 * 
	 * @param localClientId
	 *            identifier to separate callers for message filtering
	 * @param topic
	 *            name of the topic to subscribe
	 * @param handler
	 *            callback Handler implementing the INanoPubSub interface
	 */
	public void unsubscribeTopic(String localClientId, String topic,
			INanoPubSubCallback handler) {
		if (localTopics.get(topic) == null) {
			// no, so we just skip it
		} else {
			// yes, so remove handler from the handler list
			ArrayList<String> list = localTopics.get(topic);
			list.remove(localClientId);
			// FIXME check if client can be removed from the localCLients
			// hashtable
		}
	}

	/**
	 * Publishes a message on a given topic
	 * 
	 * @param localClientId
	 *            identifier to separate callers for message filtering
	 * @param topic
	 *            name of the topic to subscribe
	 * @param handler
	 *            callback Handler implementing the INanoPubSub interface
	 */
	public void publish(String localClientId, String topic, String message) {
		if (LOG.isDebugEnabled())
			LOG.debug("called local publish()");
		String msg = "#msg#" + localClientId + "#" + topic + "#" + message
				+ "#";
		internalMessage InMsg = internalMessage.parse(msg);
		netOutgoingQueue.putMessage(InMsg);
		localCallQueue.putMessage(InMsg);
	}

	/**
	 * Simple message queue
	 * 
	 * @author jseitter
	 */
	class MessageQueue {

		private final Logger LOG = Logger.getLogger("MessageQueue");
		private String queueName;
		ArrayList<internalMessage> messages = new ArrayList();

		/* lock the constructor */
		private MessageQueue() {
		};

		/**
		 * 
		 * @param queueName
		 *            name of the queue
		 */
		public MessageQueue(String queueName) {
			this.queueName = queueName;
		}

		/**
		 * Store message to the queue end
		 * 
		 * @param message
		 */
		public synchronized void putMessage(internalMessage message) {
			if (LOG.isDebugEnabled())
				LOG.debug(queueName + ": message added [" + message.toString()
						+ "]");
			messages.add(message);
			this.notifyAll();
		}

		/**
		 * Retrieve first message from the queue
		 * 
		 * @return first message in the queue
		 */
		public synchronized internalMessage getMessage() {
			while (messages.isEmpty())
				try {
					wait();
				} catch (InterruptedException e) {
					throw new RuntimeException(e);
				}
			internalMessage message = messages.remove(0);
			if (LOG.isDebugEnabled())
				LOG.debug(queueName + ": message removed ["
						+ message.toString() + "]");
			return message;
		}

	}

	/**
	 * Thread implementing the local Calls
	 * 
	 * @author jseitter
	 * 
	 */
	class BrokerLocalCallWorker extends Thread {
		private final Logger LOG = Logger.getLogger("BrokerLocalCallWorker");

		private boolean running = true;

		public void shutdown() {
			this.running = false;
		}

		@Override
		public void run() {

			while (running) {
				if (LOG.isDebugEnabled())
					LOG.debug("reading local call queue");
				internalMessage inMsg = localCallQueue.getMessage();

				// publish local
				if (localTopics.containsKey(inMsg.topic)) {
					ArrayList<String> localSubscribers = localTopics
							.get(inMsg.topic);
					Iterator<String> it = localSubscribers.iterator();
					while (it.hasNext()) {
						String clientId = it.next();
						if (!clientId.equals(inMsg.clientId)) {
							LOG.debug("local publish to " + clientId);
							// do the publish through INanoPubSub handler
							(localClients.get(clientId)).onMessage(inMsg.topic,
									inMsg.message);
						}
					}
				}
			}

		}

	}

	/**
	 * Thread implementing the UDP message sender
	 * 
	 * @author jseitter
	 * 
	 */
	class BrokerUDPSenderWorker extends Thread {
		private final Logger LOG = Logger.getLogger("BrokerUDPSenderWorker");

		private DatagramSocket sock;
		private boolean running = true;

		private byte[] packetBuffer = new byte[1024];
		DatagramPacket packet = new DatagramPacket(packetBuffer,
				packetBuffer.length);

		public void shutdown() {
			this.running = false;
		}

		@Override
		public void run() {

			try {
				LOG.info("creating UDP sender socket");
				sock = new DatagramSocket();
			} catch (SocketException e) {
				// FIXME exception handling
				e.printStackTrace();
				throw new RuntimeException(e);
			}

			while (running) {
				if (LOG.isDebugEnabled())
					LOG.debug("reading send queue");
				internalMessage message = netOutgoingQueue.getMessage();

				// check if it is a message to really send out, only msg is send
				if (message.msgType.equals("msg")) {
					packet.setData(message.toString().getBytes());
					// get Subscribers for message topic
					ArrayList subscribers = topics.get(message.topic);
					// if there are any subscribers
					if (subscribers != null) {
						// iterate over them
						Iterator it = subscribers.iterator();

						while (it.hasNext()) {
							String recvClientId = (String) it.next();
							// check if sender is not equal receiver
							if (!message.clientId.equals(recvClientId)) {
								InetAddress receiver = clients
										.get(recvClientId);
								packet.setSocketAddress(new InetSocketAddress(
										receiver, 11011));
								LOG.debug("net publish to "
										+ receiver.getHostAddress());
								try {
									sock.send(packet);
								} catch (IOException e) {
									// TODO Auto-generated catch block
									e.printStackTrace();
									// FIXME handle resending
									throw new RuntimeException(e);
								}
							}
						}
					}
				}

			}
			LOG.info("closing UDP sender socket");
			sock.close();
			LOG.info("leaving UDP sender thread");
		}

	}

	/**
	 * Thread implementing the UDP message receiver
	 * 
	 * @author jseitter
	 */
	class BrokerUDPReceiverWorker extends Thread {

		int port = 11011;
		private final Logger LOG = Logger.getLogger("BrokerUDPReceiverWorker");
		private DatagramSocket sock;
		private boolean running = true;

		private byte[] packetBuffer = new byte[1024];
		DatagramPacket packet = new DatagramPacket(packetBuffer,
				packetBuffer.length);

		public void shutdown() {
			this.running = false;
		}

		public void run() {

			StringTokenizer tokenizer;

			try {
				LOG.info("creating UDP receiver socket on Port " + port);
				sock = new DatagramSocket(port);
			} catch (SocketException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				throw new RuntimeException(e);
			}

			while (running) {

				try {

					sock.receive(packet);
					InetAddress sender = packet.getAddress();
					int datalength = packet.getLength();
					String msg = new String(packetBuffer, 0, datalength);

					internalMessage inMsg = internalMessage.parse(msg);

					String operation = inMsg.msgType;

					if (operation.equals("msg")) {
						LOG.debug("message (msg) received");
						netOutgoingQueue.putMessage(inMsg);
						localCallQueue.putMessage(inMsg);
					}

					if (operation.equals("sub")) {
						LOG.debug("subscribe (sub) received");
						String clientId = inMsg.clientId;
						String topic = inMsg.topic;
						// add client to list of known clients
						if (!clients.containsKey(clientId)) {
							LOG.info("new client " + sender.getHostAddress());
							clients.put(clientId, sender);
						}
						ArrayList subscribers = topics.get(topic);
						// if subscribers==null we have a new topic
						if (subscribers == null) {
							subscribers = new ArrayList();
							subscribers.add(clientId);
							topics.put(topic, subscribers);
							LOG.info("Client " + clientId + " subscribed new "
									+ topic);
						}
						// already known topic
						else {
							subscribers.add(clientId);
							LOG.info("Client " + clientId
									+ " subscribed existing " + topic);
						}

					}

					if (operation.equals("unsub")) {
						LOG.debug("unsubscribe (unsub) received");
						String clientId = inMsg.clientId;
						String topic = inMsg.topic;
						if (topics.containsKey(topic)) {
							ArrayList subscribers = topics.get(topic);

							if (subscribers.contains(clientId)) {
								subscribers.remove(clientId);
								LOG.debug("removed client " + clientId
										+ " from topic " + topic);
							} else {
								LOG.info("unknown client " + clientId
										+ " cannot unregister");
							}
						}
					}
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

			}
			LOG.info("closing UDP Receiver socket");
			sock.close();
			LOG.info("leaving UDP receiver thread");
		}
	}

}

/**
 * Container class for messages
 * 
 * @author jseitter
 */
class internalMessage implements Serializable {
	/**
	 * serial version id
	 */
	private static final long serialVersionUID = -3829990036989058804L;
	/**
	 * Type of message
	 */
	String msgType;
	/**
	 * id of sending client
	 */
	String clientId;
	/**
	 * topic target for message
	 */
	String topic;
	/**
	 * message content
	 */
	String message;

	/**
	 * parse a message string into an internalMessage object
	 * @param msg
	 *            the input String
	 * @return internalMessage object
	 */
	public static internalMessage parse(String msg) {
		StringTokenizer st = new StringTokenizer(msg, "#");
		internalMessage inMsg = new internalMessage();

		inMsg.msgType = st.nextToken();
		inMsg.clientId = st.nextToken();
		inMsg.topic = st.nextToken();
		if (inMsg.msgType.equals("msg")) {
			inMsg.message = st.nextToken();
		}

		return inMsg;
	}

	public String toString() {
		if (msgType.equals("msg"))
			return "#msg#" + clientId + "#" + topic + "#" + message + "#";
		if (msgType.equals("sub"))
			return "#sub#" + clientId + "#" + topic + "#";
		if (msgType.equals("unsub"))
			return "#unsub#" + clientId + "#" + topic + "#";
		throw new RuntimeException("unknown msgType");
	}
}
