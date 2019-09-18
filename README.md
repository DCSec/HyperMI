# HyperMI:  A Privilege-level Secure Execution Environment #

# Table of Contents #

* [Introduction](#introduction)    
    * [The modified Host OS kernel](#the-modified-host-os-kernel)
    * [The HyperMI World](#the-hypermi-world)
    * [The Switch Gate](#the-switch-gate)
* [Compilation and Installation](#compilation-and-installation)
    * [Compilation and Installation Steps](#Compilation-and-Installation-Steps)
    * [Modified Files](#modified-files)
* [Acknowledgement ](#acknowledgement)


# Introduction # 
**HyperMI** is a *secure* and *in-kernel isolated* execution environment to protect Virtual Machines (VMs) against the compromised hypervisor.  Currently, we designed and implemented **HyperMI** on the KVM-based cloud computing environment.   
<!-- **HyperMI** can resist on the following kinds of attacks:  -->
<!-- 1. Attacks from the inner attackers. Inner attacker can use cloud environment management tools with no limitation, thus, malicious code can be imported into the hypervisor/Host OS kernel with no limitation. The security of the guest VMs can no longer be guaranteed once the malicious code are injected.  -->
<!-- 2. Attacks from the outside attackers.  -->
<!--     * A successful VM Escaping Attack grants the attacker with access to the hypervisor/Host OS.  -->
<!--     * Remapping Attack, Double-mapping Attack or VM Memory Isolation Destruction Attack may be conducted  -->

**HyperMI**, as shown in Figure 1, consists of three parts: the modified host OS kernel (not trusted), the HyperMI World and the Switch Gate.     

![HyperMI Architecture](./doc/pic/architecture.png "HyperMI Architecture")

## The Modified Host OS Kernel ##
Since the hypervisor/Host OS is not trusted by us, access to some critical data structures must be restricted. The following modifications were made to the Host OS Kernel by **HyperMI**
1. Virtual Machines Control Structure (VMCS) and Extended Page Table (EPT) are removed from the original kernel. 
2. Functions which are relevant to Control Registers (CR0, CR3, CR4), VMCS, EPT, DMA are hooked. 

## The HyperMI World ##
The HyperMI World is the *secure* and *in-kernel isolated* execution environment to host important data structures and security tools. **HyperMI** has full capability of monitoring the Hypervisor and protecting critical data structures. Currently, the HyperMI world hosts the following data structures and security tools: 
1. Important Data Structures: 
    * Virtual Machines Control Structure (VMCS)
    * Extended Page Table (EPT)
2. Security Tools: 
    * Verification Program for Control Registers and DMA Mapping 
    * Physical Page Tag Table (PPTT)    
    
Verification Program for Control Registers and DMA Mapping is used to prevent the compromised hypervisor/Host OS kernel from subverting our HyperMI World.    
EPT behaviors are controlled by the three control bits in PPTT: PFN(Physical Page Frame Number), OwnerID and Shared ID. With the help of PPTT, the way to subvert the isolation of different VMs through dedicated EPT entries is hindered. 

## The Switch Gate ##
The Switch Gate is the only interface between the Normal World (the compromised hypervisor/Host OS Kernel) and the HyperMI world. Figure 2 shows the detail of this Switch Gate.    
![The Switch Gate](./doc/pic/the-switch-gate.png "The Switch Gate")   

The Switch Gate may be similar with the Switch Gate proposed by [SKEE](https://wenboshen.org/assets/papers/skee-ndss16.pdf) at the first look. But, there are several differences between them: 
1. **HyperMI** implemented the Switch Gate in x86\_64 environment, while SKEE just implemented their work based on ARM v7 and ARM v8. 
2. **HyperMI** implemented the Switch Gate by using a new kernel page table hosted in the HyperMI world, the access to the original kernel page table are restricted and redirected to the kernel page table in the HyperMI world. This approach is totally different from the approach adopted by SKEE. 


# Compilation and Installation #

## Compilation and Installation Steps ##

The compilation and installation process of HyperMI contains two step: 
1. Compile and install the modified linux kernel 
    ```shell 
    git clone https://github.com/DCSec/HyperMI.git 
    cd HyperMI 
    cp -r src/linux-4.4.1 /usr/src
    cd /usr/src/linux-4.4.1
    make oldcofigure && make modules && make modules_install&& make install 
    ```
2. Compile and install SecureVM 
    ```shell 
    cd home/to/HyperMI
    cd src/SecureVM
    make && insmod secureVM.ko
    ```

## Modified Files ##
HyperMI requires a modified kernel to provide the *secure* and *in-kernel isolation* execution environment. The modified source code files are listed as following:   
```shell
    fault.c 
    ksm.h 
    kvm_host.h 
    mmu.c 
    mmu.h 
    mmu_audit.c 
    pgtable_64.h 
    secureVM.h 
    types.h 
    virtio_balloon.c 
    vmscan.c 
    vmx.c 
    vmx.h 
    vmx_header_modify.h 
    vmx_modify.h
```


# Acknowledgement #
* This work has been pushlished in The 18th IEEE International Conference on Trust, Security and Privacy in Computing and Communications (TrustCom 2019). 
<!-- For more information, please read:   -->
<!-- * This work is supported by the National Key R&D Program of China.  -->


































