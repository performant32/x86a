from pathlib import Path

script_directory =  Path(__file__).parent.resolve()
project_root = script_directory.parent.parent;

def ask_questions():
    print("Answer y/n");
    file = open(project_root / ".config.mk", "w")
    is_debug = input("Is debug?(y)");
    file.write(f"-DCMAKE_BUILD_TYPE={is_yes(is_debug) and 'DEBUG' or 'Release'}")
    file.close() 
def is_yes(answer):
    return answer == 'y'

def get_answer(answer, default='y'):
    return answer or default

if(__name__ == "__main__"):
    ask_questions()
