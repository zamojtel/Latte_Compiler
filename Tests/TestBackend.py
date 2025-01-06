import os
from typing import List

def compare_results(input_files : List[str],output_dir : str,expected_output_dir : str):
    passed = 0
    for output_file_name in input_files:
        path = f'{output_dir}/{output_file_name}.output'
        f1 = open(path, "r")
        output = f1.read()

        path = f'{expected_output_dir}/{output_file_name}.output'
        f2 = open(path, "r")
        expected_output = f2.read()

        if output == expected_output:
            print(f'Test: {output_file_name} passed !!!')
            passed+=1
        else:
            print(f'Test: {output_file_name} not passed!!!')
        
        f1.close()
        f2.close()
    print(f"Total number of tests passed {passed}/{len(input_files)}")

def check_tests(input_file_names : List[str],input_dir : str,output_dir : str):
    cwd = os.getcwd()
    func_bc = f"{cwd}/lib/functions_14.bc"

    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        generated_llvm = f'{output_dir}/{input_file_name}.ll'
        test = input_dir+'/'+input_file_name+'.lat'
    
        generate_files_cmd = executable + ' ' +test
        processed_llvm_file = f'{output_dir}/{input_file_name}_intermediate.bc'
        final_bc = f'{output_dir}/{input_file_name}.bc'
        final_command = generate_files_cmd+f" > {generated_llvm}"
        os.system(final_command)

        run_llvm_as = f'llvm-as-14 {generated_llvm} -o {processed_llvm_file}'  

        os.system(run_llvm_as)

        run_link_cmd = f'llvm-link-14 {processed_llvm_file} {func_bc} -o {final_bc}'
        os.system(run_link_cmd)
        
        output_test = f"{output_dir}/{input_file_name}.output"
        stdinfile = f"{input_dir}/{input_file_name}.input"

        run_lli=None
        if os.path.exists(stdinfile):
            run_lli = f'lli-14 {final_bc} < {stdinfile} > {output_test}'
        else:
            run_lli = f'lli-14 {final_bc} > {output_test}'

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

    # Required Tests:
    print("Regular Tests:")
    os.system(f'mkdir -p {cwd}/Tests/goodOutputs/general')
    input_file_names_good = collect_all_files(f'{cwd}/Tests/good/general') 
    input_dir_good = f'{cwd}/Tests/good/general'
    output_dir_good = f'{cwd}/Tests/goodOutputs/general'
    # check_tests(["aore"],input_dir_good,output_dir_good)
    # check_tests(["bore000"],input_dir_good,output_dir_good)
    check_tests(input_file_names_good,input_dir_good,output_dir_good)
    
    # More Tests (Some of them are more complex):
    # Basic Tests:
    print("Basic Tests: ")
    os.system(f'mkdir -p {cwd}/Tests/goodOutputs/basic')
    input_file_names_basic = collect_all_files(f'{cwd}/Tests/good/basic') 
    input_dir_basic = f'{cwd}/Tests/good/basic'
    output_dir_basic = f'{cwd}/Tests/goodOutputs/basic'
    check_tests(input_file_names_basic,input_dir_basic,output_dir_basic)

    # # Test NewTests 
    # # os.system(f'mkdir -p {cwd}/Tests/myTests/NewTestsOutputs')
    
    # # input_file_names_newtests = collect_all_files(f'{cwd}/Tests/myTests/NewTests') 
    # # input_dir_newtests = f'{cwd}/Tests/myTests/NewTests'
    # # output_dir_newtests = f'{cwd}/Tests/myTests/NewTestsOutputs'
    # # check_tests(input_file_names_newtests,input_dir_newtests,output_dir_newtests)
    
    # Test BasicPredifinedFunctions
    print("BasicPredifinedFunctions: ") 
    os.system(f'mkdir -p {cwd}/Tests/myTests/BasicPredifinedFunctionsFunctionalitiesOutputs')
    
    input_file_names_func = collect_all_files(f'{cwd}/Tests/myTests/BasicPredifinedFunctionsFunctionalities') 
    input_dir_func = f'{cwd}/Tests/myTests/BasicPredifinedFunctionsFunctionalities'
    output_dir_func = f'{cwd}/Tests/myTests/BasicPredifinedFunctionsFunctionalitiesOutputs'
    check_tests(input_file_names_func,input_dir_func,output_dir_func)

    # test someTests 
    print("My Tests: ")
    os.system(f'mkdir -p {cwd}/Tests/myTests/someTestsOutputs')
    
    input_file_names_newtests = collect_all_files(f'{cwd}/Tests/myTests/someTests') 
    input_dir_newtests = f'{cwd}/Tests/myTests/someTests'
    output_dir_newtests = f'{cwd}/Tests/myTests/someTestsOutputs'
    check_tests(input_file_names_newtests,input_dir_newtests,output_dir_newtests)
    check_tests(["OneReadInt_TworeadStrings"],input_dir_newtests,output_dir_newtests)

    print("BooleanExpressions: ") 
    os.system(f'mkdir -p {cwd}/Tests/myTests/BooleanExpressionsOutputs')
    
    input_file_names_booleans = collect_all_files(f'{cwd}/Tests/myTests/BooleanExpressions') 
    input_dir_booleans = f'{cwd}/Tests/myTests/BooleanExpressions'
    output_dir_booleans = f'{cwd}/Tests/myTests/BooleanExpressionsOutputs'
    check_tests(input_file_names_booleans,input_dir_booleans,output_dir_booleans)
