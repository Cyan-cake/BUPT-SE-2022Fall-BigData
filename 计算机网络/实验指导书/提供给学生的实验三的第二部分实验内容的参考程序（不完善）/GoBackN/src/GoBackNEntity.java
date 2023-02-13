import java.net.*;
import java.util.*;
import java.lang.*;

class Frame {
    int kind;   // kind of frame:
    int s_nr;     // sequence number of frame
    int a_nr;     // acknowledgement number of frame

    StringBuffer payload;

    public Frame() {
        payload = new StringBuffer();
    }

    public Frame(int frame_kind, int seq_nr, int ack_nr, StringBuffer packet) {
        kind = frame_kind;
        s_nr = seq_nr;
        a_nr = ack_nr;
        payload = new StringBuffer(packet);
    }

    // To decode the message from the lower layer into frame format

    public int decode(StringBuffer message) {

        // System.out.println("to decode message received in decode(): " + message.toString() );
        System.out.println("************************The GobackN Protocol received a message from the lower layer channel, the received message is: " + message.toString() );

        kind = Integer.parseInt(message.substring(0,1));
        s_nr = Integer.parseInt(message.substring(2,3));
        a_nr = Integer.parseInt(message.substring(4,5));

        payload.append (message.substring(5));

        return 1;
    }

    // to encode frame into message to be sent to lower layer
    public int encode(StringBuffer encoded_string) {

        encoded_string.append(Integer.toString(kind) );
        encoded_string.append("#");
        encoded_string.append(Integer.toString(s_nr) );
        encoded_string.append("#");
        encoded_string.append(Integer.toString(a_nr) );
        encoded_string.append("#");
        encoded_string.append(payload);

        return 1;
    }

}

// The class GoBackNEntity is to implement the sliding window protocol with Go-Back-N
public class GoBackNEntity {
    // To define the constant value for frame kind
    public static final int DATA_FRAME=1;
    public static final int ACK_FRAME=2;

    // To define the TIMEOUT value for timers, in msec
    public static final long PIGGYBACK_TIMEOUT=100;
    public static final long POLLING_INTERVAL=100;

    public final int MAX_SEQ_NO = 7;
    public final int BUF_SIZE = 4;

    public GoBackNBuffer send_buffer;
    public GoBackNBuffer receive_buffer;

    public LowerLayerChannel lower_layer_channel;

    public int next_frame_to_send = 0;
    public int ack_expected = 0;
    public int frame_expected = 0;

    public int number_of_buffered_frames=0;

    // The polling_timer is used to enable the GoBackNEntity to get message from the lower layer channel in each POLLING_INTERVAL period.
    public Timer polling_timer;

    // The piggyback_timer is used check if there is acknowledgement information to be sent out with a piggyback way.
    public Timer piggyback_timer;
    public int to_piggyback = 0;

    // ReceiveFrameTask receive_task is the handler function of Timer piggyback_timer
    // PiggybackTask piggyback_task is the handler function of Timer piggyback_timer
    public ReceiveFrameTask receive_task;
    public PiggybackTask piggyback_task;

    public int is_in_interval(int lower_bound, int number_to_be_judged, int upper_bound){
        if ( ( (lower_bound<=number_to_be_judged) && (number_to_be_judged < upper_bound)) ||
                ( (upper_bound < lower_bound ) && (lower_bound <= number_to_be_judged))  ||
                ( (number_to_be_judged < upper_bound) && (lower_bound < lower_bound)) )
            return 1;
        else
            return 0;
    }

    public GoBackNEntity(String host_itself, short udp_port_itself, String peer_host, short peer_udp_port){

        // To initialize the lower layer channel
        // The lower layer channel is established with UDP protocol.
        // The lower layer channel is used by the GoBackN protocol.
        // After initialization of a lower_layer_channel instance,
        // this GoBackN protocol entity could use this lower layer channel to communicate with peer protocol entity.
        try {
            lower_layer_channel = new LowerLayerChannel(host_itself, udp_port_itself, peer_host, peer_udp_port);
        }
        catch (SocketException e)
        {
            e.printStackTrace();
        }

        // To initialize the send buffer of this GoBackN protocol entity.
        // The send_buffer is used to store the message given by the GoBackN protocol user application,
        // The message stored in the send_buffer is to be sent out by the GoBackN protocol entity.
        send_buffer = new GoBackNBuffer(this);

        // To initialize the receive buffer of this GoBackN protocol entity.
        // The message stored in the receive_buffer is got from the lower layer channel.
        // The message stored in the receive_buffer is to be retrieved by the GoBackN protocol user application.
        receive_buffer = new GoBackNBuffer(this);

        // The TimerTask receive_task is to get GoBackN protocol message from the lower layer channel
        polling_timer = new Timer();
        receive_task = new ReceiveFrameTask(this);
        polling_timer.schedule(receive_task, POLLING_INTERVAL, POLLING_INTERVAL);

        // to_piggyback = 0: there is not received frame to be acknowledged with piggyback way.
        to_piggyback = 0;
        number_of_buffered_frames = 0;
        ack_expected = 0;
    }

