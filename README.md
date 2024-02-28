# Drone Control System with Multi-Process Communication

Welcome to the Drone Control System repository! This project is designed to simulate and control drones in a dynamic environment using inter-process communication (IPC) techniques. The system includes multiple components such as drones, targets, obstacles, and a graphical user interface (GUI) implemented with the ncurses library for real-time visualization.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Setup](#setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Overview

The Drone Control System allows users to monitor and control drones in a simulated environment. The system consists of multiple processes communicating with each other through pipes, facilitating data exchange and coordination. The graphical interface provides a real-time view of the environment, enabling users to observe drone movements, interact with targets, and avoid obstacles.

## Features

- **Multi-Process Communication**: Utilizes IPC mechanisms such as pipes for seamless communication between different components.
- **Real-Time Visualization**: Implements an ncurses-based GUI for visualizing the simulated environment and drone activities.
- **Dynamic Environment**: Simulates a dynamic environment with moving targets and obstacles to test drone navigation and control algorithms.
- **Signal Handling**: Ensures robustness and reliability by handling signals, including those from the watchdog process.
- **Logging and Error Handling**: Logs process IDs and relevant information for debugging purposes, with comprehensive error handling to maintain system stability.
- **Configurability**: Supports command-line arguments for configuring system parameters such as the number of processes and file descriptors.

## Setup

To set up the Drone Control System on your machine, follow these steps:

1. **Clone the Repository**: Clone this repository to your local machine using `git clone`.
   ```bash
   git clone 
