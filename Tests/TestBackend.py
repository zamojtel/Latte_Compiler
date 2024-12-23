

import os
from typing import List


def compare_results(input_files : List[str],output_dir : str,expected_output_dir : str):
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
            
        f1.close()
        f2.close()

def check_tests(input_file_names : List[str],input_dir : str,output_dir : str):
    cwd = os.getcwd()
    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        generated_llvm = f'{output_dir}/{input_file_name}.ll'
        test = input_dir+'/'+input_file_name+'.lat'
    
        generate_files_cmd = executable + ' ' +test
        processed_llvm_file = f'{output_dir}/{input_file_name}.bc'
        final_command = generate_files_cmd+f" > {generated_llvm}"
        os.system(final_command)

        run_llvm_as = f'llvm-as {generated_llvm} -o {processed_llvm_file}'  

        os.system(run_llvm_as)

        output_test = f"{output_dir}/{input_file_name}.output"
        stdinfile = f"{input_dir}/{input_file_name}.input"

        run_lli=None
        if os.path.exists(stdinfile):
            run_lli = f'lli {processed_llvm_file} < {stdinfile} > {output_test}'
        else:
            run_lli = f'lli {processed_llvm_file} > {output_test}'

        os.system(run_lli)
    
    compare_results(input_file_names,input_dir,output_dir)
    print()


def collect_all_files(path : str):
    all_files = os.listdir(path)
    all_files.sort()

    input_file_names = []
    for j in range(len(all_files)):
        if all_files[j][-3:]=='lat':
            input_file_names.append(all_files[j][:-4])
    
    return input_file_names

if __name__ == '__main__':
    cwd = os.getcwd()

    # input_file_names_good = collect_all_files(f'{cwd}/Tests/myTests') 
    # input_dir_good = f'{cwd}/Tests/myTests'
    # output_dir_good = f'{cwd}/Tests/myTestsOutputs'

    # os.system(f'mkdir -p {cwd}/Tests/myTestsOutputs')

    # input_file_names_good = collect_all_files(f'{cwd}/Tests/myGood') 
    # input_dir_good = f'{cwd}/Tests/myGood'
    # output_dir_good = f'{cwd}/Tests/myGoodOutputs'

    # os.system(f'mkdir -p {cwd}/Tests/myGoodOutputs')


    # input_file_names_good = collect_all_files(f'{cwd}/Tests/good') 
    # input_dir_good = f'{cwd}/Tests/good'
    # output_dir_good = f'{cwd}/Ready/good'

    # os.system(f'mkdir -p {cwd}/Ready/good')

    # # # Test good
    # # check_tests(input_file_names_good,input_dir_good,output_dir_good)
    # check_tests(input_file_names_good,input_dir_good,output_dir_good)

    # Test NewTests 
    os.system(f'mkdir -p {cwd}/Tests/myTests/NewTestsOutputs')
    
    input_file_names_newtests = collect_all_files(f'{cwd}/Tests/myTests/NewTests') 
    input_dir_newtests = f'{cwd}/Tests/myTests/NewTests'
    output_dir_newtests = f'{cwd}/Tests/myTests/NewTestsOutputs'
    check_tests(input_file_names_newtests,input_dir_newtests,output_dir_newtests)

    