    // The get_frame_from_lower_layer() method is called by the receive_task() handler function of Timer  polling_timer.
    // The get_frame_from_lower_layer() is to get message sent by the peer GoBackN entity via the lower layer channel.
    public void get_frame_from_lower_layer(){
        byte[] receive_buf = new byte[256];

        // If there is not any free buffer unit in the receive_buffer of this GoBackEntity,
        // then this GoBackNEntity will not receive any message/frame from the lower layer channel.
        // This processing way is not the same as the way of flow control shown in the Textbook.
        if (receive_buffer.isGoBackNBufferFull() == 1) return;

        // to get frame from lower layer
        // The message got from the lower layer channel is to be put into variable receive_buf of type byte[].
        int bytes_received;
        bytes_received = lower_layer_channel.from_lower_layer(receive_buf);

        // If receive a message from the lower layer channel, then stepping into the while loop
        // In the while loop, process this message.
        // After having processed the message, then continue to get the next message from the lower layer channel
        // Until there is no any message to be gotten from the lower layer channel. Then break out from the while loop.
        // One task to be fulfilled in the process of received message is to put the payload of the GoBackN protocol message into the receive_buffer.
        while ( bytes_received >0 ){
//          System.out.println("bytes_received = " + Integer.toString(bytes_received) );


            // If there is free buffer unit in the receive_buffer, then to process the received frame
            // including to check if the frame is expected one, if it is not, just discard this frame
            // to check if the frame is not damaged, if it is, just discard this frame.
            // If everything is OK, then put the frame into the receive_buffer.

            // To convert the received message into Frame format.
            Frame received_frame = new Frame();

            // The variable received_string is created and is initialized with the value of the message stored in receive_buf.
            String received_string = new String(receive_buf, 0, bytes_received);
//            System.out.println("The received message in function get_frame_from_lower_layer() is:" + received_string );

            // To decode the message store in the received_string into a frame.
            StringBuffer received_message = new StringBuffer(received_string);
            received_frame.decode(received_message);

            // To process the received frame.
            // If the received frame is not data frame nor acknowledgement frame, just drop the frame
            // And then receive next frame from lower layer if there is.
            if ( received_frame.kind != ACK_FRAME  && received_frame.kind != DATA_FRAME ) {
                // If there is not any free buffer unit in the receive_buffer, the return from this method.
                if (receive_buffer.isGoBackNBufferFull() == 1) return;

                // If there is free buffer unit in the receive_buffer, then get the next message from the lower layer channel.
                bytes_received = lower_layer_channel.from_lower_layer(receive_buf);

                // After get the next message from the lower layer channel, process this next message by entering next while loop body.
                continue;
            }

            if ( received_frame.kind == DATA_FRAME ) {

                if ( receive_buffer.isGoBackNBufferFull() == 1 ){
                    System.out.println("************************The receive buffer of GoBackN protocol entity is full, and the received message is dropped.");
                    break;
                }

                if (received_frame.s_nr != frame_expected) {
//                    System.out.println("in received_frame.s_nr != frame_expected branch" );

                    // The received the frame is not the expected one, then just drop the received frame.
                    // And then receive next frame from lower layer if there is.
                    bytes_received = lower_layer_channel.from_lower_layer(receive_buf);

                    // After get the next message from the lower layer channel, process this next message by entering next while loop body.
                    continue;
                } else {
                    // to process the received frame that is the expected frame.

                    // to put the received frame into the receive buffer.
                    if ( receive_buffer.put_into_buffer(received_frame) == 0 ) break;

                    // Increase the sequence number of the expected number for next frame to be received.

                    frame_expected = (frame_expected + 1) % (MAX_SEQ_NO +1);

//                     System.out.println("frame_expected is:" + Integer.toString(frame_expected) );

                    if ( to_piggyback == 0 ) { // if piggyback timer is not created, then create a piggyback timer
                        to_piggyback = 1;

//                         System.out.println("to_piggyback:" + Integer.toString(to_piggyback) );

                        piggyback_timer = new Timer();
                        piggyback_task = new PiggybackTask(this);
                        piggyback_timer.schedule(piggyback_task, PIGGYBACK_TIMEOUT);
                    }
                }
            }

            // To process the acknowledgement number in the received frame, which is DATA_FRAME or ACK_FRAME
            // if the value of ack_nr field of the received frame is within [ack_expected, next_frame_to_send] of send_buffer,
            // then empty the send_buffer units from [ack_expected, ack_nr], and ack_expected is set to ack_nr
            // and also cancel the timers of these frame acknowledged.

//             System.out.println("To process ACK info......." );

            // if ( the received_frame's ack_seq is within [ack_expected, next_frame_to_send] ){
            while ( is_in_interval(ack_expected, received_frame.a_nr, next_frame_to_send) == 1) {
                // release the send buffer unit storing the ack_expected frame to the received.frame.ack_nr
/*
                    System.out.println("Is in interval, then release the buffer unit.");
                    System.out.println("ack_expected = " + Integer.toString(ack_expected));
                    System.out.println("received_frame.a_nr = " + Integer.toString(received_frame.a_nr));
                    System.out.println("next_frame_to_send = " + Integer.toString(next_frame_to_send));
*/
                    send_buffer.release_buffer_unit(ack_expected % BUF_SIZE);
                    ack_expected = ( ack_expected + 1) % (MAX_SEQ_NO+1);

                    number_of_buffered_frames = number_of_buffered_frames - 1 ;

//                 System.out.println("After release one buffer unit.");
            }

            // After finishing the frame process, to retrieve next frame from the lower layer channel.
            if (receive_buffer.isGoBackNBufferFull() == 1) return;
            bytes_received = lower_layer_channel.from_lower_layer(receive_buf);

//            System.out.println("To receive next frame ");
        }
    }



