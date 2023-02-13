import java.util.Timer;
import java.util.TimerTask;

// The instance of ReceiveProc is to get frame from the lower layer channel
class ReceiveFrameTask extends TimerTask
{
    GoBackNEntity goBackNEntity;

    public ReceiveFrameTask(GoBackNEntity go_back_n_entity){
        goBackNEntity = go_back_n_entity;
    }
    // In the run() thread, to get frame from lower layer channel, and process the frame received.
    public void run()
    {
        // to call the GobackNEntity's get_frame_from_lower_layer();
        // The get_frame_from_lower_layer() will receive and process all the frames arrived.
        goBackNEntity.get_frame_from_lower_layer();
    }
}

class TimeoutProcTask extends TimerTask
{
    GoBackNEntity goBackNEntity;

    // This task is for the timeout timers created for each frame already sent.
    public TimeoutProcTask(GoBackNEntity go_back_n_entity) {
        goBackNEntity = go_back_n_entity;
    }

    public void run() {
        int i=0;
        StringBuffer frame_encoded_string;
        Frame frame;

//        System.out.println("in TimeoutPro()  ..........goBackNEntity.number_of_buffered_frames = ..... " + Integer.toString(goBackNEntity.number_of_buffered_frames) );

        if (goBackNEntity.number_of_buffered_frames > 1){
            int frame_index = goBackNEntity.next_frame_to_send ;

            for (i=2; i<= goBackNEntity.number_of_buffered_frames; i++){
                goBackNEntity.send_buffer.cancel_timer_of_buffer_unit(frame_index % goBackNEntity.BUF_SIZE);
                frame_index = (frame_index +1) % (goBackNEntity.MAX_SEQ_NO+1) ;
            }
        }
        // to retransmit the packets from the next ack_expected frame to the lastest frame sent
        goBackNEntity.next_frame_to_send = goBackNEntity.ack_expected;

        for (i=1; i<= goBackNEntity.number_of_buffered_frames; i++){
            // To encode the frame to string. sequencing the packet

            int frame_index = goBackNEntity.next_frame_to_send ;

            frame = goBackNEntity.send_buffer.buffer[frame_index % goBackNEntity.BUF_SIZE].frame;
            frame_encoded_string = new StringBuffer();
            frame.encode(frame_encoded_string);

            // To send the encoded string
            goBackNEntity.lower_layer_channel.to_lower_layer(frame_encoded_string.toString());

            goBackNEntity.send_buffer.restart_timer_of_buffer_unit(frame_index % goBackNEntity.BUF_SIZE);

            goBackNEntity.next_frame_to_send =  (goBackNEntity.next_frame_to_send + 1) % (goBackNEntity.MAX_SEQ_NO+1);
        }


    }
}

class PiggybackTask extends TimerTask
{
    GoBackNEntity goBackNEntity;

    // This task is for the timeout timers created for each frame already sent.
    public PiggybackTask(GoBackNEntity go_back_n_entity) {
        goBackNEntity = go_back_n_entity;
    }

    public void run() {
        // to check which frame's timer is timeout
        // then, retransmit all the frame from the timeout frame to the latest sent frame.
        goBackNEntity.piggyback_proc();

    }
}
