def num_to_binary_array(num, byte_len):
    ret = ""
    for _ in range(byte_len):
        ret = str(num & 0x1) + "b," + ret
        num >>= 1
    return "[" + ret[:-1] + "]"

def num_to_binary_string(num, byte_len):
    ret = ""
    for _ in range(byte_len):
        ret = str(num & 0x1) + ret
        num >>= 1
    return ret

if __name__ == "__main__":
    while True:
        num = int(input("Enter a number: "))
        print(num_to_binary_array(num, 32))