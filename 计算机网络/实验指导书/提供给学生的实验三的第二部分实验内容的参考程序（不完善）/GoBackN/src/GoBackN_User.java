import java.io.IOException;
import java.util.Scanner;

public class GoBackN_User {
    public static void main(String[] args) throws IOException {
        GoBackNEntity go_back_n_entity;

        if (args.length != 5) {
            System.out.println("Usage: java GoBackN_User <host_IP> <host_UDP_port> <peer_host_ip> <peer_UDP_port> <role of this application>");
            return;
        }

        String host_IP;
        String peer_host_IP;
        short host_UDP_port;
        short peer_UDP_port;

        byte[] send_buf = new byte[256];
        StringBuffer received_message;

        host_IP = args[0];
        peer_host_IP = args[2];
        host_UDP_port = Short.valueOf(args[1]);
        peer_UDP_port = Short.valueOf(args[3]);

        // to new a GoBackNEntity instance
        go_back_n_entity = new GoBackNEntity(host_IP, host_UDP_port, peer_host_IP, peer_UDP_port);

        // To send frame via lower layer
        //String data_sent = new String("data sent from " + args[0]);
        String user_role = args[4];

        if (user_role.equalsIgnoreCase("Sender") ) {
            while (true){
                Scanner s = new Scanner(System.in);
                System.out.println("Please input any message to be sent out and then return:");
                String user_input = s.nextLine();

                go_back_n_entity.send_message(user_input);
            }
        }
        else {
            // to get frame from lower layer
            while (true) {
                Scanner s = new Scanner(System.in);
                System.out.println(" ");
                System.out.println("#########################################################################################. ");
                System.out.println("This application is a user of the GoBackN-Sliding Protocol. ");
                System.out.println("If there is already message received by the GoBackN-Sliding Protocol entity,  ");
                System.out.println("then this user application can retrieve message from the GoBackN-Sliding Protocol entity, ");
                System.out.println("To receive message from the GoBackN-Sliding Protocol, press RETURN: ");
                System.out.println(" ");

                String user_input = s.nextLine();

                while (true){
                    received_message = new StringBuffer();
                    if (go_back_n_entity.get_message(received_message) == 0) break;

//                    System.out.println("Message received: " + received_message.toString() + " from port:" + Short.toString(host_UDP_port));

                    if (received_message.length() < 1) break;

                    System.out.println("Message received: " + received_message.toString() + " from port:" + Short.toString(host_UDP_port));

                }

                System.out.println("At this moment, no more message can be get from the GoBackN-Sliding Protocol entity.");
            }
        }
    }
}
