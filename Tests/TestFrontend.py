

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
            raise 1
        f1.close()
        f2.close()

def check_bad_tests(input_file_names : List[str],input_dir : str,output_dir : str):
    cwd = os.getcwd()
    passed = 0
    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.lat'

        cmd = executable + ' ' +test
        output_test = f"{output_dir}/{input_file_name}.output"
        cmd += ' > '+f'{output_test}'
        os.system(cmd)

        path = f'{output_test}'
        f1 = open(path, "r")
        first_line = f1.readline()
        if first_line!="ERROR\n":
            print(f'Test: {input_file_name} not passed!!!')
        else:
            passed+=1
            print(f'Test: {input_file_name} passed!!!')

        f1.close()
        
    print(f"Total number of tests passed {passed}/{len(input_file_names)}")
    print()

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

    # input_file_names_ourBad =  collect_all_files(f'{cwd}/Tests/myBad')

    # input_file_names_good = collect_all_files(f'{cwd}/Tests/good') 
    # input_dir_good = f'{cwd}/Tests/good'
    # output_dir_good = f'{cwd}/Tests/goodOutputs'

    # os.system(f'mkdir -p {cwd}/Tests/goodOutputs')

    # Test All provided basic tests:
    print("BadBasic tests:")
    input_file_names_bad = collect_all_files(f'{cwd}/Tests/bad/general')
    input_dir_bad = f'{cwd}/Tests/bad/general'
    output_dir_bad = f'{cwd}/Tests/badOutputs/general'

    os.system(f'mkdir -p {output_dir_bad}')
    check_bad_tests(input_file_names_bad,input_dir_bad,output_dir_bad)

    # Test All bad/semantic tests: 
    print("BadSemantic tests:")
    input_file_names_bad_semantic = collect_all_files(f'{cwd}/Tests/bad/semantic')
    input_dir_bad_semantic = f'{cwd}/Tests/bad/semantic'
    output_dir_bad_semantic = f'{cwd}/Tests/badOutputs/semantic'

    os.system(f'mkdir -p {output_dir_bad_semantic}')
    check_bad_tests(input_file_names_bad_semantic,input_dir_bad_semantic,output_dir_bad_semantic)
    # check_bad_tests(["declaration_in_if_condition"],input_dir_bad_semantic,output_dir_bad_semantic)

    # Test bad/myBadTests tests:
    print("myBadTests tests:")
    input_file_names_bad_myBadTests = collect_all_files(f'{cwd}/Tests/bad/myBadTests')
    input_dir_bad_myBadTests = f'{cwd}/Tests/bad/myBadTests'
    output_dir_bad_myBadTests = f'{cwd}/Tests/badOutputs/myBadTests'

    os.system(f'mkdir -p {output_dir_bad_myBadTests}')
    check_bad_tests(input_file_names_bad_myBadTests,input_dir_bad_myBadTests,output_dir_bad_myBadTests)

    # input_dir_myGood = f'{cwd}/Tests/myGood'
    # input_file_names_myGood = collect_all_files(input_dir_myGood)
    # output_dir_myGood = f'{cwd}/Tests/myGoodOutputs'

    # os.system(f'mkdir -p {output_dir_myGood}')

    # input_dir_myBad = f'{cwd}/Tests/myBad'
    # input_file_names_myBad = collect_all_files(input_dir_myBad)
    # output_dir_myBad = f'{cwd}/Tests/myBadOutputs'

    # os.system(f'mkdir -p {output_dir_myBad}')
    
    # Test good
    # check_good_tests(input_file_names_good,input_dir_good,output_dir_good)
    # Test myGood
    # print("myGood tests:")
    # check_good_tests(input_file_names_myGood,input_dir_myGood,output_dir_myGood)
    # Test bad
    # Test myBad
    # print("myBad tests:")
    # check_bad_tests(input_file_names_myBad,input_dir_myBad,output_dir_myBad)
