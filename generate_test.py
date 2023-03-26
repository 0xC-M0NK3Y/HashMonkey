import random
import string

def get_random_string(len):
    letters = string.ascii_letters + string.digits + string.punctuation
    result_str = ''.join(random.choice(letters) for i in range(len))
    if '\n' in result_str:
        return get_random_string(len)
    return result_str+'\n'

file = open("testset.txt", 'w')

for i in range(10000000):
    file.write(get_random_string(random.randint(2, 50)))

file.close()