#include "../header/functions.h"

char mode_start;

void write_in_queue(RT_QUEUE *, MessageToMon);

void f_server(void *arg) {
    int err;
    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

    err = run_nodejs("/usr/local/bin/node", "/home/pi/Interface_Robot/server.js");

    if (err < 0) {
        printf("Failed to start nodejs: %s\n", strerror(-err));
        exit(EXIT_FAILURE);
    } else {
#ifdef _WITH_TRACE_
        printf("%s: nodejs started\n", info.name);
#endif
        open_server();
        rt_sem_broadcast(&sem_serverOk);
    }
}

void f_camera(void * arg){
    
    int err;
    MessageToMon msg;
    
    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_serverOk, TM_INFINITE);
    
    while(1){
        rt_sem_p(&sem_camera,TM_INFINITE);
        switch(modeForCamera){
            case CAM_OPEN:
                rt_mutex_acquire(&mutex_camera, TM_INFINITE);
                err = open_camera(&camera);
                rt_mutex_release(&mutex_camera);
                if(err < 0) {
                    printf("Failed to start camera: %s\n", strerror(-err));
                    set_msgToMon_header(&msg, HEADER_STM_NO_ACK);
                    write_in_queue(&q_messageToMon, msg);
                    exit(EXIT_FAILURE);
                }else{        
                    set_msgToMon_header(&msg, HEADER_STM_ACK);
                    write_in_queue(&q_messageToMon, msg);
                    rt_sem_v(&sem_image);
                }
                break;
            case CAM_CLOSE:
                rt_mutex_acquire(&mutex_camera, TM_INFINITE);
                close_camera(&camera);
                send_message_to_monitor(HEADER_STM_ACK,"camera closed");
                rt_mutex_release(&mutex_camera);
                break;
            case CAM_ASK_ARENA: case CAM_COMPUTE_POSITION:
                break;
            case CAM_ARENA_CONFIRM: case CAM_ARENA_INFIRM:
                rt_sem_v(&sem_image);
                break;
            default:
                printf("Unknown command for Camera ===================\n");
                break;
        }
             
    }
    
    
    
}

void f_image(void * arg){
    Image img;
    Jpg imgCompressed;
    MessageToMon msg;
    
    Arene arena;
    bool has_arena = false;
    
    Position tabPosition[10];
    int nb_position = 0;
    
    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_task_set_periodic(NULL, TM_NOW, 100000000);
    rt_sem_p(&sem_image, TM_INFINITE);
    
    while(1){
        rt_task_wait_period(NULL);
        if(camera.isOpened() == true){
            
            rt_mutex_acquire(&mutex_camera, TM_INFINITE);
            get_image(&camera,&img,"");
            rt_mutex_release(&mutex_camera);
            
            switch(modeForCamera) {
                case CAM_OPEN:
                    
                    if(has_arena) {
                        draw_arena(&img,&img,&arena); 
                        if(nb_position>0){
                            nb_position = detect_position(&img, tabPosition, &arena);
                            for(int i=0;i<nb_position;i++){
                                draw_position(&img, &img, &tabPosition[i]);
                            }
                            
                        }
                        
                    }
                    
                    compress_image(&img,&imgCompressed);
                    send_message_to_monitor(HEADER_STM_IMAGE,&imgCompressed);
                    break;
                    
                case CAM_ASK_ARENA:
                    
                    if(detect_arena(&img,&arena) < 0) {

                        send_message_to_monitor(HEADER_STM_NO_ACK,"Arena not found");

                    } else {
                        
                        send_message_to_monitor(HEADER_STM_MES,"Arena found");
                        draw_arena(&img,&img,&arena);   
                        compress_image(&img,&imgCompressed);
                        send_message_to_monitor(HEADER_STM_IMAGE,&imgCompressed);
                        
                        // TODO Check if logic
                        rt_sem_p(&sem_image, TM_INFINITE);
                        has_arena = modeForCamera == CAM_ARENA_CONFIRM;
                    }
                   
                    
                    modeForCamera = CAM_OPEN;
                   
                    break;
                   
                case CAM_COMPUTE_POSITION:
                    send_message_to_monitor(HEADER_STM_MES,"recherche position...");
                    nb_position = detect_position(&img, tabPosition, &arena);
                    send_message_to_monitor(HEADER_STM_MES,nb_position+" position trouvé");
                    
                    
                    modeForCamera = CAM_OPEN;
                    
                    break;
                case CAM_CLOSE : case CAM_ARENA_CONFIRM : case CAM_ARENA_INFIRM :
                    break;
                default:
                   send_message_to_monitor(HEADER_STM_MES,"WTF man??");
                   printf("Unknown mode\n");
                   break;
            }
            
        }   
    }
}

