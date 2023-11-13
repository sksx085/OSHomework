"""
def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        content1 = f1.read()
        content2 = f2.read()

    if content1 == content2:
        print("两个文件内容完全相同。")
    else:
        print("两个文件内容不同。")

# 用法示例
file1 = 'lab05/out.txt'  # 替换为你的第一个文件名
file2 = 'lab05/standard answer.txt'  # 替换为你的第二个文件名

compare_files(file1, file2)
"""
import re
def compare_files(file1, file2):
    flag = 1
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        lines1 = f1.readlines()
        lines2 = f2.readlines()

    # 确保两个文件行数相同
    if len(lines1) != len(lines2):
        print("行数不同，文件不匹配")
        return

    for i, (line1, line2) in enumerate(zip(lines1, lines2), 1):
        # 提取每行的数字（可能为负数）
        numbers1 = [int(match.group()) for match in re.finditer(r'-?\d+', line1)]
        numbers2 = [int(match.group()) for match in re.finditer(r'-?\d+', line2)]

        # 比较数字是否相同
        if numbers1 != numbers2:
            print(f"第 {i} 行不同:")
            print(f"文件1: {line1.strip()}")
            print(f"文件2: {line2.strip()}")
            print()
            flag = 0
    if flag == 1:
        print("完全一致！！！恭喜完成实验！！！")

# 用法示例
file1 = '/Users/sksx085/Desktop/各种实验报告等狗屎/OSHomework/exp5/lab05/out.txt'  # 你的文件1
file2 = '/Users/sksx085/Desktop/各种实验报告等狗屎/OSHomework/exp5/lab05/standard answer.txt'  # 你的文件2

compare_files(file1, file2)
