#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/int32.hpp"
#include <cmath>
#include <algorithm>
#include <string>
#include <chrono>
using namespace std;

class Detector : public rclcpp::Node {
public:
    Detector() : Node("NF_detector"){
        publisher = this->create_publisher<std_msgs::msg::Int32>("detecting_NF", 10);
        subscriber = this->create_subscription<std_msgs::msg::Int32>("detecting_NF", 10, std::bind(&Detector::detect_NF, this, std::placeholders::_1));
        timer = this->create_wall_timer(std::chrono::milliseconds(420), std::bind(&Detector::time_detector, this));
        time_start = this->get_clock()->now();
        RCLCPP_INFO(this->get_logger(), "PROGRAM HAS STARTED");
    }
private:
    int i = 0, iteration = 0, event = 0;
    float time_count = 0.0f;
    float total_time = 0.0f;
    rclcpp::Time time_start, time_end;
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber;
    rclcpp::TimerBase::SharedPtr timer;

    void time_detector(){
        iteration++;
        auto msg = std_msgs::msg::Int32();
        msg.data = iteration;
        publisher->publish(msg);
    }

    void detect_NF(const std_msgs::msg::Int32::SharedPtr msg){
        int iteration = msg->data;
        if(i <= 10){
            check_palindrome(iteration);
            if(i == 10){
                auto time_end = this->get_clock()->now();
                total_time = (time_end-time_start).seconds();
                RCLCPP_INFO(this->get_logger(), "NF HAS FOUND!\nRUNTIME: %fs\nEVENTS DETECTED: %d", total_time, event);
                rclcpp::shutdown();
            }
        }
    }

    void check_palindrome(const int& iteration){
        string a = to_string(iteration);
        string b = a;
        reverse(a.begin(), a.end());
        if(a == b){
            event++;
            RCLCPP_INFO(this->get_logger(), "event count: %d", event);
            if(event != 0 && event%67 == 0){
                i++;
            }
        }
    }
};

    int main(int a, char **b){
        rclcpp::init(a,b);
        rclcpp::spin(std::make_shared<Detector>());
        rclcpp::shutdown();
        return 0;
    }
