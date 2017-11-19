from multiprocessing import Pool
import time

def square(x):
    time.sleep(x)
    print(x)


dataset = [1, 2, 3, 4, 5]
p = Pool(5)
p.map(square, dataset)
