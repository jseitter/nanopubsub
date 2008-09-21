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

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.Parser;
import org.apache.commons.cli.PosixParser;
import org.apache.log4j.BasicConfigurator;

public class StandaloneBroker {

	private NanoBroker broker = null;

	/**
	 * Main Method for running the standalone Broker
	 * 
	 * @param args
	 * @author jseitter
	 */
	public static void main(String[] args) {
		
		Options options = new Options();

		options.addOption(OptionBuilder.hasArg(true).withArgName("port")
				.withDescription("broker port number").create('p'));

		Parser parser = new PosixParser();
		CommandLine cmdLine = null;
		try {
			cmdLine = parser.parse(options, args);
		} catch (ParseException e) {
			printHelp(options);
		}

		if (cmdLine == null)
			printHelp(options);

		new StandaloneBroker(cmdLine);
		
		
	}

	public StandaloneBroker(CommandLine cmdLine){

		BasicConfigurator.configure();
		
		if(cmdLine.hasOption('p')){
			broker = new NanoBroker(Integer.parseInt(cmdLine.getOptionValue('p')));
		}
		else {
			broker = new NanoBroker();	
		}
		
	
	}
	
	private static void printHelp(Options options) {
		HelpFormatter hf = new HelpFormatter();
		System.out.println("nanoPubSub - embedded publish subscribe messaging");
		hf.printHelp("nanobroker", options, true);
		System.exit(0);
	}

}
