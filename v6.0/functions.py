import psutil
from datetime import datetime
import subprocess
import time
import os

# TEST 1
def kill_process(process_name):
    """Kill all processes matching the given process name using Cygwin commands."""
    try:
        # List all processes
        result = subprocess.run(['ps', '-a'], stdout=subprocess.PIPE, text=True)
        processes = result.stdout.splitlines()

        for process in processes:
            if process_name in process:
                pid = process.split()[0]
                subprocess.run(['kill', '-9', pid])
                print(f"Killed {process_name} process with PID {pid}")
    except Exception as e:
        print(f"Error occurred: {e}")

def kill_mterm_mcmdr():
    # Kill all mterm processes
    kill_process('mterm')

    # Kill all mcmdr processes
    kill_process('mcmdr')



def conf_4vsel(): 
    # lines 237-245
    # Define the paths and filenames
    global OUTFL2
    global V4VSEL
    OUTFL2 = './v6.0/output/outfl2.txt'
    V4VSEL = './v6.0/output/v4vsel.txt'
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


def upload_md(outfl, suf_log): 
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
    OUTFL2 = './v6.0/output/outfl2.txt'
    V4VSEL = './v6.0/output/v4vsel.txt'
    OUTFL = './v6.0/output/outfl.txt'
    SUF_LOG = './v6.0/output/logfile.txt'
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
        subprocess.run(['cat', os.path.join(OUTFL2, V4VSEL), misc_4vsel], stdout=open('./v6.0/output/set_4vsel.txt', 'w'))
    else:
        with open('4vsel_def.ini', 'r') as file:
            v4vsel = file.read().strip()
        with open ('./v6.0/output/set_4vsel.txt', 'w') as set_4vsel_file:
            set_4vsel_file.write('4vsel_def.ini' + misc_4vsel)
    
    # Log the date, time, and value of v4vsel
    log_entry = f"{datetime.now()} 4VSEL {v4vsel}\n"
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        logfile.write(log_entry)
        print(log_entry, end='')  # Also print to stdout
    
    # Run mupld_c.exe with the specified parameters
    subprocess.Popen(['./mupld_c.exe', '-p', '10', '-I', './v6.0/output/set_4vsel.txt'])
    
    # Run mterm.exe with the specified parameters and log output
    mterm_process = subprocess.Popen(['./mterm.exe', '-d', '-c', '/dev/com4'], stdout=subprocess.PIPE)
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        for line in mterm_process.stdout:
            logfile.write(line.decode())
            print(line.decode(), end='')  # Also print to stdout


