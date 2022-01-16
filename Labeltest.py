import pandas as pd
import os

data_location = "/data/ssd/Coding/Research-Project-Crypto/Data/Crypto/1MIN/"

directory = os.fsencode(data_location)
    


def process_file(df):
    print(df.head())




for file in os.listdir(directory):
     filename = os.fsdecode(file)
     if filename.endswith(".csv"): 
         df = pd.read_csv(os.path.join(data_location, filename))
         process_file(df)
         continue
     else:
         continue

