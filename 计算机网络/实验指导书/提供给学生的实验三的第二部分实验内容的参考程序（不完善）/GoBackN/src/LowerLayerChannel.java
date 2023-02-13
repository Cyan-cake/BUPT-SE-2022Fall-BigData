
import java.io.*;
import java.net.*;
import java.util.*;

public class LowerLayerChannel {
    public String entity_address;
    public short entity_port;
    public String peer_entity_address;
    public short peer_entity_port;
    public InetAddress host_inet_address;
    public InetAddress peer_inet_address;
    public InetAddress address;
    protected DatagramSocket socket;

    public LowerLayerChannel(String  host_address, short host_udp_port, String peer_address, short peer_udp_port) throws SocketException {
        entity_address = host_address;
        entity_port = host_udp_port;
        peer_entity_address = peer_address;
        peer_entity_port = peer_udp_port;

        try {
            host_inet_address = InetAddress.getByName(entity_address);
        }
        catch (UnknownHostException e)
        {
            e.printStackTrace();
        }

        socket = new DatagramSocket(host_udp_port,host_inet_address);
    }

    public int from_lower_layer(byte[] frame_received){
        byte[] buf = new byte[256];

        try {
            peer_inet_address = InetAddress.getByName(peer_entity_address);
        }
        catch (UnknownHostException e)
        {
            e.printStackTrace();
        }
        DatagramPacket packet = new DatagramPacket(frame_received, frame_received.length, peer_inet_address, peer_entity_port);

        try {
            socket.receive(packet);
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }

        // display response
        String received = new String(packet.getData(), 0, packet.getLength());
//        System.out.println("Data received: " + received);

        return packet.getLength();
    }

    public int to_lower_layer(String frame_to_be_sent){
        byte[] buf = new byte[256];

//        System.out.println("to_lower_layer( ) ......");
//        System.out.println("The frame_to_be_sent is:" + frame_to_be_sent );
        System.out.println("************************The GobackN Protocol is to send a message: " + frame_to_be_sent );

        // if the length of frame_to_be_sent is larger than the max_frame_length, then return 0
        if (frame_to_be_sent.length() > 256) return 0;

        // To put the frame to be sent into the buffer.
        buf = frame_to_be_sent.getBytes();

        try{
            address = InetAddress.getByName(peer_entity_address);
        }
        catch (UnknownHostException e)
        {
            e.printStackTrace();
        }

//        System.out.println("buf.length is : "  + Integer.toString(buf.length));

        DatagramPacket packet = new DatagramPacket(buf, buf.length, address, peer_entity_port);

        try {
            socket.send(packet);
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }

        return 1;
    }
}
