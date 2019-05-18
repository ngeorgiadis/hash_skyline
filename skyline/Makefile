OBJS	= ApplicationException.o BinaryDataReader.o CommandlineArgs.o Common.o DataReader.o DSH.o FvecReader.o FvecWriter.o HashingBase.o ITQ.o KMeans.o Logger.o LSH.o skyline.o SphericalHashing.o stdafx.o TextWriter.o
SOURCE	= ApplicationException.cpp BinaryDataReader.cpp CommandlineArgs.cpp Common.cpp DataReader.cpp DSH.cpp FvecReader.cpp FvecWriter.cpp HashingBase.cpp ITQ.cpp KMeans.cpp Logger.cpp LSH.cpp skyline.cpp SphericalHashing.cpp stdafx.cpp TextWriter.cpp
HEADER	= ApplicationException.h BinaryDataReader.h CommandlineArgs.h Common.h DataReader.h DSH.h FvecReader.h FvecWriter.h HashingBase.h ITQ.h KMeans.h Logger.h LSH.h SphericalHashing.h stdafx.h targetver.h TextWriter.h
OUT	= skyline
CC	 = g++
FLAGS	 = -g -c -Wall -O3
LFLAGS	 = -lboost_filesystem -lboost_system -larmadillo

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ApplicationException.o: ApplicationException.cpp
	$(CC) $(FLAGS) ApplicationException.cpp -std=c11

BinaryDataReader.o: BinaryDataReader.cpp
	$(CC) $(FLAGS) BinaryDataReader.cpp -std=c11

CommandlineArgs.o: CommandlineArgs.cpp
	$(CC) $(FLAGS) CommandlineArgs.cpp -std=c11

Common.o: Common.cpp
	$(CC) $(FLAGS) Common.cpp -std=c11

DataReader.o: DataReader.cpp
	$(CC) $(FLAGS) DataReader.cpp -std=c11

DSH.o: DSH.cpp
	$(CC) $(FLAGS) DSH.cpp -std=c11

FvecReader.o: FvecReader.cpp
	$(CC) $(FLAGS) FvecReader.cpp -std=c11

FvecWriter.o: FvecWriter.cpp
	$(CC) $(FLAGS) FvecWriter.cpp -std=c11

HashingBase.o: HashingBase.cpp
	$(CC) $(FLAGS) HashingBase.cpp -std=c11

ITQ.o: ITQ.cpp
	$(CC) $(FLAGS) ITQ.cpp -std=c11

KMeans.o: KMeans.cpp
	$(CC) $(FLAGS) KMeans.cpp -std=c11

Logger.o: Logger.cpp
	$(CC) $(FLAGS) Logger.cpp -std=c11

LSH.o: LSH.cpp
	$(CC) $(FLAGS) LSH.cpp -std=c11

skyline.o: skyline.cpp
	$(CC) $(FLAGS) skyline.cpp -std=c11

SphericalHashing.o: SphericalHashing.cpp
	$(CC) $(FLAGS) SphericalHashing.cpp -std=c11

stdafx.o: stdafx.cpp
	$(CC) $(FLAGS) stdafx.cpp -std=c11

TextWriter.o: TextWriter.cpp
	$(CC) $(FLAGS) TextWriter.cpp -std=c11


clean:
	rm -f $(OBJS) $(OUT)