//
// Created by abc on 9/26/21.
//

// This file provide function to detect if there is command input by the user from the shell of this process
// The function read_user_cmd uses the select mechanism of the Linux I/O model to detect if there is input from standard input device, which is the keyboard.
// The function read the user command after the user input the command and enter the "enter" key of the keyboard
// The fifth parameter of the select() function called in the function read_user_cmd() is set to 200 usecs. The select() function is to wait 200 usecs for user input.
// After calling the select(), if user does not enter command in 200 usecs, the select() returns with error;
// if user enters the command in 200 usecs, the select() returns , and the following sentences of read_user_cmd() function will read the command inputted,
// and return the command received to the caller function.

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>

int read_user_cmd( char * cmd_buf, int buf_len)
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    int cmd_len;

    /* Watch stdin (fd 0) to see when it has input. */

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to 0 seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 200;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1){
        return 0;
    }else if (retval){
        if (FD_ISSET(0, &rfds) ){
            cmd_len = read(0, cmd_buf, buf_len);
            cmd_buf[cmd_len-1] ='\0';
            // std::cout << "cmd user input: " <<cmd_buf << std::endl;
            return 1;
        }
    }
    else
        return 0;

}
