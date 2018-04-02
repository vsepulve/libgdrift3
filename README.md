# libgdrift3.0
Repository for the simulation library libgdrift v3.0

Build
-----
```
git clone https://github.com/vsepulve/libgdrift3
mkdir libgdrift3/build
cd libgdrift3/build
cmake ..
make
./bin/threads_test_serialized ../data/settings_scenario_a_f0.json 100 4 data_a_f0_
```

In that test "../data/settings_scenario_a_f0.json" is the settings json for a particular scenario, "100" is the number of simulations, "4" is the number of threads to be used and "data_a_f0_" is the base used by each thread to store its results (so thread 0 will generate a file named "data_a_f0_0.txt").
