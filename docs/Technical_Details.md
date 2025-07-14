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

## Card Identification

### Magic Bytes
CityU student ID cards can be identified by specific magic bytes in Block 2:

**Block 2 Magic Pattern**: `30 31 33 30 20 20 20 20 20 20 20 20 20 20 20 20`

This pattern appears to be ASCII characters "0130" followed by space padding, which may represent a card type identifier or version code.

## Data Structure

### Student Information Fields

| Field | Location | Format | Length | Description |
|-------|----------|--------|--------|-------------|
| Student ID | Block 1 (Sector 0) | ASCII, space-padded | 12 bytes | Student identification number in format "SDXXXXXXXX#" |
| Card Expiry Date | Block 4 (Sector 1) | ASCII, YYYYMMDD | 16 bytes | Card expiration/graduation date |
| Card Issue Date | Block 5 (Sector 1) | ASCII, YYYYMMDD | 16 bytes | Date when card was issued |
| Student Name | Blocks 12-17 (Sector 3) | ASCII, space-padded | 96 bytes | Full student name spanning multiple blocks |
| Admit Date | Block 29 (Sector 7) | ASCII, YYYYMMDD | 16 bytes | Student admission date |

### Field Details

#### Student ID Format (Block 1)
- **Format**: `SDXXXXXXXX#`
- **Components**:
  - `SD`: Prefix indicating "Student" (fixed)
  - `XXXXXXXX`: 8-digit student number
  - `#`: Card issue number (0 = first card, 1 = second card, etc.)
- **Example**: `SD123456780` (Student 12345678, first card issued)

#### Date Format
All dates are stored in **YYYYMMDD** format:
- **Card Expiry**: `20250615` (2025-06-15)
- **Card Issue**: `2023090120230901` (appears to be duplicated data)
- **Admit Date**: `20220820` (2022-08-20)

#### Student Name Storage (Blocks 12-17)
- **Format**: Full name in mixed case with space padding
- **Storage**: Spans multiple blocks (Blocks 12, 13, 14, 16, 17)
- **Note**: Block 15 contains sector trailer data, not name data
- **Example**: `CHAN Tai Man` stored as `CHAN Tai Man    ` (with space padding)

### Block Layout

| Block | Content | Notes |
|-------|---------|-------|
| 0 | UID/Manufacturer Data | Card unique identifier |
| 1 | Student ID | Format: SDXXXXXXXX# |
| 2 | Magic Bytes | "0130" + space padding |
| 3 | Sector 0 Trailer | Authentication keys |
| 4 | Card Expiry Date | YYYYMMDD format |
| 5 | Card Issue Date | YYYYMMDD format (duplicated) |
| 6 | Empty/Reserved | All zeros |
| 7 | Sector 1 Trailer | Authentication keys |
| 8-11 | Empty/Reserved | All zeros or space padding |
| 12 | Student Name (Part 1) | First part of full name |
| 13 | Student Name (Part 2) | Middle part of full name |
| 14 | Student Name (Part 3) | Last part of full name |
| 15 | Sector 3 Trailer | Authentication keys (not name data) |
| 16 | Student Name (Part 4) | Additional name data |
| 17 | Student Name (Part 5) | Final name data |
| 18-19 | Empty/Reserved | All zeros |
| 20-22 | Empty/Reserved | All zeros |
| 23 | Sector 5 Trailer | Authentication keys |
| 24-26 | Empty/Reserved | All zeros |
| 27 | Sector 6 Trailer | Authentication keys |
| 28 | Empty/Reserved | Space padding |
| 29 | Admit Date | YYYYMMDD format |
| 30 | Empty/Reserved | All zeros |
| 31 | Sector 7 Trailer | Authentication keys |
| 32-34 | Empty/Reserved | Space padding |
| 35 | Sector 8 Trailer | Authentication keys |
| 36-37 | Empty/Reserved | Space padding |
| 38 | Empty/Reserved | All zeros |
| 39 | Sector 9 Trailer | Authentication keys |
| 40-42 | Empty/Reserved | All zeros |
| 43 | Sector 10 Trailer | Authentication keys |
| 44-46 | Empty/Reserved | All zeros |
| 47 | Sector 11 Trailer | Authentication keys |
| 48-50 | Empty/Reserved | All zeros |
| 51 | Sector 12 Trailer | Authentication keys |
| 52-54 | Empty/Reserved | All zeros |
| 55 | Sector 13 Trailer | Authentication keys |
| 56-58 | Empty/Reserved | All zeros |
| 59 | Sector 14 Trailer | Authentication keys |
| 60-62 | Empty/Reserved | All zeros |
| 63 | Sector 15 Trailer | Authentication keys |

### Data Processing Notes

- **Space Padding**: Most text fields are padded with ASCII space characters (0x20)
- **Mixed Case Names**: Student names are stored in mixed case format (e.g., "CHAN Tai Man")
- **Date Validation**: All dates follow YYYYMMDD format
- **Student ID Validation**: Must start with "SD" followed by 8 digits and a card number
- **Empty Blocks**: Blocks 6, 8-11, 18-22, 24-26, 28, 30, 32-34, 36-37, 38, 40-42, 44-46, 48-50, 52-54, 56-58, 60-62 contain either all zeros or space padding
- **Sector Trailers**: Every 4th block (3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63) contains authentication keys and access control bits
- **Name Data Continuity**: Student name spans blocks 12, 13, 14, 16, 17 (block 15 is sector trailer)
- **Date Duplication**: Card issue date appears to be stored with duplication in block 5