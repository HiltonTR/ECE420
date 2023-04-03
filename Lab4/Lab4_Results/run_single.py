import subprocess

def getElapsedTimeAndNode(out):
    templist = out.splitlines()
    ls = []
    ls.append(templist[0][24:])
    ls.append(templist[1][13:-1])
    return ls

def average(times):
    return float(sum(times)/len(times))

cmd_make_clean = ['make', 'clean']
cmd_make_everything = ['make', 'everything']
problem_sizes = ['5300', '13000', '18789']

num_run = 10

# clean and compile everything
subprocess.call(cmd_make_clean, cwd ='../Development_Kit_Lab4')
subprocess.call(cmd_make_everything, cwd ='../Development_Kit_Lab4')

filename = 'single_results_' + str(num_run) + '.txt'
file = open(filename, 'w')
file.write("run single " + str(num_run) + " times\n\n")

# loop problem sizes
for size in problem_sizes:
    times = []

    cmd_datatrim = ['./datatrim', '-b', size]
    datatrim_out = subprocess.check_output(cmd_datatrim, cwd ='../Development_Kit_Lab4')
    print(datatrim_out)
    file.write(datatrim_out)

    # run the program 10 times or as specified
    for i in range(num_run):
        cmd_main = ['./main']
        out = subprocess.check_output(cmd_main, cwd ='../Development_Kit_Lab4')
        time = float(getElapsedTimeAndNode(out)[1])
        print(time)
        times.append(time)
        file.write(str(time) + '\n')

    calc_avg = average(times)

    print("\naverage:\n")
    print(str(calc_avg) + '\n\n\n')

    file.write("\naverage:\n")
    file.write(str(calc_avg) + '\n\n\n')

file.close()