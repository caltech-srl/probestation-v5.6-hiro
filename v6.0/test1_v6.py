from functions import kill_mterm_mcmdr
import time


while True:
    #Choose Test
    print("Test options:")
    print("1) Power On Test     2) 4VSEL Test     3) Full Scan Test     4) Scope Test")
    test = input("Choose test: ")

    try:
        test = int(test)
        if test in [1,2,3,4]:
            break
        else:
            print("Test not found. Please select a number between 1 and 4")
    except:
        print("Please enter a number between 1 and 4.")

#Power On Test
if test == 1:
    STIMTIME = 60
    
    kill_mterm_mcmdr()
    

#4VSEL Test
elif test == 2:
    kill_mterm_mcmdr()


#Full Scan
elif test == 3:
    kill_mterm_mcmdr()

#Scope Test ?????????
elif test == 4:
    print("idk")

