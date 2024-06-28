import psutil
from datetime import datetime
import subprocess
import time
import os


def kill_process(process_name): # kill_mterm function
    """Kill all processes matching the given process name."""
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == process_name:
            #print(f"Killing {process_name} process with PID {proc.info['pid']}")
            proc.kill()
        else:
            print(f"{process_name} not found")


def kill_mterm_mcmdr(): # tests 1, 2, 3
    # Kill all mterm processes
    kill_process('mterm')

    # Kill all mcmdr processes
    kill_process('mcmdr')


def conf_4vsel(): # tests 1, 3, 4
    # lines 237-245
    # Define the paths and filenames
    OUTFL2 = '/v6.0/output/outfl2.txt'
    V4VSEL = '/v6.0/output/v4vsel.txt'
    ASIC = 'Your_ASIC_Value'  # Define your ASIC value

    v4vsel_file_path = os.path.join(OUTFL2, V4VSEL)
    
    # Check if the file exists and read the content
    if os.path.isfile(v4vsel_file_path):
        with open(v4vsel_file_path, 'r') as file:
            v4vsel = file.read().strip()
    else:
        v4vsel = 45
    
    # Prompt the user to confirm the v4vsel value
    new_v4vsel = input(f"Confirm {ASIC} 4VSEL value (current value: {v4vsel}):")
    
    # Write the new value to the output file
    with open(v4vsel_file_path, 'w') as file:
        file.write(new_v4vsel)


def upload_md(outfl, suf_log): # tests 1, 2, 3, 4
    #lines 201-204
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_message = f"{now} UPLOAD MD"

    with open(f"{outfl}{suf_log}", "a") as log_file:
        log_file.write(log_message + "\n")
        log_file.flush()  # Ensure the log is flushed to file immediately

        files_and_delays = [
            ("md00.txt", 2),
            ("md01.txt", 2),
            ("md02.txt", 2),
            ("md03.txt", 2),
            ("md04.txt", 3),
            ("md05.txt", 3),
            ("md06.txt", 4),
            ("md07.txt", 2),
        ]

        for file_name, delay in files_and_delays:
            print(file_name)
            with open(f"md/{file_name}", 'r') as file:
                subprocess.run(["./mupld.exe"], stdin=file)
            time.sleep(delay)


def init_scope():
    subprocess.run(["$SC_SET"], stdin=open("tek_normal.ini"))


def set_4vsel():
    # Define file paths (these should be set according to your environment)
    OUTFL2 = '/v6.0/output/outfl2.txt'
    V4VSEL = '/v6.0/output/v4vsel.txt'
    OUTFL = '/v6.0/output/outfl.txt'
    SUF_LOG = '/v6.0/output/logfile.txt'
    misc_4vsel = """ 4VSEL !
CMD!
:p
:p
:p
:q
"""
    # Check if the file exists
    if os.path.isfile(os.path.join(OUTFL2, V4VSEL)):
        with open(os.path.join(OUTFL2, V4VSEL), 'r') as file:
            v4vsel = file.read().strip()
        subprocess.run(['cat', os.path.join(OUTFL2, V4VSEL), 'misc_4vsel.txt'], stdout=open('/v6.0/output/set_4vsel.txt', 'w'))
    else:
        with open('4vsel_def.ini', 'r') as file:
            v4vsel = file.read().strip()
        with open ('/v6.0/output/set_4vsel.txt', 'w') as set_4vsel_file:
            set_4vsel_file.write('4vsel_def.ini' + misc_4vsel)
    
    # Log the date, time, and value of v4vsel
    log_entry = f"{datetime.now()} 4VSEL {v4vsel}\n"
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        logfile.write(log_entry)
        print(log_entry, end='')  # Also print to stdout
    
    # Run mupld_c.exe with the specified parameters
    subprocess.Popen(['./mupld_c.exe', '-p', '10', '-I', '_1.txt'])
    
    # Run mterm.exe with the specified parameters and log output
    mterm_process = subprocess.Popen(['./mterm.exe', '-d', '-c', '/dev/com4'], stdout=subprocess.PIPE)
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        for line in mterm_process.stdout:
            logfile.write(line.decode())
            print(line.decode(), end='')  # Also print to stdout


def run_globaldc(): 
    # From CHatGPT, revisit later to try and understand
    # Define the paths and filenames (replace these with your actual paths and filenames)
    OUTFL = '/v6.0/output/outfl.txt'
    SUF_LOG = '/v6.0/output/logfile.txt'
    TYP_GDC = '_globaldc'
    SUF_ANA = '_ana.txt'
    DMVS = './ana2/dmvs.exe'  # Path to the DMVS executable
    USER = 'your_username'  # Replace with the actual user name ???

    # Log the current date and "GLOBALDC"
    log_entry = f"{datetime.now()} GLOBALDC\n"
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        logfile.write(log_entry)
        print(log_entry.strip(), flush=True)  # Print to stdout

    # Run mupld_c.exe with "GLOBALDC" as input in the background
    subprocess.Popen(['./mupld_c.exe', '-p', '5'], stdin=subprocess.PIPE).communicate(input=b"GLOBALDC\n")

    # Run mterm.exe and log its output
    with open(os.path.join(OUTFL, TYP_GDC + SUF_LOG), 'w') as logfile:
        mterm_process = subprocess.Popen(['./mterm.exe', '-d', '-c', '/dev/com4'], stdout=subprocess.PIPE)
        for line in mterm_process.stdout:
            logfile.write(line.decode())
            print(line.decode(), end='', flush=True)  # Print to stdout

    # Run DMVS with the log file as input and log its output
    with open(os.path.join(OUTFL, TYP_GDC + SUF_LOG), 'r') as log_input, \
         open(os.path.join(OUTFL, TYP_GDC + SUF_ANA), 'w') as ana_output:
        dmvs_process = subprocess.Popen([DMVS, '-usr', USER], stdin=log_input, stdout=subprocess.PIPE)
        for line in dmvs_process.stdout:
            ana_output.write(line.decode())
            print(line.decode(), end='', flush=True)  # Print to stdout


def run_mterm():
    pass


def run_stimtest():
    pass


def run_hiroplot(stimtime):
    OUTFL = '/v6.0/output/outfl.txt'
    SUF_BIN = '/v6.0/output/'

    print(f"Wait until STIM data collect for {stimtime} second.")
    print("\n")

    time.sleep(stimtime)
    print("\n")
   
    print("If you see orange row for first and second and rest as blue,")
    print("   STIM scan worked fine.")
    print("To quit, type :e")
    print("\n")

    subprocess.run(['./hiroplot-db.exe', '-I', f'{OUTFL}{SUF_BIN}', '-m'])