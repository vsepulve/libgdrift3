# libgdrift3.0
Repository for the simulation library libgdrift v3.0, using nlohmann JSON for Modern C++ library (https://github.com/nlohmann/json)

Dependencies
-----
```
git clone https://github.com/nlohmann/json
mkdir json/build
cd json/build
cmake ..
make
sudo make install
```

Build
-----
```
git clone https://github.com/vsepulve/libgdrift3
mkdir libgdrift3/build
cd libgdrift3/build
cmake ..
make
```

Testing
-----
```
./bin/threads_test 100 4 ../data/project_B.json ../data/simulation_B.json data_ target_ results_
```

In this example "100" is the number of simulations, "4" is the number of threads used for processing, "../data/project\_B.json" and "../data/simulation\_B.json" defines the simulation. The string "data\_" is the base for the file used to store the data for each simulation, for example, file "data\_21\_f0\_0.txt" will be created for simulation id 21 (defined in "../data/simulation\_B.json"), f0 stands for feedback 0 (this program only process feedback 0), and the last number for the thread 0 (additional files for thread 1, 2 and 3 will also be created). Parameter "target\_" defines the file created with the target, in this case "target\_2.txt" for project id 2 (defined in "../data/project\_B.json"), the last parameter is not used yet for this program.

Analyzing
-----
```
./bin/generate_statistics data_21_f0_ 4 135 30 target_2.txt 0.05 distributions_B.txt results_B.txt
```

In this program "data\_21\_f0\_" is the base to read the data for each processing thread, "4" is the number of used threads, "135" is the number of statistics used in this project (3 stats, for 9 markers, for 3 populations including summary), "30" is the number of parameters for the simulation of "../data/simulation\_B.json" (including the 9 mutations rates definted in the project json), "target\_2.txt" is the target created in the previous program, "0.05" is the percentage used for top K selection (5% of the better results), and files "distributions\_B.txt" and "results\_B.txt" are used to store the results (distances and posterior distributions in the former, full results sorted by distance to the target in the latter).




