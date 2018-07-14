import sys
import csv


class DataParser:
    
    # constructor
    def __init__(self, filename):
        self.filename = filename
        self.line_cnt = 0
        self.data = []
        self.content = []
        self.original_time = 8080
        self.openfile()
        
    def openfile(self):
        with open(self.filename) as f:
            reader = f.readlines()
        self.content = [x.strip() for x in reader] 
        self.line_cnt = len(self.content)
        self.original_time = int(self.content[1][self.content[1].find("...") + 3:])
        print("Successfully open the file" + str(self.filename) + " with "+ str(self.line_cnt) + " data.")
        self.read()
        
    def read(self):
        print("Start reading...")
        for i in range(self.line_cnt):
            if((i %2) == 0):
                inflow = self.content[i].find("InFlow") +  len("InFlow is:")
                outflow = self.content[i].find("OutFlow") +  len("OutFlow is:")
                f1 = self.content[i].find("F1") + len("F1 is:")
                f2 = self.content[i].find("F2") + len("F2 is:")
            else:
                row = []
                time = self.content[i].find("...") + len("...")
                row.append(float(self.content[i-1][inflow:inflow+5]))
                row.append(float(self.content[i-1][outflow:outflow+5]))
                row.append(float(self.content[i-1][f1:f1+5]))
                row.append(float(self.content[i-1][f2:f2+5]))
                row.append(int(self.content[i][time:]) - self.original_time)
                self.data.append(row)
                print("reading...%.0f%%\r" % (i*100.0//self.line_cnt))
                sys.stdout.flush()
        print("Done reading!")


    def save_csv(self):
        print("Save to csv")
        with open(self.filename + ".csv", "wb") as f:
            writer = csv.writer(f)
            writer.writerows(self.data)

def main():
    if len(sys.argv) < 2:
        print "Usage:", sys.argv[0], "<filename>"
        sys.exit(1)
        
    fileName = sys.argv[1]
    parser = DataParser(fileName)
    parser.save_csv()
            
if __name__ == "__main__":
    main()
        