import numpy as np
import time
import os

DOUBLE = 8
DOUBLE_COUNT = 18
INT = 8
INT_COUNT = 1

BYTES_TO_READ = DOUBLE * DOUBLE_COUNT + INT * INT_COUNT

def read_bin_full_file(file):
    f = open(file, 'rb')
    b = f.read(-1)

    data = []
    for i in range(0, int(os.path.getsize(file) / BYTES_TO_READ)):
        data.append(np.frombuffer(b, dtype=np.uint64, count=1, offset=i * BYTES_TO_READ).tolist() + np.frombuffer(b, dtype=np.double, count=DOUBLE_COUNT - 1, offset=BYTES_TO_READ * i + DOUBLE).tolist() + np.frombuffer(b, dtype=np.int64, count=1, offset=BYTES_TO_READ * i + DOUBLE_COUNT * DOUBLE))

    return data

start_time = time.time()
data = read_bin_full_file('./data/output/PERLUSDT.bin')

for candle in data:
    print(candle)

    time.sleep(0.3)

print(f"Difference: {(time.time() - start_time) * 1000}ms")
