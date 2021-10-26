//Shared libraries
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <stdio.h>
#include <string.h>
#include <enet/enet.h>

//Local libraries
#include "flecs.h"
#include "csc/csc_crossos.h"
#include "csc/csc_basic.h"
#include "csc/csc_assert.h"
#include "csc/csc_malloc_file.h"
#include "csc/csc_sdl_motion.h"
#include "csc/csc_gcam.h"
#include "csc/csc_gl.h"
#include "csc/csc_math.h"
#include "csc/csc_sdlglew.h"
#include "csc/csc_xlog.h"

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);
	if (enet_initialize () != 0)
	{
		fprintf (stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit (enet_deinitialize);

	ENetHost * client;
	{
		const ENetAddress * address	= NULL; //the address at which other peers may connect to this host. If NULL, then no peers may connect to the host.
		size_t peerCount = 1; //the maximum number of peers that should be allocated for the host.
		size_t channelLimit = 2; //the maximum number of channels allowed; if 0, then this is equivalent to ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT
		enet_uint32 incomingBandwidth = 0; //downstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
		enet_uint32 outgoingBandwidth = 0; //upstream bandwidth of the host in bytes/second; if 0, ENet will assume unlimited bandwidth.
		client = enet_host_create (address, peerCount, channelLimit, incomingBandwidth, outgoingBandwidth);
	}
	if (client == NULL)
	{
		fprintf (stderr,"An error occurred while trying to create an ENet client host.\n");
		exit (EXIT_FAILURE);
	}


	ENetPeer *peer; //The server to connect to
	{
		char const * address_text = "localhost";
		ENetAddress address;
		ENetEvent event;
		enet_address_set_host (&address, address_text);
		address.port = 9002;
		// Initiate the connection, allocating the two channels 0 and 1.
		peer = enet_host_connect (client, & address, 2, 0);
		if (peer == NULL)
		{
			fprintf (stderr,"No available peers for initiating an ENet connection.\n");
			exit (EXIT_FAILURE);
		}
		// Wait up to 5 seconds for the connection attempt to succeed.
		if (enet_host_service (client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf ("Connection to %s succeeded.\n", address_text);
				break;
			default:
				enet_peer_reset (peer);
				printf ("Connection to %s failed.\n", address_text);
				break;
			}
		}
		else
		{
			// Either the 5 seconds are up or a disconnect event was
			// received. Reset the peer in the event the 5 seconds
			// had run out without any significant event.
			enet_peer_reset (peer);
			printf ("Connection to %s failed.\n", address_text);
		}
	}


	{
		/* Create a reliable packet of size 7 containing "packet\0" */
		ENetPacket * packet = enet_packet_create ("packet", strlen ("packet") + 1, ENET_PACKET_FLAG_RELIABLE);
		/* Extend the packet so and append the string "foo", so it now */
		/* contains "packetfoo\0"                                      */
		enet_packet_resize (packet, strlen ("packetfoo") + 1);
		strcpy (& packet ->  data [strlen ("packet")], "foo");
		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send (peer, 0, packet);
		/* One could just use enet_host_service() instead. */
		enet_host_flush (client);
	}


	enet_host_destroy(client);


	return 0;
}