    // Function send_message() is used by the user of the GoBackN protocol to send a message.
    public int send_message(String message){
        if (message.length() <1) return 0;

        // If the send_buffer is full, just return 0 to indicate that the send_buffer is full.
        if ( send_buffer.isGoBackNBufferFull() == 1 ) {
            System.out.println("************************The send buffer of the GoBackN protocol entity is full.");
            System.out.println("************************The message to be send is not accepted by GoBackN protocol entity.");
            return 0;
        }

        // To construct a Go-Back-N frame
        Frame frame;
/*
        System.out.println("in send_message()............. ");
        System.out.println("next_frame_to_send = " + Integer.toString(next_frame_to_send));
*/

        frame = new Frame();
        frame.kind = DATA_FRAME;
        frame.s_nr = next_frame_to_send;
        frame.a_nr = (frame_expected + MAX_SEQ_NO) % (MAX_SEQ_NO + 1);
        frame.payload = new StringBuffer(message);


        // To encode the frame to string. sequencing the packet
        StringBuffer frame_encoded_string;

        frame_encoded_string = new StringBuffer();
        frame.encode(frame_encoded_string);

        // To send the encoded string via the lower layer channel.
        lower_layer_channel.to_lower_layer(frame_encoded_string.toString() );

        // And also start the timeout timer for the frame which is to be putted into the send buffer.
        send_buffer.start_timer_of_buffer_unit();

        // And, then to put the frame into the send_buffer,
        send_buffer.put_into_buffer(frame) ;

        // to increase the next_frame_to_send
        next_frame_to_send = (next_frame_to_send + 1) % (MAX_SEQ_NO+1);

        number_of_buffered_frames = number_of_buffered_frames + 1;

//        System.out.println("number_of_buffered_frames = " + Integer.toString(number_of_buffered_frames));
//        System.out.println("leave send_message()");
        return 1;
    }

    // Function get_message() is used by the user of the GoBackN protocol to retrieve a message from the GoBackN protocol.
    public int get_message(StringBuffer to_upper_layer_message){
        Frame received_frame;

        // If the receive_buffer is empty, just return 0 to indicate that the receive_buffer is empty.
        if (receive_buffer.isGoBackNBufferEmpty() == 1 ) return 0;

//        System.out.println("#################### in get_message() ....................  receive_buffer.isGoBackNBufferEmpty() != 1 ");

        // To retrieve a frame from the receiver_buffer.
        received_frame = receive_buffer.retrieve_from_buffer();

//        System.out.println("in get_message() ... received_frame.payload is:" + received_frame.payload.toString());

        // To get the payload from the received frame.
        to_upper_layer_message.append(received_frame.payload);

//        System.out.println("in get_message() ... upper_layer_message is:" + to_upper_layer_message.toString());

        return 1;
    }


    // piggyback_proc is used to send ACK_FRAME to the peer entity when there is not data frame
    // to be sent to the peer entity for a period of PIGGYBACK_TIME
    void piggyback_proc(){

        Frame ack_frame;
        StringBuffer ack_frame_encoded_string;

//        System.out.println("in piggyback_proc()");

        to_piggyback = 0;

        // To form an ACK_FRAME
        ack_frame = new Frame();
        ack_frame.kind = ACK_FRAME;
        ack_frame.a_nr = ( frame_expected + MAX_SEQ_NO ) % (MAX_SEQ_NO + 1) ;

        // To encode the ACK_FRAME into a String.
        ack_frame_encoded_string = new StringBuffer();
        ack_frame.encode(ack_frame_encoded_string);

        // To send the ACK_FRAME frame.
        lower_layer_channel.to_lower_layer(ack_frame_encoded_string.toString());
    }

}


