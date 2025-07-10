# CityUID

Flipper Zero application to extracts and displays basic information student ID cards from the City University of Hong Kong.

This repository is a fork of the original [UDECard project](https://github.com/hahnworks/UDECard) by hahnworks, adapted for City University of Hong Kong student ID cards.

## Motivation

This project was born from a curiosity about NFC technology and a desire to understand how student ID cards work at City University of Hong Kong. While exploring the technical aspects of MIFARE Classic 1K cards, I discovered that the Flipper Zero lacked proper tools for analyzing the data structure of these cards.

This application serves as an educational tool for students and researchers interested in NFC technology, providing a legal and ethical way to understand card data structures without compromising security. It demonstrates how to parse and display card information while respecting institutional policies and regulations.

**Educational Goals:**
- Learn about MIFARE Classic card data structures
- Understand NFC technology and its applications
- Provide a safe, legal framework for studying card systems
- Share knowledge with the academic community

## Legal and Ethical Compliance

**⚠️ Important: City University of Hong Kong Student ID Card Policy**

According to the [City University of Hong Kong Student Identity Card Regulations](https://www.cityu.edu.hk/arro/regu/regu_idru.htm):

> "Alteration, falsification, or misuse of the Student Identity Card is a violation of the Code of Student Conduct, and other related university rules and regulations, and will be subject to disciplinary action. Fraudulent use of the Student Identity Card may also result in criminal charges and/or civil proceedings."

**For legal and ethical reasons, this application:**
- Does NOT include any keys for reading CityU student ID cards that are not already available in standard Flipper Zero dictionaries
- Is limited to parsing card contents only (read-only functionality)
- Does NOT enable any modifications to the card data
- Will NOT provide any additional keys or methods to access protected card sectors

Your Flipper Zero must have the standard `mf_classic_dict.nfc` installed to use this application.

Should you have any Intercards from other institutions, you may find the [Technical Details](docs/Technical_Details.md) document helpful.

## Properties this app supports

- Student ID Number
- Card Issue Date
- Card Expiry Date
- Student Name
- Admit Date


## Usage

### Reading directly from the card

- Select **Read**.
- Place the card on the back of the flipper.

### Loading previously made dump

- Use the Flipper’s built-in NFC App to create a dump of your card.
- Open UDECard and select **Load**.
- Select the previously created dump.

## Acknowledgements

The following images and assets are currently inherited from the original [UDECard project](https://github.com/hahnworks/UDECard):

- `images/ApplyFlipperToUDE_60x50.png` is based on `NFC_manual_60x50.png` from the [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
- `images/loading_24x24.png` is taken from `Loading_24` from the [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)
- `images/Nfc_10x10.png` is taken from `Nfc_10px.png` from the [Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware)

*This section will be updated if/when original images are created for CityUID.*
