# Smart Irrigation System

## Project Objective
The objective of this project is to develop an IoT-based smart irrigation system that automatically controls irrigation according to environmental conditions and soil moisture levels.

## Features
- Soil moisture monitoring
- Automatic irrigation
- Weather-based irrigation control
- Cloud data storage
- Real-time monitoring

## System Architecture
<img src="Image/system_architecture.jpg" width="500">

The system collects environmental data through sensors connected to ESP32.\
Sensor data are transmitted to AWS cloud services for storage and analysis. Based on soil moisture levels and weather forecasts, the system automatically decides whether irrigation is required.

## Prototype
<img src="Image/System_Exterior.jpg" width="300">
<img src="Image/System_Interior.jpg" width="300">

## Hardware Components Description
| Component | Function |
|------------|------------|
| ESP32 | Main controller for sensor data collection, processing, and cloud communication |
| Soil Moisture Sensor | Measures soil moisture level to determine irrigation needs |
| Temperature/Humidity Sensor (DHT11) | Monitors environmental temperature and humidity |
| Water Flow Sensor | Measures water flow rate and irrigation water consumption |
| UV Sensor | Detects UV intensity and sunlight conditions |
| Water Pump | Provides water for irrigation |
| Motor Driver | Controls the operation of the water pump |
| LCD Display | Displays real-time sensor readings and system status and provide electricity for component|

## Bill of Materials (BoM)
| Component | Quantity | Cost (NTD) |
|------------|----------|------------|
| Moisture Sensor | 1 | 102 |
| UV Sensor | 1 | 300 |
| Temperature Sensor | 1 | 120 |
| Water Flow Sensor | 1 | 360 |
| Motor Driver | 1 | 315 |
| Water Pump | 1 | 100 |
| ESP32 | 1 | 269 |
| **Total** | | **1566** |

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

## AWS Cloud Services
| AWS Service | Function |
|-------------|------------|
| IoT Core – Device Shadow | Tracks real-time device status and enables remote control |
| IoT Core – Message Routing Rule | Routes sensor data to cloud services automatically |
| Lambda – Watering Decision | Determines irrigation actions based on soil moisture and UV levels |
| Lambda – Shadow Update | Updates device status after watering events |
| EventBridge Scheduler | Performs scheduled daily irrigation checks |
| DynamoDB | Stores historical sensor data for analysis |
| S3 | Archives raw sensor data for long-term storage |

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


## Website
[Project Website: 4.0 Smart Irrigation System](https://sonik6703.wixsite.com/smartirrigation)

## Team Members
| Name | Student ID | Main Responsibility |
|--------|--------|--------|
| Julian | E11415029 | Hardware Integration, Sensor Deployment, Arduino Programming |
| 許智翔 | M11451035 | AWS IoT Core, Lambda, Cloud Infrastructure |
| Sonia Martini | E11412004 | Presentation Design, Website Development |
| 孫菱 | M11451025 | GitHub Repository Management, Documentation |
