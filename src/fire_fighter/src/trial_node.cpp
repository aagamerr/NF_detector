//hai!! Ini day-2 belajar ROS2! Semangat terus yaa...
// haii ini day-3 semangatt!
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <cmath>
#include <algorithm>
#include <vector>
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
        system("ros2 service call /turtle1/set_pen turtlesim//srv/SetPen \"{r: 255, g: 255, b: 255, width:2, 'off': 1}\"");
        best_route();
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
    float x_point[5]= {0.0, 0.0, 0.0, 0.0, 0.0};
    float y_point[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

    rclcpp::Publisher<Twist>::SharedPtr publisher;
    rclcpp::Subscription<Pose>::SharedPtr subscriber;
    rclcpp::TimerBase::SharedPtr timer;

    void best_route(){
        float x_init[5] = {8.0, 2.0, 9.0, 5.0, 1.5};
        float y_init[5] ={8.0, 9.0, 3.0, 5.0, 5.5};
        string point_name[5] = {"A", "B", "C", "D", "E"};

        vector<int> order = {0, 1, 2, 3, 4};
        float min_d = 1000;
        vector<int> winning_route;
        do{
            float total_d = 0;
            for (int k = 0; k < 4; k++){
                int now = order[k];
                int next = order[k+1];
                total_d += sqrt(pow(x_init[next]-x_init[now], 2) + pow(y_init[next]-y_init[now], 2));
            }
            if (total_d < min_d){
                winning_route = order;
            }
        } while (next_permutation(order.begin(), order.end()));
        for (int j = 0;  j < 5; j++){
            x_point[j] = x_init[winning_route[j]];
            y_point[j] = y_init[winning_route[j]];
        }
        RCLCPP_INFO(this->get_logger(), "BEST ROUTE HAS FOUND!: %.2f", min_d);
    }
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
            if(i==0){
                system("ros2 service call /turtle1/set_pen turtlesim//srv/SetPen \"{r: 255, g: 255, b: 255, width:2, 'off': 0}\"");
            }
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