def run_globaldc(): 
    # From CHatGPT, revisit later to try and understand
    # Define the paths and filenames (replace these with your actual paths and filenames)
    global SUF_LOG
    OUTFL = './v6.0/output/outfl.txt'
    SUF_LOG = './v6.0/output/logfile.txt'
    TYP_GDC = '_globaldc'
    SUF_ANA = '_ana.txt'
    DMVS = './ana2/dmvs.exe'  # Path to the DMVS executable
    USER = 'your_username'  # Replace with the actual user name maybe IDK

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
    #NEEDS TO BE CHECKED
    OUTFL = './v6.0/output/outfl.txt'
    SUF_LOG = './v6.0/output/logfile.txt'
    TYP_OSC = '_scope' #IDK but it's in the code

    # Log the current date and "START_MTERM"
    log_entry = f"{datetime.now()} START_MTERM\n"
    with open(os.path.join(OUTFL, SUF_LOG), 'a') as logfile:
        logfile.write(log_entry)
        print(log_entry.strip(), flush=True)  # Print to stdout

    # Run mterm.exe in the background and log its output
    mterm_process = subprocess.Popen(['./mterm.exe', '-d'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    with open(os.path.join(OUTFL, TYP_OSC + SUF_LOG), 'a') as logfile:
        for line in mterm_process.stdout:
            logfile.write(line.decode())
            print(line.decode(), end='', flush=True)  # Print to stdout

    # Sleep for 3 seconds
    time.sleep(3)

def run_stimtest():
    sc_ini = os.environ['SC_INI']

    # Sleep for 8 seconds
    time.sleep(8)

    # Run $SC_SET < tek_stim.ini
    with open('tek_stim.ini', 'r') as f:
        subprocess.run(['$SC_SET'], stdin=f)

    # Sleep for 5 seconds
    time.sleep(5)

    # Run ./mupld_c.exe -I misc_pulse.txt
    subprocess.run(['./mupld_c.exe', '-I', 'misc_pulse.txt'])

    print()
    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    log_message = f"{current_time} CALON"
    outfl_suf_log = f"{os.environ['OUTFL']}{os.environ['SUF_LOG']}"
    outfl_suf_bin = f"{os.environ['OUTFL']}{os.environ['SUF_BIN']}"
    stimtime = os.environ['STIMTIME']

    # Append to log file
    with open(outfl_suf_log, 'a') as log_file:
        log_file.write(log_message + '\n')
        log_file.write(f"save bin data to {outfl_suf_bin}\n")

    # Run ./msave.exe -s $OUTFL$SUF_BIN -e $STIMTIME
    subprocess.Popen(['./msave.exe', '-s', outfl_suf_bin, '-e', stimtime])

    # Run echo "CAL BUILD CALON " | ./mupld_c.exe
    subprocess.run(['./mupld_c.exe'], input=b"CAL BUILD CALON ")

    # Sleep for 3 seconds
    time.sleep(3)

    print()
    
    # Run echo ":q " | ./mupld_c.exe
    subprocess.run(['./mupld_c.exe'], input=b":q ")

    print()

def run_hiroplot(stimtime):
    OUTFL = './v6.0/output/outfl.txt'
    SUF_BIN = './v6.0/output/'

    print(f"Wait until STIM data collect for {stimtime} second.")
    print("\n")

    time.sleep(stimtime)
    print("\n")
   
    print("If you see orange row for first and second and rest as blue,")
    print("   STIM scan worked fine.")
    print("To quit, type :e")
    print("\n")

    subprocess.run(['./hiroplot-db.exe', '-I', f'{OUTFL}{SUF_BIN}', '-m'])


# TEST 2

# kill_mterm_mcmdr()
# upload_md()
# init_scope()
# run_mterm()

def init_misc():
    subprocess.run(['./mupld_c.exe', '-I', 'misc_scini.txt'])
    time.sleep(3)

def run_scope_cpm():
    # Log the date and SCOPE_CPM
    OUTFL_SUF_LOG = SUF_LOG
    SC_INI="./dpo4104.exe"
    with open(OUTFL_SUF_LOG, 'a') as log_file:
        log_file.write(f"{datetime.now()} SCOPE_CPM\n")
    
    # Run the sc_ini command and redirect output to _1.txt
    with open('_1.txt', 'w') as output_file:
        subprocess.run([SC_INI], stdout=output_file)
    
    # Sleep for 8 seconds
    time.sleep(8)

def set_4vsel_scope():
    SC_SET="./dpo4104_set.exe"
    with open('tek_cpmode2.ini', 'r') as input_file:
        subprocess.run([SC_SET], stdin=input_file)

def run_mcmdr():
    # Print messages to the console
    print("Now you are directory connected to MISC.")
    print("type 4VSEL ? to see what is the current setting")
    print("watch oscilloscope to minimize the saw tooth with adjusting 4VSEL value")
    print("to change 4VSEL, type .")
    print("number 4VSEL !")
    print("CMD!")
    print("number should be something near 45.")
    print("To quit from this mode, type :q")
    
    # Sleep for 3 seconds
    time.sleep(3)
    
    # Run the mcmdr.exe command with the -s flag
    subprocess.run(['./mcmdr.exe', '-s'])

def save_4vsel():
    outfl2_v4vsel = OUTFL2 + V4VSEL  # Example output file

    # Prompt the user for input
    optimal_4vsel = input("Enter optimal 4VSEL value: ")

    # Write the input to the output file
    with open(outfl2_v4vsel, 'w') as output_file:
        output_file.write(optimal_4vsel)