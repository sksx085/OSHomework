import re

def extract_numbers(input_filename, output_filename):
    with open(input_filename, 'r') as input_file:
        lines = input_file.readlines()

    with open(output_filename, 'w') as output_file:
        for line in lines:
            # 使用正则表达式提取数字（包括负数）
            numbers = [int(match.group()) for match in re.finditer(r'-?\d+', line)]
            
            # 保证每行有三个数字
            if len(numbers) == 3:
                output_line = ','.join(map(str, numbers)) + '\n'
                output_file.write(output_line)
            else:
                print(f"Warning: Skipped line with less than three numbers: {line.strip()}")

# 用法示例
input_filename = 'lab05/output.txt'  # 你的输入文件名
output_filename = 'lab05/out.txt'  # 你的输出文件名

extract_numbers(input_filename, output_filename)
