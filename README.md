## ECE448-CellAutomataGA
A collection of Software and Scripts built and used in the Research of our Senior Design Project. Our goal was to determine if evolutionary algorithms can find emerging living cellular automata systems. Using a custom Genetic Algorithm, these tools have allowed us to explore different two-dimensional Automata and find new and interesting rulesets.

## Motivation
This project stemmed from planned research from our Project Advisor, [Dr. Peter Jamieson](http://www.drpeterjamieson.com/).
 
## Screenshots
| ![Terminal Screenshot of Genetic Algorithm](https://i.postimg.cc/j5fKgQq6/terminal.jpg) | 
|:--:| 
| *Terminal Screenshot of Genetic Algorithm in Ubuntu VM* |

| ![B3/S02378 Simulation](https://i.postimg.cc/zXm0WwRw/b3-s02378.gif) ![B37/S23678 Simulation](https://i.postimg.cc/CLLXdsyv/b37-s23678-rle.gif) ![B3/S01278 Simulation](https://i.postimg.cc/L6wDRtmg/b3-s01278.gif)| 
|:--:| 
| *Some Interesting Cellular Automaton found by our Genetic Algorithm* |
## Requirements & Dependencies 
The project was built in a VM of Ubuntu 20.04 LTS. The simulations needed to compute our Fitness were ran on [Golly](http://golly.sourceforge.net/), an open-source application built to explore different Cellular Automata. The Algorithm was developed in C++ and we use Python 3 Scripts to interface with Golly. We also make use of [RapidXML](http://rapidxml.sourceforge.net/)'s C++ Library to read our Configuration before any testing.

## Features
This project finds emergent Cellular Automata through the simulation of many rulesets. When properly tuned, the algorithm has found multiple interesting rulesets similar to Conway's Game of Life. This Repository also includes testing software to furthur experiment with known and unknown Cellular Automata, with the goal being to tune our Genetic Algorithm even further. 

## Installation
This Software Suite has many relational dependencies between the Scripts and Applications. Additionally, there are filesystem connections that need sorted out before being able to successfully run the Algorithm. This is solved through a fully encompassed VM Image that will be available for download. If interested, contact [me](mailto:halect2@miamioh.edu) for additional details.

## Credits
Our team consists of three Senior Miami University Students, Carter Hale, Owen Hichens, and Eric Schonauer. The project was overseen and advised by Dr. Peter Jamieson.

## License
MIT © [HaleCT2](https://github.com/HaleCT2)