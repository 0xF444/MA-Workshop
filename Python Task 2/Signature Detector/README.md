# Signature Detector
- This program is a command line utility built in Python that attempts to recursively unzip and compare file hashes to a hash database.
- **NOTE**: This script works in Python 3.11 and above.
## Usage
```batch
sig_detector.py -d <Database_File> -f <Your_File> [[--remove-path]]
```
Example: ```sig_detector.py -d Mal-Hashes.txt -f Task2.zip --remove-path

| Option         | Function     
|--------------|-----------|
| -d | Provide the hash database to compare with.|
|-f | Target File.  |
| --remove-path | Removes the extraction path for target zip files.

- **NOTE**: The program can also take normal files and it'll compare this file *only*.
- If you want to add a new hash to the database, just append it to the hashes database files.
## Hashes Database
- Each hash is put inside with lines seperated between each entry.
- You may put the following hashing and checksum algorithms: SHA256, SHA512, SHA386, MD5, CRC32