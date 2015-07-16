#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <yocto/PWM_info.h>
#include "yocto/yocto_api.h"
#include "yocto/yocto_pwminput.h"
/**
 * Código para a publicação de mensagens do tipo yocto/PWM_info no tópico /yocto/pwm_info
 */

using namespace std;

int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

int main(int argc, char **argv)
{
    /**
     * The ros::init() function needs to see argc and argv so that it can perform
     * any ROS arguments and name remapping that were provided at the command line. For programmatic
     * remappings you can use a different version of init() which takes remappings
     * directly, but for most command-line programs, passing argc and argv is the easiest
     * way to do it.  The third argument to init() is the name of the node.
     *
     * You must call one of the versions of ros::init() before using any other
     * part of the ROS system.
     */
    ros::init(argc, argv, "yocto_PWM");

    /**
     * NodeHandle is the main access point to communications with the ROS system.
     * The first NodeHandle constructed will fully initialize this node, and the last
     * NodeHandle destructed will close down the node.
     */
    ros::NodeHandle n;

    /**
     * The advertise() function is how you tell ROS that you want to
     * publish on a given topic name. This invokes a call to the ROS
     * master node, which keeps a registry of who is publishing and who
     * is subscribing. After this advertise() call is made, the master
     * node will notify anyone who is trying to subscribe to this topic name,
     * and they will in turn negotiate a peer-to-peer connection with this
     * node.  advertise() returns a Publisher object which allows you to
     * publish messages on that topic through a call to publish().  Once
     * all copies of the returned Publisher object are destroyed, the topic
     * will be automatically unadvertised.
     *
     * The second parameter to advertise() is the size of the message queue
     * used for publishing messages.  If messages are published more quickly
     * than we can send them, the number here specifies how many messages to
     * buffer up before throwing some away.
     */
    ros::Publisher pub = n.advertise<yocto::PWM_info>("/yocto/pwm_info", 1000); //avisa que irá publicar no tópico /yocto/pwm_info

    ros::Rate loop_rate(10);


    int channel=1;         
    double duty_Cycle=70.589;
    double frequency=1.507;

    string       errmsg;
    string       target;
    YPwmInput   *pwm;
    YPwmInput   *pwm1;
    YPwmInput   *pwm2;
    YModule     *m;

    YAPI::DisableExceptions();

    // Setup the API to use local USB devices
    if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    // retreive any pwm input available
    pwm = YPwmInput::FirstPwmInput();
    if (pwm == NULL) {
        cerr << "No module connected (Check cable)" << endl;
        exit(1);
    }

    // we need to retreive both channels from the device.
    if (pwm->isOnline()) {
        m = pwm->get_module();
        pwm1 = YPwmInput::FindPwmInput(m->get_serialNumber() + ".pwmInput1");
        pwm2 = YPwmInput::FindPwmInput(m->get_serialNumber() + ".pwmInput2");
    } else {
            cerr << "No module connected (Check cable)" << endl;
            exit(1);
    }





    yocto::PWM_info mtr;     //objeto da mensagem que será publicada
    while (ros::ok())
    {
        mtr.channel_nmbr = 1;
        mtr.frequency = pwm1->get_frequency();
        mtr.duty_cycle= pwm1->get_dutyCycle();
        frequency =     pwm2->get_frequency();
        duty_Cycle =    pwm2->get_dutyCycle();


        pub.publish(mtr);
        ros::spinOnce();
        loop_rate.sleep();



        mtr.channel_nmbr = 2;
        mtr.frequency = frequency;
        mtr.duty_cycle= duty_Cycle;
        ROS_INFO("Node yocto rodando \n");
        /**
        * The publish() function is how you send messages. The parameter
        * is the message object. The type of this object must agree with the type
        * given as a template parameter to the advertise<>() call, as was done
        * in the constructor above.
        */
        pub.publish(mtr);
        ros::spinOnce();
        loop_rate.sleep();
    }



    return 0;
}