void f_checkBattery(void * arg) {
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);
    
    rt_task_set_periodic(NULL, TM_NOW, 500000000);
    
    int batteryLevel;
    while(1){
        rt_task_wait_period(NULL);
        if(robotStarted == 1){
            batteryLevel = send_command_to_robot(DMB_GET_VBAT)+48;
            send_message_to_monitor(HEADER_STM_BAT,&batteryLevel);
        }
    }
    
}

void f_sendToMon(void * arg) {
    int err;
    MessageToMon msg;

    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

#ifdef _WITH_TRACE_
    printf("%s : waiting for sem_serverOk\n", info.name);
#endif
    rt_sem_p(&sem_serverOk, TM_INFINITE);
    while (1) {

#ifdef _WITH_TRACE_
        printf("%s : waiting for a message in queue\n", info.name);
#endif
        if (rt_queue_read(&q_messageToMon, &msg, sizeof (MessageToRobot), TM_INFINITE) >= 0) {
#ifdef _WITH_TRACE_
            printf("%s : message {%s,%s} in queue\n", info.name, msg.header, msg.data);
#endif

            send_message_to_monitor(msg.header, msg.data);
            free_msgToMon_data(&msg);
            rt_queue_free(&q_messageToMon, &msg);
        } else {
            printf("Error msg queue write: %s\n", strerror(-err));
        }
    }
}

void f_receiveFromMon(void *arg) {
    MessageFromMon msg;
    MessageToMon msgTest;
    int err;

    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

#ifdef _WITH_TRACE_
    printf("%s : waiting for sem_serverOk\n", info.name);
#endif
    rt_sem_p(&sem_serverOk, TM_INFINITE);
    do {
#ifdef _WITH_TRACE_
        printf("%s : waiting for a message from monitor\n", info.name);
#endif
        err = receive_message_from_monitor(msg.header, msg.data);
#ifdef _WITH_TRACE_
        printf("%s: msg {header:%s,data=%s} received from UI\n", info.name, msg.header, msg.data);
#endif
        if (strcmp(msg.header, HEADER_MTS_COM_DMB) == 0) {
            if (msg.data[0] == OPEN_COM_DMB) { // Open communication supervisor-robot
#ifdef _WITH_TRACE_
                printf("%s: message open Xbee communication\n", info.name);
#endif
                rt_sem_v(&sem_openComRobot);
            }
        } else if (strcmp(msg.header, HEADER_MTS_DMB_ORDER) == 0) {
            if (msg.data[0] == DMB_START_WITHOUT_WD) { // Start robot
#ifdef _WITH_TRACE_
                printf("%s: message start robot\n", info.name);
#endif 
                rt_sem_v(&sem_startRobot);

            } else if ((msg.data[0] == DMB_GO_BACK)
                    || (msg.data[0] == DMB_GO_FORWARD)
                    || (msg.data[0] == DMB_GO_LEFT)
                    || (msg.data[0] == DMB_GO_RIGHT)
                    || (msg.data[0] == DMB_STOP_MOVE)) {

                rt_mutex_acquire(&mutex_move, TM_INFINITE);
                move = msg.data[0];
                rt_mutex_release(&mutex_move);
#ifdef _WITH_TRACE_
                printf("%s: message update movement with %c\n", info.name, move);
#endif

            }
        }else if (strcmp(msg.header, HEADER_MTS_CAMERA) == 0){
#ifdef _WITH_TRACE_
                printf("%s: message for camera\n", info.name);
#endif          
               modeForCamera = msg.data[0];
               rt_sem_v(&sem_camera);
                
        }
    } while (err > 0);

}

