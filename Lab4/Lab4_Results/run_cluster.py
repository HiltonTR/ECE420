import subprocess

def getElapsedTimeAndNode(out):

    # nodecount is initially: 1112
    # nodecount is initially: 1112
    # nodecount is initially: 1112
    # nodecount is initially: 1112
    # Elapsed time 0.047963.
    # 1112

    templist = out.splitlines()
    ls = []
    ls.append(templist[len(templist)-2][13:-1])
    ls.append(templist[len(templist)-1])
    return ls

def average(times):
    return float(sum(times)/len(times))

# (cd ~/Development_Kit_Lab4 && make clean)
cmd_make_clean = ['make', 'clean']
cmd_make_everything = ['make', 'everything']
problem_sizes = ['5300', '13000', '18789']
hosts = ['192.168.1.116', '192.168.1.175', '192.168.1.53']

num_process = 8
num_run = 10

# clean and compile everything
subprocess.call(cmd_make_clean, cwd ='../Development_Kit_Lab4')
subprocess.call(cmd_make_everything, cwd ='../Development_Kit_Lab4')

filename = 'cluster_results_' + str(num_process) + '_' + str(num_run) + '.txt'
file = open(filename, 'w')
file.write("run cluster " + str(num_process) + " processes " + str(num_run) + " times\n\n")

# loop problem sizes
for size in problem_sizes:
    times = []
    cmd_datatrim = ['./datatrim', '-b', size]
    datatrim_out = subprocess.check_output(cmd_datatrim, cwd ='../Development_Kit_Lab4')
    print(datatrim_out)
    file.write(datatrim_out)

    # copy over to hosts
    for host in hosts:
        cmd_scp = ['scp', '-r', '../Development_Kit_Lab4', host+':~/.']
        subprocess.call(cmd_scp, cwd = "./")

    # run the program 10 times or as specified 
    for i in range(num_run):
        cmd = ['mpirun', '-np', str(num_process), '-f', '../hosts', './main']
        out = subprocess.check_output(cmd, cwd ='../Development_Kit_Lab4')
        time = float(getElapsedTimeAndNode(out)[0])
        print(time)
        times.append(time)
        file.write(str(time) + '\n')

    calc_avg = average(times)

    print("\naverage:\n")
    print(str(calc_avg) + '\n\n\n')

    file.write("\naverage:\n")
    file.write(str(calc_avg) + '\n\n\n')

file.close()