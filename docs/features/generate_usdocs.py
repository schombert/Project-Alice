import asyncio
import os
from datetime import datetime
import re

# This script parses all source files of the game and generates a single concise User Stories MD document from comments marks
# Use // US[id] to declare a User Story
# Use // US[id]AC[id] to declare an Acceptance Criteria

# Run the file with CWD pointing to the Project Alice repository root
# From VS Code: With Project Folder opened, Right Mouse Button -> Run Python File in terminal or F5 for debug start

cwd = os.getcwd()
outputfpath = os.path.join(cwd, "docs/features/UserStoriesGenerated.md")

ignore_folders = [".git"]
ignorefiles = ["UserStoriesGenerated.md"]
allowed_extensions = ["txt", "py", "cs", "h", "hpp", "cpp", "c", "gd", "md"]

data = {}

def EMPTY_US():
    return { "name":"", "acceptance_criteria": {} }

def add_us(usnumber, name):
    usid = int(usnumber)
    if usid in data:
        data[usid]["name"] = name
    else:
        data[usid] = EMPTY_US()
        data[usid]["name"] = name
        
def add_ac(usnumber, acnumber, name):
    usid = int(usnumber)
    acid = int(acnumber)
    if usid in data:
        data[usid]["acceptance_criteria"][acid] = name
    else:
        data[usid] = EMPTY_US()
        data[usid]["acceptance_criteria"][acid] = name

def compile_data():
    res = "# User Stories\n"
    res += "*Automatically generated file on " + datetime.today().strftime('%Y-%m-%d') + "*\n"
    
    uslist = sorted(data.keys())
    
    for usid in uslist:
        us = data[usid]
        res += f"\n## US{usid}. {us["name"]}\n\n"

        aclist = sorted(us["acceptance_criteria"].keys())
        
        for acid in aclist:
            ac = us["acceptance_criteria"][acid]
            res += f"| AC{acid} | {ac} |\n"
    return res

async def process_folder(path):
    fileslist = os.listdir(path)
    
    async with asyncio.TaskGroup() as tg:
        for k in fileslist:
            if k in ignore_folders:
                continue
            if k in ignorefiles:
                continue
            if (os.path.isfile(os.path.join(path, k))):
                extstr = k.split(".")[-1]
                if extstr not in allowed_extensions:
                    continue
                try:
                    process_file(path, k)
                except Exception as e:
                    print(e)
            else:
                newpath = os.path.join(path, k)
                tg.create_task(process_folder(newpath))

def process_file(folderpath, fname):
    fpath = os.path.join(folderpath, fname)
    
    print("Processing file" + fpath)
    
    f = open(fpath)
    
    while True:
        line = f.readline()
        if not line:
            break
        
        usmatch = re.search("(//|;|#)[ ]*US([0-9]+)[ .]([a-zA-z0-9.,' %]+)", line)
        if usmatch:
            print(line)
            commenttype = usmatch.group(1)
            usnumber = usmatch.group(2)
            usname = usmatch.group(3)

            print(commenttype, usnumber, usname)
            add_us(usnumber, usname)
        
        acmatch = re.search("(//|;|#)[ ]*US([0-9]+)AC([0-9]+)[ .]([a-zA-z0-9.,' %]+)", line)
        if acmatch:
            print(line)
            commenttype = acmatch.group(1)
            usnumber = acmatch.group(2)
            acnumber = acmatch.group(3)
            acname = acmatch.group(4)

            print(commenttype, usnumber, acnumber, acname)
            add_ac(usnumber, acnumber, acname)
    
    f.close()
    
start_time = datetime.now()

r = asyncio.run(process_folder(cwd))
        
output = compile_data()
with open(outputfpath, "w", encoding="utf-8") as f:
    f.write(output)

time_elapsed = datetime.now() - start_time 
print('Extras Time elapsed (hh:mm:ss.ms) {}'.format(time_elapsed))
