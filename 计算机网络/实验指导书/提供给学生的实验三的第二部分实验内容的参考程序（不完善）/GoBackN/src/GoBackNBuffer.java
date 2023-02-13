import java.util.Timer;


class BufferUnit{
    Frame frame;
    int state;  // to recode the state of the buffer unit: EMPTY, OCCUPIED

    public Timer timeout_timer;

    BufferUnit(Frame f, int s){
        frame = f;
        state = s;
    }
}

class GoBackNBuffer{

    // To define the constant indicating the state of buffer unit
    public final static int EMPTY=0;
    public final static int OCCUPIED=1;

    public final static int Buffer_Size = 4;

    public final static long SEND_TIMEOUT=40000;

    public GoBackNEntity goBackNEntity;

    BufferUnit[] buffer;

    public int buffer_lower_boundary;
    public int buffer_upper_boundary;

    public GoBackNBuffer(GoBackNEntity go_back_n_entity) {
        goBackNEntity = go_back_n_entity;

        buffer_lower_boundary = 0;
        buffer_upper_boundary = 0;

        buffer = new BufferUnit[Buffer_Size];

        // To initialize buffer unit's state as EMPTY
        for (int i =0; i < Buffer_Size; i++){
            buffer[i] = new BufferUnit(new Frame(), EMPTY);
        }
    }

    synchronized int increase_lower_boundary(){

        buffer_lower_boundary = (buffer_lower_boundary + 1 ) % Buffer_Size;

        return 1;
    };

    synchronized int increase_upper_boundary(){
        buffer_upper_boundary = (buffer_upper_boundary + 1 ) % Buffer_Size;

        return 1;
    }

    synchronized public int isGoBackNBufferFull(){
/*
        System.out.println("..............  in isGoBackNBufferFull() ...........");
        System.out.println("..............  buffer_lower_boundary = " + Integer.toString(buffer_lower_boundary));
        System.out.println("..............  buffer_upper_boundary = " + Integer.toString(buffer_upper_boundary));
        System.out.println("..............  buffer[buffer_upper_boundary].state  = " + Integer.toString(buffer[buffer_upper_boundary].state ));
*/
        if ( buffer[buffer_upper_boundary].state != EMPTY && buffer_lower_boundary == buffer_upper_boundary)
            return 1;
        else
            return 0;

    }

    synchronized public int isGoBackNBufferEmpty(){
        if (( buffer_lower_boundary == buffer_upper_boundary ) && ( buffer[buffer_upper_boundary].state == EMPTY) )
            return 1;
        else
            return 0;
    }
    //
    synchronized public int put_into_buffer(Frame frame){
        // System.out.println("in put_into_buffer() ...........");

        if ( isGoBackNBufferFull() == 1 ) return 0;

        if ( (frame.s_nr % Buffer_Size) == buffer_upper_boundary
            && buffer[buffer_upper_boundary].state == OCCUPIED )
            return 0;

        buffer[buffer_upper_boundary].frame = frame;
        buffer[buffer_upper_boundary].state = OCCUPIED;

        increase_upper_boundary();

        return 1;
    }

    synchronized public Frame retrieve_from_buffer(){
/*
        System.out.println("in retrieve_from_buffer() ...........");
        System.out.println("in retrieve_from_buffer() ... buffer[buffer_lower_boundary].state = " + Integer.toString( buffer[buffer_lower_boundary].state));
        System.out.println("in retrieve_from_buffer() ... payload is:" + buffer[buffer_lower_boundary].frame.payload.toString());
*/
        // To get Frame from the buffer.
        Frame frame;

        frame = new Frame(buffer[buffer_lower_boundary].frame.kind,
                          buffer[buffer_lower_boundary].frame.s_nr,
                          buffer[buffer_lower_boundary].frame.a_nr,
                          buffer[buffer_lower_boundary].frame.payload);

        buffer[buffer_lower_boundary].state = EMPTY;

//        System.out.println("in retrieve_from_buffer() ... buffer[buffer_lower_boundary].state = " + Integer.toString( buffer[buffer_lower_boundary].state));

        increase_lower_boundary();

        return frame;
    }

    synchronized public int release_buffer_unit(int buf_index){
//        System.out.println("in release_buffer_unit() ... buf_index = " + Integer.toString(buf_index));

        goBackNEntity.send_buffer.buffer[buf_index].state = EMPTY;
        goBackNEntity.send_buffer.buffer[buf_index].timeout_timer.cancel();

        return 1;
    }

    synchronized int start_timer_of_buffer_unit(){
        int buffer_unit_index;

        buffer_unit_index = buffer_upper_boundary;
        goBackNEntity.send_buffer.buffer[buffer_unit_index].timeout_timer = new Timer();
        TimeoutProcTask timeout_task = new TimeoutProcTask(goBackNEntity);
        goBackNEntity.send_buffer.buffer[buffer_unit_index].timeout_timer.schedule(timeout_task, SEND_TIMEOUT);

        return 1;
    }

    synchronized int cancel_timer_of_buffer_unit(int buffer_unit_index){
        goBackNEntity.send_buffer.buffer[buffer_unit_index].timeout_timer.cancel();

        return 1;
    }

    synchronized int restart_timer_of_buffer_unit(int buffer_unit_index){
        goBackNEntity.send_buffer.buffer[buffer_unit_index].timeout_timer = new Timer();
        TimeoutProcTask timeout_task = new TimeoutProcTask(goBackNEntity);
        goBackNEntity.send_buffer.buffer[buffer_unit_index].timeout_timer.schedule(timeout_task, SEND_TIMEOUT);

        return 1;
    }
}


