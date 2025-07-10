# Technical Details - CityU Student ID Cards

> [!IMPORTANT]
> This documentation is for educational purposes only.
> No security keys or authentication mechanisms are exposed.
> This application is read-only and does not enable any modifications.

## Security and Legal Compliance

- **Read-Only Access**: Application designed for read-only data extraction only
- **No Security Exposure**: No authentication keys or security mechanisms documented
- **Educational Purpose**: Documentation intended for learning NFC technology
- **Policy Compliance**: Respects City University of Hong Kong regulations
- **No Modification**: No capability to alter card data

## Educational Objectives

This documentation serves to:
- Demonstrate MIFARE Classic card structure analysis
- Provide insight into student ID card systems
- Support NFC technology education
- Show legitimate reverse engineering practices

## Overview

City University of Hong Kong student ID cards use MIFARE Classic 1K format. For detailed technical specifications of the MIFARE Classic EV1 1K card, refer to the [official NXP documentation](https://www.nxp.com/docs/en/data-sheet/MF1S50YYX_V1.pdf).

## Data Structure

### Student Information Fields

| Field | Location | Format | Length | Description |
|-------|----------|--------|--------|-------------|
| Student ID Number | Block 1 (Sector 0) | ASCII, space-padded | 12 bytes | Student identification number |
| Card Expiry Date | Block 4 (Sector 1) | ASCII, YYYYMMDD | 16 bytes | Card expiration/graduation date |
| Card Issue Date | Block 5 (Sector 1) | ASCII, YYYYMMDD | 16 bytes | Date when card was issued |
| Student Name | Block 12 (Sector 3) | ASCII, space-padded | 16 bytes | Full student name |
| Admit Date | Block 29 (Sector 7) | ASCII, YYYYMMDD | 16 bytes | Student admission date |