void f_openComRobot(void * arg) {
    int err;

    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

    while (1) {
#ifdef _WITH_TRACE_
        printf("%s : Wait sem_openComRobot\n", info.name);
#endif
        rt_sem_p(&sem_openComRobot, TM_INFINITE);
#ifdef _WITH_TRACE_
        printf("%s : sem_openComRobot arrived => open communication robot\n", info.name);
#endif
        err = open_communication_robot();
        if (err == 0) {
#ifdef _WITH_TRACE_
            printf("%s : the communication is opened\n", info.name);
#endif
            MessageToMon msg;
            set_msgToMon_header(&msg, HEADER_STM_ACK);
            write_in_queue(&q_messageToMon, msg);
        } else {
            MessageToMon msg;
            set_msgToMon_header(&msg, HEADER_STM_NO_ACK);
            write_in_queue(&q_messageToMon, msg);
        }
    }
}

void f_startRobot(void * arg) {
    int err;

    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

    while (1) {
#ifdef _WITH_TRACE_
        printf("%s : Wait sem_startRobot\n", info.name);
#endif
        rt_sem_p(&sem_startRobot, TM_INFINITE);
#ifdef _WITH_TRACE_
        printf("%s : sem_startRobot arrived => Start robot\n", info.name);
#endif
        err = send_command_to_robot(DMB_START_WITHOUT_WD);
        if (err == 0) {
#ifdef _WITH_TRACE_
            printf("%s : the robot is started\n", info.name);
#endif
            rt_mutex_acquire(&mutex_robotStarted, TM_INFINITE);
            robotStarted = 1;
            rt_mutex_release(&mutex_robotStarted);
            MessageToMon msg;
            set_msgToMon_header(&msg, HEADER_STM_ACK);
            write_in_queue(&q_messageToMon, msg);
        } else {
            MessageToMon msg;
            set_msgToMon_header(&msg, HEADER_STM_NO_ACK);
            write_in_queue(&q_messageToMon, msg);
        }
    }
}

void f_move(void *arg) {
    /* INIT */
    RT_TASK_INFO info;
    rt_task_inquire(NULL, &info);
    printf("Init %s\n", info.name);
    rt_sem_p(&sem_barrier, TM_INFINITE);

    /* PERIODIC START */
#ifdef _WITH_TRACE_
    printf("%s: start period\n", info.name);
#endif
    rt_task_set_periodic(NULL, TM_NOW, 100000000);
    while (1) {
#ifdef _WITH_TRACE_
        printf("%s: Wait period \n", info.name);
#endif
        rt_task_wait_period(NULL);
#ifdef _WITH_TRACE_
        printf("%s: Periodic activation\n", info.name);
        printf("%s: move equals %c\n", info.name, move);
#endif
        rt_mutex_acquire(&mutex_robotStarted, TM_INFINITE);
        if (robotStarted) {
            rt_mutex_acquire(&mutex_move, TM_INFINITE);
            send_command_to_robot(move);
            rt_mutex_release(&mutex_move);
#ifdef _WITH_TRACE_
            printf("%s: the movement %c was sent\n", info.name, move);
#endif            
        }
        rt_mutex_release(&mutex_robotStarted);
    }
}

void write_in_queue(RT_QUEUE *queue, MessageToMon msg) {
    void *buff;
    buff = rt_queue_alloc(&q_messageToMon, sizeof (MessageToMon));
    memcpy(buff, &msg, sizeof (MessageToMon));
    rt_queue_send(&q_messageToMon, buff, sizeof (MessageToMon), Q_NORMAL);
}