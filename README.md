# lkm_synch
Linux Kernel Module implementing a synchronization barrier for processes

This is a project assignement from the 2nd year Master Course ["Operating Systems II" (Italian)](http://www.dis.uniroma1.it/~quaglia/DIDATTICA/SO-II-6CRM/esame.html) at DIAG / Sapienza University of Rome.

The Operating Systems II course aims at presenting advanced design/implementation methods and techniques for modern operating systems. The topics dealt with by the course are bind to case studies oriented to LINUX systems and x86 compliant processors. The course requires basic knowledge on the structure and functionalities of operating systems, and knowledge on C programming.

The examination also requires the development of software sub-systems to be embedded within the LINUX kernel.

The specification for the sub-system implemented in this project are the following:

 >Develop a synchronization system based on barrier that has to be handle synchronization TAG belonging to the interval [0,31]. The synchronization system interface (system-calls) must be the following:

>int get_barrier(key_t key, int flags) - install a barrier with the given key and returns the barrier descriptor; the flags can ether be simple opening, exclusive or non exclusive installation

>int sleep_on_barrier(int bd, int tag) - request sleepon barrier with the given tag

>int awake_barrier(int bd, int tag) - wakeup from the barrier the procceses with the given tag

>int release_barrier(int bd) - uninstall the barrier

>In addition, it has to handle the wakup of a signaled process.e handled by this subsystem is of 16KB), and that there exist a limit to the number of sessions opened in parallel.

This assignement had to be carried out in teams of two persons, and my teammate was Eleonora Calore.

Our implementation targets the 3.2.0-31 Linux Kernel.
