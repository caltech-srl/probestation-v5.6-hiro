import psutil

def kill_process(process_name):
    """Kill all processes matching the given process name."""
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == process_name:
            #print(f"Killing {process_name} process with PID {proc.info['pid']}")
            proc.kill()
        else:
            print(f"{process_name} not found")

def kill_mterm_mcmdr():
    # Kill all mterm processes
    kill_process('mterm')

    # Kill all mcmdr processes
    kill_process('mcmdr')

def conf_4vsel():
    # lines 237-245
    if "this exists":
        v4vsel = "this"
    else:
        v4vsel = 45
