//hai!! Ini day-2 belajar ROS2! Semangat terus yaa...
// haii ini day-3 semangatt!
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <cmath>
#include <chrono>
using namespace std;
using Twist = geometry_msgs::msg::Twist;
using Pose = turtlesim::msg::Pose;

//day-4!!!
class FireFighter : public rclcpp::Node {
public:
    FireFighter() : Node("turtle_fighter"){
        publisher = this->create_publisher<Twist>("/turtle1/cmd_vel", 10);
        subscriber = this->create_subscription<Pose>("/turtle1/pose", 10, std::bind(&FireFighter::position, this, std::placeholders::_1));
        timer = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&FireFighter::fight,this));
        angle_now = 0.0;
        distance = 0;
        i = 0;
        t_start = this->get_clock()->now();
        finished = false;
        RCLCPP_INFO(this->get_logger(), "FIRE FIGHTERS UNITED");
    }
private:
    float x_now, y_now, x_next, y_next, distance, angle_error;
    float angle_now, angle_next, dy, dx, total_time;
    bool finished;
    int i;
    rclcpp::Time t_start, t_end;
    float x_point[5]= {9.0, 8.0, 5.0, 1.5, 2.0};
    float y_point[5] = {3.0, 8.0, 5.0, 3.5, 9.0};

    rclcpp::Publisher<Twist>::SharedPtr publisher;
    rclcpp::Subscription<Pose>::SharedPtr subscriber;
    rclcpp::TimerBase::SharedPtr timer;

    void position(const Pose::SharedPtr msg){
        x_now = msg->x;
        y_now = msg->y;
        angle_now = msg->theta;
    }
    void fight(){
        if(i > 4){
            return;
        }
        x_next = x_point[i];
        y_next = y_point[i];
        dy = y_next-y_now;
        dx = x_next-x_now;
        distance = sqrt(pow(dx, 2)+pow(dy, 2));
        angle_next = atan2(dy, dx);
        angle_error = angle_next-angle_now;
        while (angle_error > M_PI){
            angle_error -= 2*M_PI;
        }
        while (angle_error < -M_PI){
            angle_error += 2*M_PI;
        }
        auto move = Twist();
        if (abs(angle_error) > 0.02){
            move.angular.z = 2*angle_error;
            move.linear.x = 0.0;
        }else if (distance > 0.1){
            move.linear.x = 2.0*distance;
            move.angular.z = 0.0;
        }else{
            i++;
            if(i==5){
                finished = true;
                auto t_end = this->get_clock()->now();
                total_time = (t_end-t_start).seconds();
                RCLCPP_INFO(this->get_logger(), "TOTAL TIME: %.2f s", total_time);
            }
        }
        publisher->publish(move);
    }
};
    int main(int a, char **b){
        rclcpp::init(a,b);
        rclcpp::spin(std::make_shared<FireFighter>());
        rclcpp::shutdown();
        return 0;
    }
