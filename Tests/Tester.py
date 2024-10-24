

import os
from typing import List


def compare_results(input_files : List[str],output_dir : str,expected_output_dir : str):
    cwd = os.getcwd()

    for output_file_name in input_files:
        path = f'{output_dir}/{output_file_name}.output'
        f1 = open(path, "r")
        output = f1.read()

        path = f'{expected_output_dir}/{output_file_name}.output'
        f2 = open(path, "r")
        expected_output = f2.read()

        if output == expected_output:
            print(f'Test: {output_file_name} passed !!!')
        else:
            print(f'Test: {output_file_name} not passed!!!')
            raise 1
        f1.close()
        f2.close()

def check_bad_tests(input_file_names : List[str],input_dir : str,output_dir : str):
    cwd = os.getcwd()
    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.lat'

        cmd = executable + ' ' +test
        output_test = f"{output_dir}/{input_file_name}.output"
        cmd += ' > '+f'{output_test}'
        os.system(cmd)

        path = f'{output_test}'
        f1 = open(path, "r")
        # output = f1.read()
        first_line = f1.readline()
        if first_line!="ERROR\n":
            print(f'Test: {input_file_name} not passed!!!')
        else:
            print(f'Test: {input_file_name} passed!!!')
        

# we run and verify tests in one method 
def check_good_tests(input_file_names : List[str],input_dir : str,output_dir : str):
    cwd = os.getcwd()
    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.lat'

        cmd = executable + ' ' +test
        output_test = f"{output_dir}/{input_file_name}.output"
        cmd += ' > '+f'{output_test}'
        os.system(cmd)

        path = f'{output_test}'
        f1 = open(path, "r")
        output = f1.read()
        if output!="OK\n":
            print(f'Test: {input_file_name} not passed!!!')
        else:
            print(f'Test: {input_file_name} passed!!!')
            

if __name__ == '__main__':
    cwd = os.getcwd()
    all_files_good = os.listdir(f'{cwd}/Tests/good')
    all_files_good.sort()
    print(all_files_good)
    input_file_names_good = []
    for j in range(len(all_files_good)):
        if all_files_good[j][-3:]=='lat':
            input_file_names_good.append(all_files_good[j][:-4])
            
    all_files_bad = os.listdir(f'{cwd}/Tests/bad')
    all_files_bad.sort()
    print(all_files_bad)
    input_file_names_bad = []
    for j in range(len(all_files_bad)):
        if all_files_bad[j][-3:]=='lat':
            input_file_names_bad.append(all_files_bad[j][:-4])

    input_dir_good = f'{cwd}/Tests/good'
    output_dir_good = f'{cwd}/Tests/goodOutputs'

    os.system(f'mkdir -p {cwd}/Tests/goodOutputs')

    input_dir_bad = f'{cwd}/Tests/bad'
    output_dir_bad = f'{cwd}/Tests/badOutputs'

    os.system(f'mkdir -p {cwd}/Tests/badOutputs')

    check_good_tests(input_file_names_good,input_dir_good,output_dir_good)
    check_bad_tests(input_file_names_bad,input_dir_bad,output_dir_bad)