import pandas as pd 
import os

def main():
    print('Please supply the file name to be converted: ')
    print('*** Note: Data must be txt file in format: TIMEID UNIXTIME BID ASK BIDSIZE ASKSIZE ***')
    print('*** Additionally, make sure that the file to be formatted is within the /data directory. ***')
    path = input('Input file path: ')
    split_file(path)

def split_file(path):
    file_path = './data/' + path
    if not os.path.exists(file_path):
        print(f"FILE DOES NOT EXIST AT PATH: {path}")
        return
    df = pd.read_csv(file_path, sep='\s+', header=None, names = ['TIMEID','UNIXTIME','BID','ASK','BIDSIZE','ASKSIZE'])
    symbol = path[:4] # should use rstrip, and error handle incorrect file types
    column_to_file = {
            'TIMEID': f'./data/{symbol}_TIMEID.txt',
            'UNIXTIME': f'./data/{symbol}_UNIXTIME.txt',
            'BID': f'./data/{symbol}_BID.txt',
            'BIDSIZE': f'./data/{symbol}_BIDSIZE.txt',
            'ASK': f'./data/{symbol}_ASK.txt',
            'ASKSIZE': f'./data/{symbol}_ASKSIZE.txt'
            }
    for col, filename in column_to_file.items():
        with open(filename, 'w') as file:
            for data in df[col]:
                file.write(f'{data}\n')
    print("Success!")

if __name__=='__main__':
    main()
