# Smart Irrigation System

## Project Objective

The objective of this project is to develop an IoT-based smart irrigation system that automatically controls irrigation according to environmental conditions and soil moisture levels.

## Introduction
This project aims to develop a smart irrigation system using IoT technology.  
The system monitors soil moisture and environmental conditions,\
then automatically determines whether irrigation is needed.</br>

## Features
- Soil moisture monitoring
- Automatic irrigation
- Weather-based irrigation control
- Cloud data storage
- Real-time monitoring

## System Architecture
![System Architecture](Image/system_architecture.png)

## Hardware Components Description
| Component | Function |
|------------|------------|
| Moisture Sensor | Measure soil moisture level |
| UV Sensor | Detect sunlight intensity |
| Temperature Sensor | Measure environmental temperature |
| Water Flow Sensor | Monitor water consumption |
| Water Valve | Control water flow |
| Water Pump | Provide irrigation water |
| UV Lamp | Supplemental plant lighting |
| Water Pipe | Water transportation |
| ESP32 | IoT communication and control |

## Bill of Materials (BoM)
| Component | Quantity | Cost (NTD) |
|------------|----------|------------|
| Moisture Sensor | 1 | 102 |
| UV Sensor | 1 | 300 |
| Temperature Sensor | 1 | 120 |
| Water Flow Sensor | 1 | 360 |
| Water Valve | 1 | 177 |
| Water Pump | 1 | 100 |
| UV Lamp | 1 | 296 |
| Water Pipe | 1 | 101 |
| ESP32 | 1 | 269 |
| **Total** | | **1825** |

## Software / Technologies
| Technology | Purpose |
|------------|---------|
| Arduino IDE | ESP32 programming |
| AWS IoT Core | Device communication |
| AWS Lambda | Irrigation decision logic |
| AWS EventBridge | Scheduled irrigation checks |
| DynamoDB | Sensor data storage |
| Amazon S3 | Raw data backup |
| OpenWeather API | Weather forecasting |
| GitHub | Version control |
| Wix | Project website |

## Future Improvements
- Mobile application integration
- AI-based irrigation prediction
- Additional environmental sensors
- Larger-scale agricultural deployment
- Real-time alert notifications

## Results
The smart irrigation system successfully:
- Monitored soil moisture in real time
- Collected environmental data through sensors
- Sent sensor data to AWS cloud services
- Executed automatic watering decisions
- Stored historical data for future analysis
The project demonstrates the feasibility of combining IoT devices and cloud computing for intelligent irrigation management.
### product picture
<img src="Image/product.png" width="600">


## Website
[Project Website: 4.0 Smart Irrigation System](https://sonik6703.wixsite.com/smartirrigation)

## Team Members
| Name | Student ID | Main Responsibility |
|--------|--------|--------|
| Julian | E11415029 | Hardware Integration & Arduino Programming |
| 許智翔 | M11451035 | AWS Cloud |
| Sonia Martini | E11412004 | Presentation Slides & Website |
| 孫菱 | M11451025 | GitHub |
