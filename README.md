# Goblin Run  
Mixed Reality Proof of Concept for Live Interactive TV Formats.
Project provided by IRRL HSLU Suurstoffi 12 Rotkreuz 6343 Switzerland

## Overview

Goblin Run is a Proof of Concept project that explores the feasibility of using mixed reality technologies in a live TV show like game format. The project combines real world environments, live video streams and computer generated characters to create an interactive experience for contestants and observers.

The concept can be described as a mix of The Amazing Race, Hunger Games and fantasy themed gameplay. Contestants navigate a real environment while encountering virtual enemies and events that are remotely controlled in near real time.

The project was developed as part of a university research and development module and focuses on technical feasibility rather than production readiness.

## Core Idea

Most existing TV shows use little to no mixed reality. When AR or VR is used, interaction between real participants and virtual elements is often limited or visually inconsistent.

Goblin Run investigates whether current consumer grade VR hardware and game engines are capable of supporting a live mixed reality format with the following characteristics.

- Real world movement in outdoor environments  
- Virtual enemies and events anchored to the environment  
- Remote control of gameplay events  
- Live monitoring of the contestant through video streams  

## System Architecture

The system is split into multiple loosely coupled components to keep complexity manageable and allow independent testing.

### VR Application

Runs on a Meta Quest headset and represents the contestant perspective.

Responsibilities include

- Rendering virtual enemies and obstacles  
- Receiving commands from the control system  
- Handling player interaction and game state  
- Optional passthrough for mixed reality perception  

### Controller Application

Runs on a workstation and acts as a control panel for operators.

Responsibilities include

- Triggering gameplay events  
- Spawning enemies and obstacles  
- Monitoring system state  
- Sending commands to the VR headset via network messaging  

### Geo App

A separate application used for location based logic.

Responsibilities include

- Sending GPS data  
- Triggering location based events  
- Extending the control system with spatial logic  

### Backend Server

Acts as a lightweight communication layer.

Responsibilities include

- WebSocket based command and control  
- State synchronization between components  
- Minimal latency communication  

## Technologies Used

**Game Engine**  
Unity

**Networking**  
WebSockets for command and control communication

**Hardware**  
Meta Quest VR headset  
Workstation for control and streaming  

**Development Environment**  
Visual Studio Code  
Git and GitHub for version control  

## Project Goals

The main goal of the project was to validate technical assumptions.

Key evaluation points

- Near real time communication between controller and VR headset  
- Stability of mixed reality content in outdoor environments  
- Feasibility of live monitoring and event control  
- Limitations of consumer grade AR and VR tracking systems  

The project does not aim to be a finished product. It is intentionally scoped as a Proof of Concept.

## Deliverables

- Functioning VR prototype  
- Controller application with event triggering  
- Live demo showing communication and interaction  
- Recorded video material including  
  - First person contestant view  
  - Third person camera view  
  - Live mixed reality composition  
- Scientific paper documenting methods, results and limitations  

## Known Limitations

- Tracking stability in large outdoor environments is limited  
- World anchoring relies on the Quest Guardian system  
- GPS based positioning is coarse and not suitable for precise alignment  
- Latency and bandwidth strongly depend on network conditions  

These limitations are documented and discussed in the accompanying paper.

## Repository Structure

The repository contains multiple applications and experimental components. Not all folders represent production ready code. Some parts exist purely for testing and evaluation.

Expect rough edges. This is intentional.

## Setup Notes

Exact setup steps depend on available hardware and network configuration.

In general

- Unity projects must be opened with the correct Unity version  
- The backend server must be reachable from all devices  
- Firewall and network restrictions can break communication  
- Outdoor testing requires manual calibration and iteration  

If something breaks unexpectedly, that is normal for this prototype.

## License

This project was created for academic purposes.  
No warranty. No production guarantees.

## Authors

Student project developed as part of a university mixed reality research module.
