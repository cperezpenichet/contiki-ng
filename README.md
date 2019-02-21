# Contiki-NG: The OS for Next Generation IoT Devices

[![Build Status](https://travis-ci.org/contiki-ng/contiki-ng.svg?branch=master)](https://travis-ci.org/contiki-ng/contiki-ng/branches)
[![license](https://img.shields.io/badge/license-3--clause%20bsd-brightgreen.svg)](https://github.com/contiki-ng/contiki-ng/blob/master/LICENSE.md)
[![Latest release](https://img.shields.io/github/release/contiki-ng/contiki-ng.svg)](https://github.com/contiki-ng/contiki-ng/releases/latest)
[![GitHub Release Date](https://img.shields.io/github/release-date/contiki-ng/contiki-ng.svg)](https://github.com/contiki-ng/contiki-ng/releases/latest)
[![Last commit](https://img.shields.io/github/last-commit/contiki-ng/contiki-ng.svg)](https://github.com/contiki-ng/contiki-ng/commit/HEAD)

Contiki-NG is an open-source, cross-platform operating system for Next-Generation IoT devices. It focuses on dependable (secure and reliable) low-power communication and standard protocols, such as IPv6/6LoWPAN, 6TiSCH, RPL, and CoAP. Contiki-NG comes with extensive documentation, tutorials, a roadmap, release cycle, and well-defined development flow for smooth integration of community contributions.

Unless excplicitly stated otherwise, Contiki-NG sources are distributed under
the terms of the [3-clause BSD license](LICENSE.md). This license gives
everyone the right to use and distribute the code, either in binary or
source code format, as long as the copyright license is retained in
the source code.

Contiki-NG started as a fork of the Contiki OS and retains some of its original features.


## Carrier-assisted Communications Extensions for Cooja

This repository branch contains extensions to Contiki-NG to support simulating carrier-assisted (backscatter) communications with Cooja and MSPSim. Details on the implementation and radio medium models see the paper: "**Modelling Battery-free Communications for the Cooja Simulator**" 
by Carlos Pérez-Penichet, Georgios Theodoros Daglaridis, Dilushi Piumwardane and Thiemo Voigt in **EWSN 2019**.

### Usage

1. Clone this repository.
2. Switch to the `carrier_assited` branch if necessary with: `git checkout carrier_assisted`
3. Download submodules with `git submodule update --init --recursive`. This will clone repositories with customized versions of Cooja and MSPSim for carrier-assisted communications. 
4. Start Cooja as usual by doing: `cd tools/cooja` and then `ant run`.

### Examples

Some of the examples used for the evaluation in the paper are available for testing. See under [`examples/battery-free`](./examples/battery-free).


Find out more:

* GitHub repository: https://github.com/contiki-ng/contiki-ng
* Documentation: https://github.com/contiki-ng/contiki-ng/wiki
* Web site: http://contiki-ng.org

Engage with the community:

* Gitter: https://gitter.im/contiki-ng
* Twitter: https://twitter.com/contiki_ng
