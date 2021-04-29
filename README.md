## CellAutomataGA
A collection of Software and Scripts built and used in the Research of our Senior Design Project. Our goal was to determine if evolutionary algorithms could find emerging living cellular automata systems. Using a custom Genetic Algorithm, these tools have allowed us to explore different two-dimensional Automata and find new and interesting rulesets. 

## Motivation
This project stemmed from planned research from our Project Advisor, [Dr. Peter Jamieson](http://www.drpeterjamieson.com/).
 
## Screenshots
<center>

| ![Terminal Screenshot of Genetic Algorithm](https://i.postimg.cc/xT8LQ4k5/Terminal.jpg) | 
|:--:| 
| *Terminal Output of Genetic Algorithm in Ubuntu VM* |

<br />

| ![Collection of Cellular Automata](https://i.postimg.cc/NfPWn2HF/Cellular-Automata.gif) | 
|:--:| 
| *Some of the Emergent Cellular Automata found by our Genetic Algorithm* |

</center>

## Requirements & Dependencies 
The project was built in a VM of Ubuntu 20.04 LTS. The simulations needed to compute our Fitness were ran on [Golly](http://golly.sourceforge.net/), an open-source application built to explore different Cellular Automata. The Algorithm was developed with C++17 and used Python 3 Scripts to interface with Golly. We also make use of [RapidXML](http://rapidxml.sourceforge.net/)'s C++ Library to read our Configuration before any testing.

## Features
This project finds emergent Cellular Automata through the simulation of many rulesets. When properly tuned, the algorithm has found multiple interesting rulesets similar to Conway's Game of Life. This Repository also includes testing software to further experiment with known and unknown Cellular Automata, with the goal being to tune our Genetic Algorithm even further. 

## Installation
This Software Suite has many relational dependencies between the Scripts and Applications. Additionally, there are filesystem connections that need sorted out before being able to successfully run the Algorithm. This is solved through a fully encompassed VM Image that is available for [download](https://drive.google.com/file/d/1XToRe16e2IZbmlWRZCWrsQ4wYAn_fCII/view?usp=sharing). If interested, contact [Carter](mailto:halect2@miamioh.edu) for additional details.

## Credits
Our team consists of three Senior Miami University Students: Carter Hale, Owen Hichens, and Eric Schonauer. The project was overseen and advised by Dr. Peter Jamieson.

## License
MIT © [HaleCT2](https://github.com/HaleCT2)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4725604.svg)](https://doi.org/10.5281/zenodo.4725604)
