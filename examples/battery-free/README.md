# Examples for Cooja's Carrier-assisted Communications Extensions

These are examples on how to use the extensions to Contiki-NG to support simulating carrier-assisted (backscatter) communications with Cooja and MSPSim. Details on the implementation and radio medium models see the paper: "**Modelling Battery-free Communications for the Cooja Simulator**" 
by Carlos Pérez-Penichet, Georgios Theodoros Daglaridis, Dilushi Piumwardane and Thiemo Voigt in **EWSN 2019**.

This directory contains several different projects:

### carrier-generator

This is a very simple program that simply instructs a node (`Sky`) to constantly generate and unmodulated carrier.

### tag

This is is code intended to run in the simulated carrier-assisted devices (use the `Backscatter Tag` platform in Cooja). `example-PA.c` and `example-SA-TDMA.c` are the files used in the paper evaluation for "Pure ALOHA" and for "Slotted ALOHA" and "TDMA" respectively.

### beacon-node

The code in `beacon-node` is intended to run in a regular node (`Sky` or other). This is the interrogating node in the examples in the paper. 

## Cooja Simulation Files

Some `.scs` files with pre-defined Cooja simulations are also included to facilitate testing.

