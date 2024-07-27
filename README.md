<!-- This was Created By Oumayma Guizeni  -->
<!-- Date : july 26 2024 -->

<div align="center">
  <img src="https://www.itrelease.com/wp-content/uploads/2017/11/Sequential-processing-vs-parallel-processing.jpg" alt="Testing" width="400" height="300" >

  <br/>

  <h3><b>sequential-vs-parallel-Image-Processing</b></h3>

</div>

---

[![C](https://img.shields.io/badge/C-%2300599C.svg?style=flat&logo=c&logoColor=white)](https://www.learn-c.org/)
[![OpenCV](https://img.shields.io/badge/OpenCV-%23white.svg?style=flat&logo=opencv&logoColor=white)](https://opencv.org/)
[![StarPU](https://img.shields.io/badge/StarPU-%23006400.svg?style=flat&logo=starpu&logoColor=white)](https://starpu.gitlabpages.inria.fr/)
[![Bash](https://img.shields.io/badge/Bash-%23121011.svg?style=flat&logo=gnu-bash&logoColor=white)](https://www.gnu.org/software/bash/)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/Naereen/StrapDown.js/graphs/commit-activity)

---

<br>
<br>

# 📗 Table of Contents

- [Overview](#overview)
- [Live_Demo](#live_demo)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [FAQ](#faq)
- [License](#license)

<br>
<br>

---

# 📖  Overview <a name="overview"></a>

<p> This project demonstrates the differences in performance between sequential and parallel processing of images, converting them to grayscale using OpenCV and parallel processing techniques in C </p>

<br>
<br>

## 🚀 Live_Demo <a name="live_demo"></a>

- [Live Demo Link](link_here)

<br>
<br>


## 💻 Requirements <a name="requirements"></a>

List of dependencies and requirements needed to run the script:

	OpenCV
	StarPU
	g++
	Bash


<br>
<br>

## 🛠 Installation <a name="installation"></a>

Ensure you have the necessary dependencies installed:

1. **Install OpenCV**:
Follow the [OpenCV installation guide](https://docs.opencv.org/master/d2/de6/tutorial_py_setup_in_ubuntu.html) for your operating system.

2. **Install StarPU**:
Follow the [StarPU installation guide](http://starpu.gitlabpages.inria.fr/tutorials/installing_starpu.html) for your operating system.

3. **Clone the repository**:

```bash
git clone https://github.com/OumaymaGuizeni/sequential-vs-parallel-Image-Processing--Grayscale-.git
cd sequential-vs-parallel-Image-Processing-(Grayscale)
```

<br>
<br>

## 💻  Usage <a name="usage"></a>

1. Sequential Processing: To run the sequential processing implementation:
   
   ```bash
   ./test_seq
   ```
   
2. Parallel Processing: To run the parallel processing implementation:
   
   ```bash
   ./test_para
   ```

3. Automated the process of run
   <p> A bash script comparaison.sh is provided to run both implementations and compare their performance.</p>
   a) Ensure the script has execute permissions:

   ```bash
   chmod +x comparaison.sh
   ```
   
   b) Run the script:

   ```bash
   ./comparaison.sh
   ```

<br>
<br>


## ❓ FAQ  <a name="faq"></a>

<p> If you have any questions, please feel free to reach out or create an issue in the repository. </p>
<br>
<br>


## Contributing

This repository is private and not open for public contributions.

<br>
<br>


## 📝 License <a name="license"></a>

This project is proprietary and all rights are reserved.
