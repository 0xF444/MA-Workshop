import os
import shutil
from zipfile import ZipFile
import hashlib
import zlib
import rarfile
import sys
import termcolor
hashalgo =['SHA256', 'CRC32', 'MD5', 'SHA512', 'SHA384']# Ordered ==> SHA256, CRC32, MD5, SHA512, SHA384
def crc32(fileName):
    with open(fileName, 'rb') as fh:
        hash = 0
        while True:
            s = fh.read(65536)
            if not s:
                break
            hash = zlib.crc32(s, hash)
        return "%08x" % (hash & 0xFFFFFFFF) # This is to be able to calculcate CRC32 for an entire file.
def sha256sum(filename):
    with open(filename, 'rb', buffering=0) as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()
def md5sum(filename):
    with open(filename, 'rb', buffering=0) as f:
        return hashlib.file_digest(f, 'md5').hexdigest()
def sha512sum(filename):
    with open(filename, 'rb', buffering=0) as f:
        return hashlib.file_digest(f, 'sha512').hexdigest()
def sha384sum(filename):
    with open(filename, 'rb', buffering=0) as f:
        return hashlib.file_digest(f, 'sha384').hexdigest()
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname) # Setting up the script for it to work in any directory.
if len(sys.argv) < 5:
    print("Insufficent Arguments.\nUsage: Sig_Detector.py -f [File] -d [Hash_Database] [[--remove-path]]")
    sys.exit(-1)
file_given = sys.argv[sys.argv.index("-f")+1] # Grab File from argv
DB_given = sys.argv[sys.argv.index("-d")+1] # Grab DB from argv
if file_given.endswith('.zip'):
    extraction_path = fr'{os.getcwd()}\Sig_Detector_Extraction' # This is where all the magic happens
    if not os.path.exists(extraction_path):
        os.makedirs(extraction_path) # Make sure that folder exists when you operate
    try:
        taskzipobj =  ZipFile(fr"{os.getcwd()}\{file_given}",'r')
    except FileNotFoundError:
        print("Zip file not found, please try again.")
        sys.exit(-1)
    taskzipobj.extractall(path=fr"{os.getcwd()}\Sig_Detector_Extraction")
    for root, dirs, files in os.walk(extraction_path):
        for file in files:
            if file.endswith('.zip'):
                file_path = os.path.join(root,file)
                try:
                    file_zip_obj = ZipFile(file_path,'r')
                    file_zip_obj.extractall(path=root)
                except Exception:
                    pass
            elif file.endswith('.rar'):
                file_path = os.path.join(root,file)
                try:
                    file_rar_obj = rarfile.RarFile(file_path)
                    file_rar_obj.extractall(path=root)
                except Exception:
                    pass
    taskzipobj.close()
    file_rar_obj.close()
    file_zip_obj.close()
    # Extraction Complete.
    try:
        Mal_Db = open(fr"{os.getcwd()}\{DB_given}")
    except FileNotFoundError:
        print("Hashes Database not found, please try again.")
        sys.exit(-1)
    for root,dirs,files in os.walk(extraction_path):
        for file in files:
            if file.endswith('.zip') or file.endswith('.rar'):
                continue # Skip zip, rar files
            file_path = os.path.join(root,file)
            file_hash = [sha256sum(file_path),crc32(file_path),md5sum(file_path),sha512sum(file_path),sha384sum(file_path)] # Ordered ==> SHA256, CRC32, MD5, SHA512, SHA384
            for indx, hash in enumerate(file_hash):
                Mal_Db.seek(0) # Put the cursor back to where it started.
                for entry in Mal_Db:
                    entry = entry.rstrip('\n')
                    if hash == entry:
                        print(f"{file_path} is {termcolor.colored("MALICIOUS.","red")}")
                        print(f"Malicious hash found: {termcolor.colored(f"{hash}","red")} ({termcolor.colored(hashalgo[indx],"cyan")})\n")
                        break
    Mal_Db.close()
    try:
        if sys.argv.index("--remove-path"):
            shutil.rmtree(extraction_path) # Remove the crime scene ;)
    except ValueError:
        pass
else:
    try:
        Mal_Db = open(fr"{os.getcwd()}\{DB_given}")
    except FileNotFoundError:
        print("Hashes Database not found, please try again.")
        sys.exit(-1)
    try:
        File_entry = fr"{os.getcwd()}\{file_given}"
    except FileNotFoundError:
        print("File not found, please try again.")
        sys.exit(-1)
    file_hash = [sha256sum(File_entry),crc32(File_entry),md5sum(File_entry),sha512sum(File_entry),sha384sum(File_entry)] # Ordered ==> SHA256, CRC32, MD5, SHA512, SHA384
    for indx,hash in enumerate(file_hash):
        Mal_Db.seek(0)
        for entry in Mal_Db:
            entry = entry.rstrip("\n")
            if hash == entry:
                print(f"{file_given} is {termcolor.colored("MALICIOUS.","red")}")
                print(f"Malicious hash found: {termcolor.colored(f"{hash}","red")} ({termcolor.colored(hashalgo[indx],"cyan")})\n")
                break
    Mal_Db.close()