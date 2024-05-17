CC = c++ `xapian-config --libs --cxxflags`
OBJ = metadata_indexer metadata_search

all: $(OBJ)
metadata_indexer: metadata_indexer.cc
	$(CC) -o $@ $<

metadata_search: metadata_search.cc
	$(CC) -o $@ $<
