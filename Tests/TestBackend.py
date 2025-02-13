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

    executable = cwd + '/latc_llvm'
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.lat'
        cmd = executable +" " f'{test}'
        os.system(cmd)

        final_bc = f'{input_dir}/{input_file_name}.bc'
        output_test = f"{input_dir}/{input_file_name}.output"
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

    #TODO Required Tests: OK
    print("Regular Tests:")
    input_file_names_good = collect_all_files(f'{cwd}/Tests/good/general')
    input_dir_good = f'{cwd}/Tests/good/general'
    output_dir_good = f'{cwd}/Tests/goodOutputs/general'
    check_tests(input_file_names_good,input_dir_good,output_dir_good)

    ## TODO TESTY DODATKOWE DO PODSTAWY BACKENDU: OK
    print("Basic Tests: ")
    input_file_names_basic = collect_all_files(f'{cwd}/Tests/good/basic')
    input_dir_basic = f'{cwd}/Tests/good/basic'
    output_dir_basic = f'{cwd}/Tests/goodOutputs/basic'
    check_tests(input_file_names_basic,input_dir_basic,output_dir_basic)

    ##TODO Testowanie Predefiniowanych funkcji: OK
    print("BasicPredifinedFunctions: ")
    input_file_names_func = collect_all_files(f'{cwd}/Tests/myTests/PredifinedFunctions')
    input_dir_func = f'{cwd}/Tests/myTests/PredifinedFunctions'
    output_dir_func = f'{cwd}/Tests/myTests/PredifinedFunctionsOutputs'
    check_tests(input_file_names_func,input_dir_func,output_dir_func)

    ## TODO myTests/Tests OK
    print("My Tests: ")
    input_file_names_newtests = collect_all_files(f'{cwd}/Tests/myTests/Tests')
    input_dir_newtests = f'{cwd}/Tests/myTests/Tests'
    output_dir_newtests = f'{cwd}/Tests/myTests/TestsOutputs'
    check_tests(input_file_names_newtests,input_dir_newtests,output_dir_newtests)

    ## TODO Testowanie Wyrazen Boolowskich: OK
    print("BooleanExpressions: ")
    input_file_names_booleans = collect_all_files(f'{cwd}/Tests/myTests/BooleanExpressions')
    input_dir_booleans = f'{cwd}/Tests/myTests/BooleanExpressions'
    output_dir_booleans = f'{cwd}/Tests/myTests/BooleanExpressionsOutputs'
    check_tests(input_file_names_booleans,input_dir_booleans,output_dir_booleans)

    #TODO For testing Arrays: OK
    print("Test Arrays: ")
    input_dir_arrays = f'{cwd}/Tests/extensions/arrays1'
    output_dir_arrays = f'{cwd}/Tests/extensionsOutputs/arrays1'
    input_file_names_arrays = collect_all_files(f'{cwd}/Tests/extensions/arrays1')
    check_tests(input_file_names_arrays,input_dir_arrays,output_dir_arrays)

    # #TODO Testowanie Tablic: OK
    print("Test Arrays: ")
    input_dir_goodArrays = f'{cwd}/Tests/arrays/good'
    output_dir_goodArrays = f'{cwd}/Tests/arraysOutputs/good'
    input_file_names_goodArrays = collect_all_files(f'{cwd}/Tests/arrays/good')
    check_tests(input_file_names_goodArrays,input_dir_goodArrays,output_dir_goodArrays)

    ###TODO Testowanie Struktur moje testy: OK all
    print("Test structs: ")
    input_dir_goodStructs = f'{cwd}/Tests/struct'
    output_dir_goodStructs = f'{cwd}/Tests/structOutputs'
    input_file_names_goodStructs = collect_all_files(f'{cwd}/Tests/struct')
    check_tests(input_file_names_goodStructs,input_dir_goodStructs,output_dir_goodStructs)

    # ###TODO Podstawowe testy do struktur TESTY BENKE: OK 2 dorzucone dodatkowo all
    print("Test Structs Required")
    input_dir_structs = f'{cwd}/Tests/extensions/struct'
    output_dir_structs = f'{cwd}/Tests/extensionsOutputs/struct'
    os.system(f'mkdir -p {output_dir_structs}')
    input_file_names_structs = collect_all_files(input_dir_structs)
    check_tests(input_file_names_structs,input_dir_structs,output_dir_structs)

    # ##TODO Moje testy do tablic: OK all
    print("Test myArraysGood: ")
    input_dir_myArrays = f'{cwd}/Tests/arrays/myArraysGood'
    output_dir_myArrays = f'{cwd}/Tests/arraysOutputs/myArraysGood'
    input_file_names_myArrays = collect_all_files(f'{cwd}/Tests/arrays/myArraysGood')
    check_tests(input_file_names_myArrays,input_dir_myArrays,output_dir_myArrays)

    # # #TODO Dodatkowe testy do tablic: OK all
    print("Test additionalArrays: ")
    input_dir_myArrays = f'{cwd}/Tests/arrays/arraysAdditional'
    output_dir_myArrays = f'{cwd}/Tests/arraysOutputs/arraysAdditional'
    input_file_names_myArrays = collect_all_files(f'{cwd}/Tests/arrays/arraysAdditional')
    check_tests(input_file_names_myArrays,input_dir_myArrays,output_dir_myArrays)

    # # # TODO [MOJE TESTY] TESTOWANIE OBIEKTOW: OK 9/10 -> 10/10
    print("Test Objects with methods : ")
    input_dir_objects = f'{cwd}/Tests/objects'
    output_dir_objects = f'{cwd}/Tests/objectsOutputs'
    input_file_names_objects = collect_all_files(f'{cwd}/Tests/objects')
    check_tests(input_file_names_objects,input_dir_objects,output_dir_objects)

    # #####TODO TESTS STRINGS: OK
    print("Test strings : ")
    input_dir_objects = f'{cwd}/Tests/strings'
    output_dir_objects = f'{cwd}/Tests/stringsOutputs'
    input_file_names_objects = collect_all_files(f'{cwd}/Tests/strings')
    check_tests(input_file_names_objects,input_dir_objects,output_dir_objects)

    #TODO TESTY OD BENKE OBJECTS1: OK ale nie wszystko przechodzi 2/4 -> Po poprawkach 4/4
    print("Test objects1 : ")
    input_dir_objects = f'{cwd}/Tests/extensions/objects1'
    output_dir_objects = f'{cwd}/Tests/extensionsOutputs/objects1'
    input_file_names_objects = collect_all_files(input_dir_objects)
    check_tests(input_file_names_objects,input_dir_objects,output_dir_objects)


    # TODO TESTY OD BENKE OBJECTS2: 0/1 nie przechodzi
    print("Test objects2  : ")
    input_dir_objects2 = f'{cwd}/Tests/extensions/objects2'
    output_dir_objects2 = f'{cwd}/Tests/extensionsOutputs/objects2'
    input_file_names_objects2 = collect_all_files(input_dir_objects2)
    check_tests(input_file_names_objects2,input_dir_objects2,output_dir_objects2)

    # #TODO DODATKOWE TESTY ZE STRONY DO METOD WIRTUALNYCH: 0/1
    print("Test virtual:")
    input_dir_virtual = f'{cwd}/Tests/virtual'
    output_dir_virtual = f'{cwd}/Tests/virtualOutputs'
    input_file_names_virtual = collect_all_files(input_dir_virtual)
    check_tests(input_file_names_virtual,input_dir_virtual,output_dir_virtual)

    # print("Test Poprawki:")
    # input_dir_poprawki = f'{cwd}/Tests/Poprawki'
    # output_dir_poprawki = f'{cwd}/Tests/PoprawkiOutputs'
    # input_file_names_poprawki = collect_all_files(input_dir_poprawki)

    # check_tests(input_file_names_poprawki,input_dir_poprawki,output_dir_poprawki)
